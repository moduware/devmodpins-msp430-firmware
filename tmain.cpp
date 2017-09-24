/*
 * tmain.cpp
 * MaKe iT eaSy
 */
//libraries
#include <np_module_mdk_v1.h>
#include "t_my_app.h"
#include "NCN_GPIO.h"

#define lenght 2
#define PIN_NUMBERS 16
extern unsigned char GPIO_Pins[];
unsigned char old_sender_buffer[PIN_NUMBERS+3] = {0};
unsigned char sender_buffer[PIN_NUMBERS+3] = {0};

// RECEIVE command - use range from 0x2700 to 0x27ff
//**Suggested**
//odd number for command number
//even number for response command number
//define function's command code at t_my_app.c

const MDK_REGISTER_CMD my_cmd_func_table[lenght] = { //Specify table's length according to number of commands used
		{0x2700, Settings }, // Command's name can be changed
		{0x2702, PWM }
};

void np_api_setup() {
	//Libraries, divers and PIN initialization
	//np_api_set_app_version(x, x, x); -- optional

	pinMode(0, INPUT);
	pinMode(1, INPUT);
	pinMode(2, INPUT);
//	pinMode(3, INPUT);	-- PIN 3 is not possible to access, skip it
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

	// If the command number is out of the range 0x2700 - 0x27ff, a FAIL message is displayed
	// Handle the fail event here!

	if ( np_api_register((MDK_REGISTER_CMD*)my_cmd_func_table, lenght) == MDK_REGISTER_FAIL ) { //communication check
	}

	// After setting this command, np_api_loop() will run just once
	//np_api_pm_automode_set();
}


void np_api_loop() {
	// This loop will run continuously while the MCU is not in sleep mode or has a stop condition

	/*SEND command -- use 0x2800
	*np_api_upload(0x2800, "I am sensor value!", 2) -- sensor value is unsigned char
	*
	*np_api_pm_automode_set(void) -- power save mode
	**In auto power save mode, the loop runs once
	*to run it one more time call "np_api_run_loop_once();"
	*To exit auto power save mode call "np_api_pm_automode_clear()"
	*delay(10);
	*/

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
	//Start module's function
}

void np_api_stop() {
	// Stop module's function
}
