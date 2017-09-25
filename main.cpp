/*
* devmodpins msp430
*/

/*
 * Library declaration
 */
#include <np_module_mdk_v1.h>
#include "NCN_GPIO.h"

/*
 * variables and pin declaration
 */
#define lenght 2
#define pin_number 16
/*
 * MSP module has 16 pin. Each pin sends 1byte of data but ADC type sends 2byte (10bit). There is 3 ADC pin
 */
unsigned char old_sender_buffer[pin_number+3] = {0};
unsigned char sender_buffer[pin_number+3] = {0};
unsigned char old_data[pin_number] = {0};
unsigned char gpio_pins[pin_number] = {0};

//Configures pins to one of modes: GPIO I, GPIO O, ADC or PWM
void SetPinModes (unsigned char*pData, unsigned char len);
//Sets PWM values
void SetPWMValues (unsigned char*pData, unsigned char len);

const MDK_REGISTER_CMD PinType[lenght] = {
		{0x2700, SetPinModes },
		{0x2702, SetPWMValues }
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

	// check all pins
	for (i = 0; i < pin_number; i ++) {
		//read GPIO input values
	    if (gpio_pins[i] == 1){
			//skip first 3 pins can be ADC, each one is 2 Byte length
	        if (i < 3) {
				inputvalue = digitalRead(i);
				sender_buffer[i*2+1] = inputvalue; //Remaining PINS are 1 Byte length
			} else {
				inputvalue = digitalRead(i);
				sender_buffer[i+3] = inputvalue;
			}
			//Read ADC values
			} else if ( gpio_pins[i] == 3 ){
			    unsigned int value = analogRead(i);
				//first three pins can be ADC, and each of them got 2 Bytes space but ADC is 10 bits
				sender_buffer[i*2] = (value>>8)&0xff; //store most 8 significant bits and send it to sender buffer
				sender_buffer[i*2+1] = value&0xff; //get remaining bits (2) and send value to following buffer element
			}
	}

	for (i = 0; i < pin_number+3 ; i ++ ){

		if(sender_buffer[i] != old_sender_buffer[i]){
			send = 1;
		}
		old_sender_buffer[i] = sender_buffer[i];
	}
	if(send == 1){

		np_api_upload(0x2800,sender_buffer, pin_number+3);
	}

	delay(500);
}

void np_api_start() {
}

void np_api_stop() {
}


void SetPinModes (unsigned char*pData, unsigned char len) {

    int i;
    for (i = 0; i < pin_number; i ++) {
            if(pData[i] == 0 && pData[i] != old_data[i]){
                gpio_pins[i] = 0;
                pinMode(i,INPUT);
            }else if(pData[i] == 1 && pData[i] != old_data[i]){
                gpio_pins[i] = 1;
                pinMode(i,INPUT);
            }else if(pData[i] == 2 && pData[i] != old_data[i]){
                gpio_pins[i] = 2;
                pinMode(i,OUTPUT);
                digitalWrite(i,LOW);
            }else if(pData[i] == 3 && pData[i] != old_data[i]){
                gpio_pins[i] = 2;
                pinMode(i,OUTPUT);
                digitalWrite(i,HIGH);
            }else if(pData[i] == 4 && pData[i] != old_data[i]){
                gpio_pins[i] = 3;
                pinMode(i,INPUT);
            }else if(pData[i] == 5 && pData[i] != old_data[i]){
                gpio_pins[i] = 4;
                pinMode(i,OUTPUT);
            }
            old_data[i] = pData[i];
    }
        // Feedback to Command 0x2701
        unsigned char response = 0x00;
        np_api_upload(0x2701, &response, 1);
}

void SetPWMValues (unsigned char*pData, unsigned char len) {

    if (gpio_pins[4] == 4){
        analogFrequencySet(4,180);
        analogWrite(4,pData[0]);
    }
    if (gpio_pins[5] == 4){
        analogFrequencySet(5,180);
        analogWrite(5,pData[1]);
    }
    // Feedback to Command 0x2703
    unsigned char response = 0x00;
    np_api_upload(0x2703, &response, 1);
}
