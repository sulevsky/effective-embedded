## Kalman Filter migration from Ardupilot on Rust and ESP32

- Ardupilot has >2 KF
- Start with Wokwi 

## Todo

- [x] setup project
- [x] setup wokwi
- [x] read sensor baro
  - datasheet https://github.com/sparkfun/BMP180_Breakout/raw/master/Documentation/BMP180%20Datasheet%20V2.5.pdf

- [ ] read sensor accel
- [x] log sensor
- [ ] out sensor data Mavlink, watch in Mvalink debugger
- [ ] add Kalman filter
- [ ] review (add?) ardupilot Kalman filter
- [ ] [opt] Migrate to STM32, flash Speedybee, watch in
- [ ] check which sensor in Ardupilot/Spedeybee/order sensor (and probes)
- [ ] [opt] fix baro async