/*
 * devmodpins msp430
 */
#include <np_module_mdk_v1.h>
#include "NCN_GPIO.h"

//Configures pins to one of modes: GPIO I, GPIO O, ADC or PWM
#define GPIO_IN 0
#define GPIO_OUT 1
#define ADC 2
#define PWM 3

#define PIN_UPDATE_LENGTH 4
#define CONFIGURATION_LENGTH 16 // 16 PIN
#define CACHE_LENGTH 19 // 16 pins, 3 ADC PINS - 2bye per ADC PIN
#define SEND_BUFFER 64 // 16 pins, 4 bytes per PIN
unsigned char configuration[CONFIGURATION_LENGTH] = {0};
unsigned char cache[CACHE_LENGTH] = {0};
unsigned char send_buffer[SEND_BUFFER] = {0};
unsigned int updated_pins;


void PinConfigurationReceivedHandler (unsigned char*pData, unsigned char len);

const MDK_REGISTER_CMD pin_configuration[1] = {
        {0x2700, PinConfigurationReceivedHandler}
};

void np_api_setup() {

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

	if ( np_api_register((MDK_REGISTER_CMD*)pin_configuration, 1) == MDK_REGISTER_FAIL ) {
	}
}

/*
 * receive data from tile, detect pin type and set pin type to corresponding pin
 */
void PinConfigurationReceivedHandler (unsigned char*pData, unsigned char len) {
    // update pin type in configuration bytes
    unsigned int pin_number = pData[0];
    unsigned int pin_type = pData[1];
    configuration[pin_number] = pin_type;

    // set pin to value if applicable (GPIO out - 1 or PWM - 3)
    if(pin_type == GPIO_IN || pin_type == ADC) {
        pinMode(pin_number, INPUT);
    } else if(pin_type == GPIO_OUT) {
        pinMode(pin_number, OUTPUT);
        digitalWrite(pin_number, pData[2]);
    } else if(pin_type == PWM) {
        //set frequency hz
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

//update PIN information
void make_pin_update(int pin_number, int pin_type, unsigned char value1, unsigned char value2) {
    // form 4 byte array of pin description
    unsigned char update[4] = {0};
    update[0] = pin_number;
    update[1] = pin_type;

    //fill buffer to send to tile on GPIO IN and ADC modes
    if(pin_type == GPIO_IN) {
        update[2] = value1;
    } else if(pin_type == ADC) {
        update[2] = value1;
        update[3] = value2;
    }

    add_to_send_buffer(update);
}

// store pin information to send to tile - 4 byte per PIN configuration
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
    //fill pin update on GPIO input and ADC modes
    if(pin_type == GPIO_IN) {
        if(cache[cache_index] != value1) {
            make_pin_update(pin_number, pin_type, value1, 0x00);
            cache[cache_index] = value1;
            return true;
        }
    } else if(pin_type == ADC) {
        if(cache[cache_index] != value1 || cache[cache_index + 1] != value2) {
            make_pin_update(pin_number, pin_type, value1, value2);
            cache[cache_index] = value1;
            cache[cache_index + 1] = value2;
            return true;
        }
    }

    return false;
}

void np_api_loop() {
    updated_pins = 0;
    //select data to send to tile when PIN type is GPIO OUT or ADC
	for(unsigned int pin_number=0; pin_number<CONFIGURATION_LENGTH; pin_number++) {
	    //skip PIN 3 as is not in use
	    if(pin_number == 3) continue;

	    bool is_updated;
	    if(configuration[pin_number] == GPIO_IN) {
	        // send GPIO input value
	        unsigned char value = digitalRead(pin_number);
	        is_updated = send_pin_data(pin_number, GPIO_IN, value, 0x00);
	    } else if(configuration[pin_number] == ADC) {
	        // send ADC value
	        unsigned int value = analogRead(pin_number);
	        unsigned char upper_byte = (value >> 8) & 0xff;
	        unsigned char lower_byte = value & 0xff;
	        is_updated = send_pin_data(pin_number, ADC, upper_byte, lower_byte);//lower_byte);
	    }

	    if(is_updated) {
	        updated_pins++;
	    }
	}
	//send data to tile just when PIN type is different than previous PIN configuration
	if(updated_pins != 0) {
	    int data_length = updated_pins * 4;

	    np_api_upload(0x2800, send_buffer, data_length);
	}


	delay(150);
}


void np_api_start() {
}

void np_api_stop() {
}
