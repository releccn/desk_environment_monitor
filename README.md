# Desk Environment Monitor

The device is a bare metal embedded system built around the **ATmega328P-PU** (the same MCU on the Arduino Uno).

It measures temperature, relative humidity and air pressure using an **AHT20+BMP280** module and displays the sensor readings on an **LCD1602**.

Both the sensors and display communicate with the microcontroller using **I2C**. The LCD1602 is packaged with the **PCF8574** allowing the display to communicate using I2C.

All of the firmware is written in bare metal C. Drivers AHT20, BMP280, LCD1602, I2C and USART were developed from scratch using datasheets, C standard library and AVR-LibC.

---

# Hardware

- **ATmega328P-PU** - Microcontroller
- **AHT20+BMP280** - Measures temperature, humidity, pressure
- **AMS1117-3.3V** - 3.3V voltage regulator for power to AHT20+BMP280
- **3.3V <-> 5V Bidirectional Logic Level Shifter** - Intefaces sensors 3.3V I2C bus and the 5V microcontroller I2C bus
- **LCD1602** - Displays temperature, humidity, pressure
- **5V DC Wall Adapter** - Power source

---

# Software Structure

```text
driver/
├── i2c/
│   ├── i2c.c
│   └── i2c.h
├── usart/
│   ├── usart.c
│   └── usart.h
├── aht20/
│   ├── aht20.c
│   └── aht20.h
├── bmp280/
│   ├── bmp280.c
│   └── bmp280.h
└── lcd/
    ├── lcd.c
    └── lcd.h

tools/
├── i2c_scanner.c
└── i2c_scanner.h
```

---

# Motivation

The motivation behind this project is to ditch the Arduino libraries and build skills in bare metal C through developing peripheral drivers myself and implementing communication protocols from scratch by reading datasheets.

---

# Project Status

The project is currently in progress. The custom PCB I created in KiCad for this is currently being shipped, and in the meantime, I am planning out the CAD enclosure in Fusion 360.

---

# Schematic

<img width="1134" height="781" alt="Screenshot 2026-07-09 142032" src="https://github.com/user-attachments/assets/992de505-f250-478e-8e4b-852fa529b4f7" />

---

# PCB Design

<img width="1047" height="673" alt="Screenshot 2026-07-09 142107" src="https://github.com/user-attachments/assets/38f3bfea-fbff-4b73-b8dc-2033a1781263" />
<img width="961" height="607" alt="Screenshot 2026-07-09 142050" src="https://github.com/user-attachments/assets/c6e4f3c1-2aea-4c35-a0d9-30d5bbfbb155" />
