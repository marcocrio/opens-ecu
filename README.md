# Open Source Electronic Control Module

This a Senior project implementation of an Electronic Control Module focused on the Fuel Management System of a small one cylinder EFI engine developed by EE & ECE students of The University of Texas at El Paso. 

This project is an open source Electronic Control Unit focused on the Fuel Management System of a small (35.8cc) 1 cylinder 4 strokes Electronically Fuel Injected engine (Honda GX35) based on the "Honda GX35 Engine Fuel Injection kit" sold by [ecotrons.com](https://www.ecotrons.com/small_engine_fuel_injection_kit/honda_gx35_engine_fuel_injection_kit/) and its sensors included in its kit.

### Hardware and SDK
--------------------
The ECU features a Xtensa ® dual-core 32-bit LX6 microprocessor [Espressif's ESP32-S2-WROOM-32D](https://www.espressif.com/en/products/hardware/esp32/overview) development board microcontroeller as the main control system for the Fuel Injection System as well as the rest of the functionalities of the ECU.

It contains relevant features for the project such as:
- 34 × programmable GPIOs
- 12-bit SAR ADC up to 18 channels
- 2 × 8-bit DAC
- CAN 2.0
- Motor PWM
- LED PWM up to 16 channels
- Hall sensor

It is programed and flashed in C/C++ from a Debian based Linux distribution using Espressif's SDK and framework [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
In order to compile the project the ESP-IDF Toolchain and build tools are required. more information can be found in the [Get Started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) guide.

**Project preparation:** 
-----------------------
Before compiling the project and after the succesful installation of Espressif's toolkit please follow the following steps.
* open the terminal and navegate to your projects main direcotry

* run `idf.py fullclean`

* after full clean command has finished run `idf.py menuconfig`

* after running *menuconfig* command a menu is going to open on the terminal. Navegate it with the up/down keys and go to "Partition Table --->" and press enter

*  make sure the name under *Custom partition CSV file* matches the name of the partition table file in the root directory of the project. (partitions.csv in this case).

* Save sdkconfig file by pressing *S* and quit the menu by pressing *Q*

* lastly run  `idf.py build` to compile the project and `idf.py flash` to flash it into the ESP32
------------------------------

# Features