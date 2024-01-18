# Boe-Bot Robot Mapping
Boe-Bot robot that is programmed to map a given path and display the same map in a smaller form on an LCD screen.

Dependencies:
- Arduino IDE. (https://www.arduino.cc/en/software)
- Servo library. (https://www.arduino.cc/reference/en/libraries/servo/)
- Wire library. (https://www.arduino.cc/reference/en/language/functions/communication/wire/)
- LiquidCrystal I2C library. (https://reference.arduino.cc/reference/en/libraries/liquidcrystal-i2c/)

Components:
- Arduino Uno microcontroller.
- Boe-Bot Robot Kit.
- LCD screen. (Similar to this: https://www.parallax.com/product/parallax-2-x-16-serial-lcd-with-piezo-speaker-backlit-blue/)

Installation:
- Install Arduino IDE and mentioned libraries.
- Install the LCD screen onto the Robot Kit's breadboard.
- Check that the defined pins are the same as the ones on the breadboard.
- Upload the code to the microcontroller using a compatible USB cable.

Usage:
- Boe-Bot robot will traverse a 2D maze and map it using QTI sensors. 
- The map will be displayed on an LCD screen. 
- When the robot has traversed all of the map, it will then stop and let the user know with a beeper.
- When started, the robot will wait on standby mode and when a button is pressed, the robot will start it's work.

Video of the robot: https://youtu.be/PahHFl3Yav8

Image of the map: (https://github.com/HaroldOtsus/Boe-Bot-Robot-Mapping/assets/92230750/ea4d3ff2-5094-47ba-bf75-dbc96009319f)
