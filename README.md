# SecureDoors

Alarm for your door with SMS or call notification

# What such device should do

- When door is opened (REED SWITCH IS OPENED)
    - Makes X calls (You specify duration and count of those)
    - Turns on a siren (You specify a delay between opening a door and toggling a siren and the duration of the alarm)
- When disarm button is pressed
    - Disarms the alarm

# Parts used for this project

- Arduino Nano (or UNO, It's basically the same)
- Reed Switch (Normally closed)
- Siren (Or any other sound emmiter working by applying voltage)
- SIM800L EVB 2.0 GSM module.
- 1CH Relay (Low level trigger)
- Pushbutton
- Battery, or any other power source (Prefferably 12V)
- Step-down converter (On output 5V and min. 2.2A)

# Libraries used for this project

- SoftwareSerial.h (https://www.arduino.cc/en/Reference/SoftwareSerial)
    - Note: This library is installed with Arduino IDE

# Schematics and diagrams

## Diagram

![img](./schematics/diagram.png)

## Schematics

Not yet released