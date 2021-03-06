# DevMod Pins

Devmod project allows developers to quickstart prototyping a module idea without need write a line of code. 

Different PIN modes can be selected according to MSP430 GPIO library configuration. 
![pin_table]

By default configuration all pins are set as GPIO input, but pin settings can be easily changed on the tile
![devmod_tile]

Tapping on the middle column changes pin mode
* **GPIO I** displays **'LOW'** or **'HIGH'** on pin change
* Tap once on any pin to change mode from GPIO I to GPIO O. **GPIO O** mode allows to change to **'LOW'** or **'HIGH'** values
* Tap twice on pin 0,1 and 2 to change the mode from GPIO I to ADC. **ADC** mode displays a decimal value between 0 and 3.3V
* Tap twice on PIN 4 and 5 to set **PWM** mode. On PWM enter values from 0 to 100% to change duty cycle 

![pin_settings]

Find code for [DevMod pin tile](https://github.com/nexpaq/tile-devmod-pins)

[pin_table]:https://github.com/nexpaq/devmodpins-msp430-firmware/blob/v2-update/images/pin_table.PNG
[devmod_tile]:https://github.com/nexpaq/devmodpins-msp430-firmware/blob/v2-update/images/devmod_tile.PNG
[pin_settings]:https://github.com/nexpaq/devmodpins-msp430-firmware/blob/v2-update/images/pin_settings.PNG