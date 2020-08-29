/*
  xdrv_36_keeloq.ino - Jarolift Keeloq shutter support for Tasmota

  Copyright (C) 2020  he-so

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_SOMFY
/*********************************************************************************************\
 * Keeloq shutter support
 *
 * Uses hardware SPI and two user configurable GPIO's (CC1101 GDO0 and CC1101 GDO2)
 *
 * Considering the implementation these two user GPIO's are fake.
 * Only CC1101 GDO0 is used and must always be GPIO05 dictated by the used CC1101 library.
\*********************************************************************************************/

#define XDRV_70 70

#include <HopeDuino_CMT211xA.h>

const char kSomfyCommands[] PROGMEM = "Somfy|" // prefix
  "Up|Down|Stop|Prog|Control|Set";

void (* const somfyCommand[])(void) PROGMEM = {
  &CmdUp, &CmdDown, &CmdStop, &CmdProg, &CmdControl, &CmdSet};

void SendCommand(byte *frame, byte sync);
void BuildFrame(byte *frame, byte button, unsigned long address);

cmt211xaClass rfm119 = cmt211xaClass();
word CfgTbl[21] = {
        0x007F,
        0x5400,
        0x0000,
        0x0000,
        0x0000,
        0xF000,
        0x0000,
        0xAE14,
        0x4200,
        0x0000,
        0x2401,
        0x01B0,
        0x8000,
        0x0007,
        0xFFFF,
        0x0020,
        0x5FD9,
        0x22D6,
        0x0E13,
        0x0019,
        0x2000};

#define SYMBOL 640

void CmdSomfy(byte command){
  byte frame[7];
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("calling command %d"), command);
  
  //Check if index in expected range
  if ((XdrvMailbox.index < 1) || (XdrvMailbox.index > 4)) {
    AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("XdrvMailbox.index out of range: %d"), XdrvMailbox.index);
    return;
  }
  
  //Get address from settings
  uint32_t address = Settings.somfy_address[XdrvMailbox.index - 1];
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("using address %d"), address);
  //Get Code in Settings
  uint16_t code = Settings.somfy_code[XdrvMailbox.index - 1];
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("using rolling code %d"), code);
	BuildFrame(frame, command, address, code);
	SendFrame(frame);
  //Update Code in Settings and save
  Settings.somfy_code[XdrvMailbox.index - 1] = code + 1;
  SettingsSaveAll();
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("finished command %d"), command);
  ResponseCmndDone();
}

void CmdUp(void){
  CmdSomfy(0x2);
}

void CmdDown(void){
  CmdSomfy(0x4);
}

void CmdStop(void){
  CmdSomfy(0x1);
}

void CmdProg(void){
  CmdSomfy(0x8);
}

void CmdControl(void){
  char sub_string[XdrvMailbox.data_len +1];

  if (!strcmp(subStr(sub_string, XdrvMailbox.data, ",", 1),"UP")) {
    CmdUp();
  }
  if (!strcmp(subStr(sub_string, XdrvMailbox.data, ",", 1),"DOWN")) {
    CmdDown();
  }
  if (!strcmp(subStr(sub_string, XdrvMailbox.data, ",", 1),"STOP")) {
    CmdStop();
  }
}

void CmdSet(void)
{
  AddLog_P(LOG_LEVEL_DEBUG_MORE, PSTR("this is Cmdset"));
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("data len %d"), XdrvMailbox.data_len);
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("payload %d"), XdrvMailbox.payload);
  AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("data %s"), XdrvMailbox.data);

  if ((XdrvMailbox.index < 1) || (XdrvMailbox.index > 4)) {
    AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("XdrvMailbox.index out of range: %d"), XdrvMailbox.index);
    return;
  }

  char sub_string[XdrvMailbox.data_len +1];
  //Check for setting address
  if (!strcmp(subStr(sub_string, XdrvMailbox.data, ",", 1),"ADDRESS")) { // Note 1 used for param number
    uint32_t address = atoi(subStr(sub_string, XdrvMailbox.data, ",", 2));  // Note 2 used for param number
    AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("save setting address: %d"), address);
    Settings.somfy_address[XdrvMailbox.index - 1] = address;
    SettingsSaveAll();
    ResponseCmndDone();
  }

  if (!strcmp(subStr(sub_string, XdrvMailbox.data, ",", 1),"CODE")) { // Note 1 used for param number
    uint16_t code = atoi(subStr(sub_string, XdrvMailbox.data, ",", 2));  // Note 2 used for param number
    AddLog_P2(LOG_LEVEL_DEBUG_MORE, PSTR("save setting code: %d"), code);
    Settings.somfy_code[XdrvMailbox.index - 1] = code;
    SettingsSaveAll();
    ResponseCmndDone();
  }
}

void SendFrame(byte* frame){
	SendCommand(frame, 2);
	for (int i = 0; i<4; i++){
		SendCommand(frame, 7);
	}
}

void SendCommand(byte *frame, byte sync) {
  if(sync == 2) { // Only with the first frame.
    //Wake-up pulse & Silence
	  digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), HIGH);
    delayMicroseconds(9415);
    digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), LOW);
    delayMicroseconds(89565);
  }

// Hardware sync: two sync for the first frame, seven for the following ones.
  for (int i = 0; i < sync; i++) {
	  digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), HIGH);
    delayMicroseconds(4*SYMBOL);
    digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), LOW);
    delayMicroseconds(4*SYMBOL);
  }

// Software sync
  digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), HIGH);
  delayMicroseconds(4550);
  digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), LOW);
  delayMicroseconds(SYMBOL);


//Data: bits are sent one by one, starting with the MSB.
  for(byte i = 0; i < 56; i++) {
    if(((frame[i/8] >> (7 - (i%8))) & 1) == 1) {
      digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), LOW);
      delayMicroseconds(SYMBOL);
      digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), HIGH);
      delayMicroseconds(SYMBOL);
    }
    else {
      digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), HIGH);
      delayMicroseconds(SYMBOL);
      digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), LOW);
      delayMicroseconds(SYMBOL);
    }
  }

  digitalWrite(Pin(GPIO_SOMFY_RTS_DAT), LOW);
  delayMicroseconds(30415); // Inter-frame silence
}

void BuildFrame(byte *frame, byte button, unsigned long address, unsigned int code) {
  byte checksum;
  frame[0] = 0xA7; // Encryption key. Doesn't matter much
  frame[1] = button << 4;  // Which button did  you press? The 4 LSB will be the checksum
  frame[2] = code >> 8;    // Rolling code (big endian)
  frame[3] = code;         // Rolling code
  frame[4] = address >> 16; // Remote address
  frame[5] = address >>  8; // Remote address
  frame[6] = address;       // Remote address

// Checksum calculation: a XOR of all the nibbles
  checksum = 0;
  for(byte i = 0; i < 7; i++) {
    checksum = checksum ^ frame[i] ^ (frame[i] >> 4);
  }
  checksum &= 0b1111; // We keep the last 4 bits only


//Checksum integration
  frame[1] |= checksum; //  If a XOR of all the nibbles is equal to 0, the blinds will
                        // consider the checksum ok.

// Obfuscation: a XOR of all the bytes
  for(byte i = 1; i < 7; i++) {
    frame[i] ^= frame[i-1];
  }

  
  //this->writeCode(code + 1);
  //EEPROM.put(EEPROM_ADDRESS, code + 1); //  We store the value of the rolling code in the
                                        // EEPROM. It should take up to 2 adresses but the
                                        // Arduino function takes care of it.
  //EEPROM.commit();
}

void SomfyInit(void) {
  //Prepare rfm119 for OOK@433MHz
	rfm119.Chipset = CMT2119A;
	rfm119.vCMT2119AInit(Pin(GPIO_SOMFY_RTS_DAT), Pin(GPIO_SOMFY_RTS_CLK), CfgTbl, 21);
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/
bool Xdrv70(uint8_t function)
{
  if (!PinUsed(GPIO_SOMFY_RTS_DAT) || !PinUsed(GPIO_SOMFY_RTS_CLK)) { return false; }

  bool result = false;

  switch (function) {
    case FUNC_COMMAND:
      AddLog_P(LOG_LEVEL_DEBUG_MORE, PSTR("calling command"));
      result = DecodeCommand(kSomfyCommands, somfyCommand);
      break;
    case FUNC_INIT:
      SomfyInit();
      DEBUG_DRIVER_LOG(LOG_LEVEL_DEBUG_MORE, PSTR("init done."));
      break;
    case FUNC_JSON_APPEND:
      for(int i=0; i<4;i++){
        ResponseAppend_P(",\"Somfy%d\":{\"Address\":%d,\"Code\":%d}", i+1, Settings.somfy_address[i], Settings.somfy_code[i]);
      }
  }

  return result;
}

#endif  // USE_SOMFY
