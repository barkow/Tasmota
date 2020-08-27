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

	#define	SetTCLK()	digitalWrite(D1, HIGH)
	#define	ClrTCLK()	digitalWrite(D1, LOW)
	
	#define	InputTDAT() pinMode(D4, INPUT)
	#define	OutputTDAT() pinMode(D4, OUTPUT)
	
	#define	SetTDAT()	digitalWrite(D4, HIGH)
	#define	ClrTDAT()	digitalWrite(D4, LOW)
	
	#define TDAT_H()	(digitalRead(D4) == HIGH)
	#define	TDAT_L()	(digitalRead(D4) == LOW)

	class twiClass
	{
	 public:
	 	void vTWIInit(void);							/** initialize TWI port **/
	 	void vTWIWrite(byte adr, byte dat);		
	 	byte bTWIRead(byte adr);
	 	void vTWIReset(void);
	 	
	 private:
	 	void vTWIWriteByte(byte dat);
	 	byte bTWIReadByte(void);
	};

#else
	#warning "HopeDuino_TWI.h have been defined!"

#endif 
