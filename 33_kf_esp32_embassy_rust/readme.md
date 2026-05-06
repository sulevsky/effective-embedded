## Kalman Filter migration from Ardupilot on Rust and ESP32

- Ardupilot has >2 KF
- Start with Wokwi 

## Todo

- [x] setup project
- [x] setup wokwi
- [x] read sensor baro
  - datasheet https://github.com/sparkfun/BMP180_Breakout/raw/master/Documentation/BMP180%20Datasheet%20V2.5.pdf

- [+] add/test libs
  - async does not work
  - sync requires old embedded-hal
- [ ] add tests
- [+] read sensor accel
  - SpeedyBee F405 V3 (Stack/Mini)
    - Gyro (IMU): BMI270 
    - Barometer: SPL06-001 (often listed simply as SPL06)  
  - Wokwi
    - Gyro (IMU): MPU6050 
    - Barometer: BMP180  


- [x] log sensor
- [ ] out sensor data Mavlink, watch in Mvalink debugger
- [ ] add Kalman filter
- [ ] review (add?) ardupilot Kalman filter
- [ ] [opt] Migrate to STM32, flash Speedybee, watch in
- [ ] check which sensor in Ardupilot/Spedeybee/order sensor (and probes)
- [ ] [opt] fix baro async