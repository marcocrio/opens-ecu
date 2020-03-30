# Open Source Electronic Control Module

This a SeniorProject implementation of an Electronic Control Module focused on the Fuel Management System of a small one cylinder EFI engine developed by EE & ECE students of The University of Texas at El Paso. 

This project is an open source Electronic Control Unit focused on the Fuel Management System of a small (35.8cc) 1 cylinder 4 strokes Electronically Fuel Injected engine (Honda GX35) based on the "Honda GX35 Engine Fuel Injection kit" sold by [ecotrons.com](https://www.ecotrons.com/small_engine_fuel_injection_kit/honda_gx35_engine_fuel_injection_kit/) and its sensors included in its kit.


Hardware and SDK
---

The ECU features an Xtensa ® dual-core 32-bit LX6 [Espressif's ESP32-S2-WROOM-32D](https://www.espressif.com/en/products/hardware/esp32/overview) development board microprocessor as the main controls system for the Fuel Injection System as well as the custom functionalities of the ECU.

The development board contains relevant features for the project such as:
- 34 × programmable GPIOs
- 12-bit SAR ADC. Up to 18 channels
- 2 × 8-bit DAC
- 3 × UART
- CAN 2.0
- Motor PWM
- LED PWM up to 16 channels
- Hall sensor

It is programed and flashed in C/C++ from a Debian based Linux distribution using Espressif's SDK and framework [ESP-IDF](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/)
In order to compile the project the ESP-IDF Toolchain and build tools are required. more information can be found in the [Get Started](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) guide.

The ESP32 runs on a FreeRTOS operating system whith lwIP for OS functionality implementation such as task registration, load distribution, semaphores, interrupt service routines, etc.

### Resources:
* [ESP32-WROOM-32D & ESP32-WROOM-32U Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf)
* [ESP32-D0WD processor - ESP32 Series Datasheet](https://www.espressif.com/sites/default/files/documentation/esp32_datasheet_en.pdf)
* [Xtensa LX6 Customizable DPU](https://mirrobo.ru/wp-content/uploads/2016/11/Cadence_Tensillica_Xtensa_LX6_ds.pdf)
* [Kolban's book on ESP32](https://leanpub.com/kolban-ESP32)


Project preparation:
---

Before compiling the project and after the succesful installation of [Espressif's toolkit](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/get-started/) please follow the next steps.

* Open the terminal and navegate to your project's main direcotry
* Run `idf.py fullclean`
* After full clean command has finished, run `idf.py menuconfig`
* Upon running *menuconfig* command, a menu is going to open on the terminal.
  Navegate it with the up/down keys and go to "Partition Table --->" and press enter
* Make sure the name under *Custom partition CSV file* matches the name of the partition table file in the root directory of the project. (partitions.csv in this case).
* Save sdkconfig file by pressing *S* and quit the menu by pressing *Q*
* Lastly run  `idf.py build` to compile the project and/or `idf.py flash` to compile and flash it into the ESP32. `idf.py monitor` can be used to connect to the "terminal-like" console developed to communicate with the ECU (to close the monitori push `ctrl + ]`).
---

# Features