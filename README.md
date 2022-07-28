# kth-hyperloop-electronics
Software on board the first pod iteration of KTH Hyperloop


## Libraries
These are all the Arduino libraries that are needed. Download them through the Arduino IDE directly as it will also download all pre-requisties. Use the keywords that are mentioned within "...".
1. Temperature Sensor - Adafruit 3328 - "MAX31865" - https://learn.adafruit.com/adafruit-max31865-rtd-pt100-amplifier/arduino-code
2. Voltage Sensor - Adafruit 4226 - "INA260" - https://learn.adafruit.com/adafruit-ina260-current-voltage-power-sensor-breakout/arduino
3. IMU - Adafruit 2472 - "BNO055" - https://learn.adafruit.com/adafruit-bno055-absolute-orientation-sensor/arduino-code
4. ESC/motors:  https://howtomechatronics.com/tutorials/arduino/arduino-brushless-motor-control-tutorial-esc-bldc/
5. Wireless transceivers: https://howtomechatronics.com/tutorials/arduino/arduino-wireless-communication-nrf24l01-tutorial/ (video: https://www.youtube.com/watch?v=7rcVeFFHcFM)
6. Transistor switch: https://create.arduino.cc/projecthub/SBR/switching-using-transistor-c9114a (For the solenoidal valve actuation in the brakes system)


To install a library within Arduino IDE - Follow the steps below:
1. Go to Sketch->Include Library->Manage Libraries (or Ctrl+Shift+I)
2. In the search field within the new pop-up window, type the keywords mentioned within "...". For example; "MAX31865".


