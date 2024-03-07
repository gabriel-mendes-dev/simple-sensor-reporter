
# Hardware and software stack used

To build this simple case solution, it was chosen to use an HC-SR04 ultrasonic distance sensor and the ESP32 SoC.

Although the project was created from ESP-IDF project structure, the program only uses ESP32 lower-level drivers (hardware abstraction layer) and FreeRTOS, not using other framework facilities.

# Diagrams

Hardware and software diagrams in the `diagrams` directory can be compiled with plantuml. The images will be available as release files.

# Encrypted communication
To run this program, you must generate a 32-bytes AES key to be used in the communication between the microcontroller and the Host PC. For testing purposes, you can generate a random one [here](https://acte.ltd/utils/randomkeygen) - the "Encryption key 256". Copy the string and add it to a file named `aes.key` in the `main` directory. The building process will add this key to the read-only data section.

# Host PC script
To test data receiving and decryption, you can use the python script `host-pc.py` in the directory `host-pc-test`. It is a simple script just for testing purpose.

First, install necessary module `pycryptodome`. This module has some incompatibilities with some deprecated modules, so it may be necessary to first run:
```
pip uninstall crypto
pip uninstall pycrypto
```
Then, install used module:
```
pip install pycryptodome
```
Change this line in the script to use the right serial port (COM port for windows and serial device /dev for linux):
```
ser = serial.Serial('COM3', 115200, 8, "N", 1)
```
Add the same `aes.key` file in the `host-pc-test` directory and run:
```
cd host-pc-test
python host-pc.py
```