/*
 * THE FOLLOWING FIRMWARE IS PROVIDED: 
 *  (1) "AS IS" WITH NO WARRANTY; 
 *  (2) TO ENABLE ACCESS TO CODING INFORMATION TO GUIDE AND FACILITATE CUSTOMER.
 * CONSEQUENTLY, HopeRF SHALL NOT BE HELD LIABLE FOR ANY DIRECT, INDIRECT OR
 * CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING FROM THE CONTENT
 * OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE CODING INFORMATION
 * CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
 * 
 * Copyright (C) HopeRF
 *
 * website: www.HopeRF.com
 *          www.HopeRF.cn 
 */

/*! 
 * file       HopeDuino_TWI.cpp
 * brief      for HopeRF's EVB to use config soft TWI port
 * hardware   HopeRF's EVB
 *            
 *
 * version    1.1
 * date       Jan 15 2015
 * author     QY Ruan
 */

#include "HopeDuino_TWI.h"

/**********************************************************
**Name: 	vTWIInit
**Func: 	Init Port & config
**Note: 	
**********************************************************/
void twiClass::vTWIInit(int dataPin, int clockPin)
{
 DataPin = dataPin;
 ClockPin = clockPin;
 SetTCLK(ClockPin);
 pinMode(ClockPin, OUTPUT);						//CLK Output;
 
 SetTDAT(DataPin); 
 OutputTDAT(DataPin);	
}

/**********************************************************
**Name: 	vTWIReset
**Func: 	TWI Reset 
**Note: 	
**********************************************************/
void twiClass::vTWIReset(void)
{
 	byte bitcnt; 
 	
 	SetTCLK(ClockPin);			//CLK = 1;
 	
 	OutputTDAT(DataPin);
 	ClrTDAT(DataPin);			//DAT = 0;
 	
 	for(bitcnt=32; bitcnt!=0; bitcnt--)
 		{
 		SetTCLK(ClockPin);
		delayMicroseconds(TWI_SPEED);
		ClrTCLK(ClockPin);
 		delayMicroseconds(TWI_SPEED);
 		}
 	
 	SetTCLK(ClockPin);			//CLK = 1;
	ClrTDAT(DataPin);			//DAT = 0;
 	vTWIWrite(0x0D, 0x00);		//0x8D00 
}


/**********************************************************
**Name: 	vTWIWriteByte
**Func: 	TWI send one byte
**Note: 	
**********************************************************/
void twiClass::vTWIWriteByte(byte dat)
{
 	byte bitcnt;	
 	
 	SetTCLK(ClockPin);			//CLK = 1;
 	
	OutputTDAT(DataPin);
 	ClrTDAT(DataPin);			//DAT = 0;
 		
 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		SetTCLK(ClockPin);
 		if(dat&0x80)
 			SetTDAT(DataPin);
 		else
 			ClrTDAT(DataPin);
 		delayMicroseconds(TWI_SPEED);
 		ClrTCLK(ClockPin);
 		delayMicroseconds(TWI_SPEED);
 		dat <<= 1;
 		}
 	
 	SetTCLK(ClockPin);			//CLK = 1;
 	ClrTDAT(DataPin);			//DAT = 0;
}

/**********************************************************
**Name: 	bTWIReadByte
**Func: 	TWI read one byte
**Note: 	
**********************************************************/
byte twiClass::bTWIReadByte(void)
{
 	byte RdPara = 0;
 	byte bitcnt;
 	
 	InputTDAT(DataPin); 
 	SetTCLK(ClockPin);			//CLK = 1; 
 	 
 	for(bitcnt=8; bitcnt!=0; bitcnt--)
 		{
 		SetTCLK(ClockPin);
 		delayMicroseconds(TWI_SPEED);
 		RdPara <<= 1;
 		ClrTCLK(ClockPin);
 		delayMicroseconds(TWI_SPEED);
 		if(TDAT_H(DataPin))
 			RdPara |= 0x01;
 		else
 			RdPara |= 0x00; 
 		}
 	SetTCLK(ClockPin);	
	OutputTDAT(DataPin);
	ClrTDAT(DataPin);
 	return(RdPara);
}

/**********************************************************
**Name: 	vTWIWrite
**Func: 	TWI send one word
**Note: 	
**********************************************************/
void twiClass::vTWIWrite(byte adr, byte dat)
{
 	adr |= 0x80;			//Bit14=0 Write   
 	adr &= 0xBF;				
 
 	vTWIWriteByte(adr);
 	vTWIWriteByte(dat);
}

/**********************************************************
**Name: 	bTWIRead
**Func: 	TWI read one word
**Note: 	
**********************************************************/
byte twiClass::bTWIRead(byte adr)
{
 	adr |= 0xC0;				//Bit14=1 Read	
 	vTWIWriteByte(adr);			//��ַ
 	return(bTWIReadByte());
}







