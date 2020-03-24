# Open Source Electronic Control Module

This a Senior project implementation of an Electronic Control Module focused on the Fuel Management System of a small one cylinder EFI engine developed by EE & ECE students of The University of Texas at El Paso. 

This project is an open source Electronic Control Unit focused on the Fuel Management System of a small (35.8cc) 1 cylinder 4 strokes Electronically Fuel Injected engine (Honda GX35) based on the "Honda GX35 Engine Fuel Injection kit" sold by [ecotrons.com](https://www.ecotrons.com/small_engine_fuel_injection_kit/honda_gx35_engine_fuel_injection_kit/) and its sensors included in its kit.

### Hardware and SDK
The ECU uses [spressif's ESP32-S2-WROOM-32D](https://www.espressif.com/en/products/hardware/esp32/overview) development board microcontroeller as the main control system for the Fuel Injection System as well as the rest of the functionalities of the ECU.
It is programed and flashed in C/C++ using Espressif's SDK and framework [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
