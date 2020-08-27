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

 

#ifndef HopeDuino_TWI_h
	#define HopeDuino_TWI_h

#include <Arduino.h>

	#define	TWI_SPEED	10			//the bigger the slow

	/** Hardware brief **/

	/** Hardware brief **/    

	#define	SetTCLK(pin)	digitalWrite(pin, HIGH)
	#define	ClrTCLK(pin)	digitalWrite(pin, LOW)
	
	#define	InputTDAT(pin) pinMode(pin, INPUT)
	#define	OutputTDAT(pin) pinMode(pin, OUTPUT)
	
	#define	SetTDAT(pin)	digitalWrite(pin, HIGH)
	#define	ClrTDAT(pin)	digitalWrite(pin, LOW)
	
	#define TDAT_H(pin)	(digitalRead(pin) == HIGH)
	#define	TDAT_L(pin)	(digitalRead(pin) == LOW)

	class twiClass
	{
	 public:
	 	void vTWIInit(int dataPin, int clockPin);							/** initialize TWI port **/
	 	void vTWIWrite(byte adr, byte dat);		
	 	byte bTWIRead(byte adr);
	 	void vTWIReset(void);
	 	
	 private:
	 	void vTWIWriteByte(byte dat);
	 	byte bTWIReadByte(void);
		int DataPin;
		int ClockPin;
	};

#else
	#warning "HopeDuino_TWI.h have been defined!"

#endif 
