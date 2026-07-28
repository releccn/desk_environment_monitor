# Desk Indoor Environment Monitor

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

The project is currently in progress. 

---

# Schematic (updated)

<img width="1133" height="779" alt="Screenshot 2026-07-28 160106" src="https://github.com/user-attachments/assets/1b0cab40-a41c-4570-9474-d9fb6bd58835" />

---

# PCB Design (updated)

<img width="1048" height="713" alt="Screenshot 2026-07-28 160212" src="https://github.com/user-attachments/assets/44cdc6a4-eea8-4c5e-a348-62a9827c3553" />
<img width="1087" height="678" alt="Screenshot 2026-07-28 160156" src="https://github.com/user-attachments/assets/9e92edd7-6d6d-4fe9-92e9-b75c7971c631" />
<img width="888" height="604" alt="Screenshot 2026-07-28 160137" src="https://github.com/user-attachments/assets/f4ebb258-d54b-42fc-8206-049ef10f08e0" />

