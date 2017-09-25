/*
* devmodpins msp430
*/

/*
 * Library declaration
 */
#include <np_module_mdk_v1.h>
#include "NCN_GPIO.h"

/*
 * variables, identifier and pin declaration
 */
#define lenght 2
#define PIN_NUMBERS 16
extern unsigned char GPIO_Pins[];
unsigned char old_sender_buffer[PIN_NUMBERS+3] = {0};
unsigned char sender_buffer[PIN_NUMBERS+3] = {0};
unsigned char old_pData[PIN_NUMBERS] = {0};
unsigned char GPIO_Pins[PIN_NUMBERS] = {0};

/*
 * receiving functions declaration
 */
void Settings (unsigned char*pData, unsigned char len);
void PWM (unsigned char*pData, unsigned char len);

const MDK_REGISTER_CMD PinType[lenght] = {
		{0x2700, Settings },
		{0x2702, PWM }
};

void np_api_setup() {

    // default PIN setting as input
	pinMode(0, INPUT);
	pinMode(1, INPUT);
	pinMode(2, INPUT);
//	pinMode(3, INPUT); - PIN used by mdk, not accessible
	pinMode(4, INPUT);
	pinMode(5, INPUT);
	pinMode(6, INPUT);
	pinMode(7, INPUT);
	pinMode(8, INPUT);
	pinMode(9, INPUT);
	pinMode(10, INPUT);
	pinMode(11, INPUT);
	pinMode(12, INPUT);
	pinMode(13, INPUT);
	pinMode(14, INPUT);
	pinMode(15, INPUT);

	if ( np_api_register((MDK_REGISTER_CMD*)PinType, lenght) == MDK_REGISTER_FAIL ) {
	}
}


void np_api_loop() {

	unsigned char send = 0;

	unsigned char inputvalue;
	int i;
	for ( i = 0; i < PIN_NUMBERS; i ++ ) {	//fill in pin information
			//read GPIO input values
			if ( GPIO_Pins[i] == 1 ){
				//first 3 pins can be ADC, each one is 2 Byte length
				if ( i < 3) {
					inputvalue = digitalRead(i);
					sender_buffer[i*2+1] = inputvalue; //Remaining PINS are 1 Byte length
				} else {
					inputvalue = digitalRead(i);
					sender_buffer[i+3] = inputvalue;
				}
			//Read ADC values
			} else if ( GPIO_Pins[i] == 3 ){
				unsigned int value = analogRead(i);
				//first three pins can be ADC, and each of them got 2 Bytes space but ADC is 10 bits
				sender_buffer[i*2] = (value>>8)&0xff; //store most 8 significant bits and send it to sender buffer
				sender_buffer[i*2+1] = value&0xff; //get remaining bits (2) and send value to following buffer element
			}
	}
	for (i = 0; i < PIN_NUMBERS+3 ; i ++ ){

		if(sender_buffer[i] != old_sender_buffer[i]){
			send = 1;
		}
		old_sender_buffer[i] = sender_buffer[i];
	}
	if(send == 1){

		np_api_upload(0x2800,sender_buffer, PIN_NUMBERS+3);
	}

	delay(500);
}

void np_api_start() {
}

void np_api_stop() {
}


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
