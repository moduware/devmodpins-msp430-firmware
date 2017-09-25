/*
 * tmain.cpp
 * MaKe iT eaSy
 */
//libraries
#include <np_module_mdk_v1.h>
#include "NCN_GPIO.h"


#define GPIO_IN 0
#define GPIO_OUT 1
#define ADC 2
#define PWM 3

#define PIN_UPDATE_LENGTH 4
#define CONFIGURATION_LENGTH 16
#define CACHE_LENGTH 19
#define SEND_BUFFER 64
unsigned char configuration[CONFIGURATION_LENGTH] = {0};
unsigned char cache[CACHE_LENGTH] = {0};
unsigned char send_buffer[SEND_BUFFER] = {0};
unsigned int updated_pins;

// RECEIVE command - use range from 0x2700 to 0x27ff
//**Suggested**
//odd number for command number
//even number for response command number
//define function's command code at t_my_app.c

void PinConfigReceivedHandler (unsigned char*pData, unsigned char len);

const MDK_REGISTER_CMD my_cmd_func_table[1] = { //Specify table's length according to number of commands used
        {0x2700, PinConfigReceivedHandler}
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

	if ( np_api_register((MDK_REGISTER_CMD*)my_cmd_func_table, 1) == MDK_REGISTER_FAIL ) { //communication check
	}

}

void PinConfigReceivedHandler (unsigned char*pData, unsigned char len) {
    // update pin type in configuration bytes
    unsigned int pin_number = pData[0];
    unsigned int pin_type = pData[1];
    configuration[pin_number] = pin_type;

    // set pin to value if applicable (GPIO In - 0 or PWM - 3)
    if(pin_type == GPIO_IN) {
        pinMode(pin_number, OUTPUT);
        digitalWrite(pin_number, pData[2]);
    } else if(pin_type == PWM) {
        analogFrequencySet(4,180);
        analogWrite(pin_number, pData[2]);
    }
}

void add_to_send_buffer(unsigned char* update) {
    // add 4 byte array of pin description to send buffer
    unsigned int current_index = updated_pins * PIN_UPDATE_LENGTH;
    for(unsigned int i = 0; i < PIN_UPDATE_LENGTH; i++) {
        send_buffer[current_index + i] = update[i];
    }
}

void make_pin_update(int pin_number, int pin_type, unsigned char value1, unsigned char value2) {
    // form 4 byte array of pin description
    unsigned char update[4] = {0};
    update[0] = pin_number;
    update[1] = pin_type;

    if(pin_type == GPIO_IN) {
        update[2] = value1;
    } else if(pin_type == ADC) {
        update[2] = value1;
        update[3] = value2;
    }

    add_to_send_buffer(update);
}

bool send_pin_data(int pin_number, int pin_type, unsigned char value1, unsigned char value2) {
    // TODO: improve cash mapping to make it simpler and easier to understand
    unsigned int cache_index;
    if(pin_number <= 2) {
        // For first 3 pins each takes two bytes in cash
        cache_index = pin_number * 2;
    } else {
        // For rest of pins we need to shift 3 additional bytes for first 3 pins and skip pin 3
        cache_index = pin_number + 2;
    }

    if(pin_type == GPIO_OUT) {
        if(cache[cache_index] != value1) {
            make_pin_update(pin_number, pin_type, value1, 0x00);
            return true;
        }
    } else if(pin_type == ADC) {
        if(cache[cache_index] != value1 || cache[cache_index + 1] != value2) {
            make_pin_update(pin_number, pin_type, value1, value2);
            return true;
        }
    }

    return false;
}

void np_api_loop() {
    updated_pins = 0;

	for(unsigned int pin_number=0; pin_number<CONFIGURATION_LENGTH; pin_number++) {
	    if(pin_number == 3) continue;

	    bool is_updated;
	    if(configuration[pin_number] == GPIO_OUT) {
	        // send GPIO Out value
	        unsigned char value = digitalRead(pin_number);
	        is_updated = send_pin_data(pin_number, GPIO_OUT, value, 0x00);
	    } else if(configuration[pin_number] == ADC) {
	        // send ADC value
	        unsigned int value = digitalRead(pin_number);
	        unsigned char upper_byte = (value >> 8) & 0xfff;
	        unsigned char lower_byte = value & 0xff;
	        is_updated = send_pin_data(pin_number, ADC, upper_byte, lower_byte);
	    }

	    if(is_updated) {
	        updated_pins++;
	    }
	}

	if(updated_pins != 0) {
	    int data_length = updated_pins * 4;

	    np_api_upload(0x2800, send_buffer, data_length);
	}


	delay(500);
}


void np_api_start() {
    //Start module's function
}

void np_api_stop() {
    // Stop module's function
}
