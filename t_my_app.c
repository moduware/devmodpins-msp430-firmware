/*
 * t_my_app.c
 * MaKe iT eaSy
 */
#include <np_module_mdk_v1.h>
#include "t_my_app.h"
#include "NCN_GPIO.h"
#include "msp430.h"

//Perform module's function
//Receive command - use range from 0x2700 to 0x27ff
// **Suggested**
// odd number for command number
// even number for response command number

#define PIN_NUMBERS 16

unsigned char old_pData[PIN_NUMBERS] = {0};
unsigned char GPIO_Pins[PIN_NUMBERS] = {0};

//pData[i]=1 --> GPIO I (upstream)
//pData[i]=2 --> GPIO O (LOW) (downstream)
//pData[i]=3 --> GPIO O (HIGH) (downstream)
//pData[i]=4 --> ADC (upstream)
//pData[i]=5 --> PWM (downstream)

//GPIO_Pins[i]= 1 -> GPIO I (upstream)
//GPIO_Pins[i]= 2 -> GPIO 0 (upstream)
//GPIO_Pins[i]= 3 -> ADC (upstream)
//GPIO_Pins[i]= 4 -> PWM (downstream)

// PIN 3 is not possible to access, skip it

void Settings (unsigned char*pData, unsigned char len) {

	int i;
	for (i = 0; i < PIN_NUMBERS; i ++) {
			if(pData[i] == 0 && pData[i] != old_pData[i]){
				GPIO_Pins[i] = 0;
				pinMode(i,INPUT);
			}else if(pData[i] == 1 && pData[i] != old_pData[i]){
				GPIO_Pins[i] = 1;
				pinMode(i,INPUT);
			}else if(pData[i] == 2 && pData[i] != old_pData[i]){
				GPIO_Pins[i] = 2;
				pinMode(i,OUTPUT);
				digitalWrite(i,LOW);
			}else if(pData[i] == 3 && pData[i] != old_pData[i]){
				GPIO_Pins[i] = 2;
				pinMode(i,OUTPUT);
				digitalWrite(i,HIGH);
			}else if(pData[i] == 4 && pData[i] != old_pData[i]){
				GPIO_Pins[i] = 3;
				pinMode(i,INPUT);
			}else if(pData[i] == 5 && pData[i] != old_pData[i]){
				GPIO_Pins[i] = 4;
				pinMode(i,OUTPUT);
			}
			old_pData[i] = pData[i];
	}
		// Feedback to Command 0x2701
		unsigned char response = 0x00;
		np_api_upload(0x2701, &response, 1);
}

void PWM (unsigned char*pData, unsigned char len) {

	if (GPIO_Pins[4] == 4){
		analogFrequencySet(4,180);
		analogWrite(4,pData[0]);
	}
	if (GPIO_Pins[5] == 4){
		analogFrequencySet(5,180);
		analogWrite(5,pData[1]);
	}
	// Feedback to Command 0x2703
	unsigned char response = 0x00;
	np_api_upload(0x2703, &response, 1);
}
