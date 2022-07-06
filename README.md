# picoarcadestick

This is a arcade control panel to USB HID encoder for the Raspberry Pi Pico, it can be compiled by setting up your IDE/environment as per the https://github.com/raspberrypi/pico-sdk instructions and relies on TinyUSB.

It is designed to be flexible to use all the GPIO of the Raspberry Pi Pico for multiple USB HIDs such as a 2L8B control panel that shows up as two seperate USB HIDs in Windows and MiSTer.

I hope to extend it in the future to also:
* Have a XInput/Switch/PS3 etc modes.
* Link multiple Raspberry Pi Picos to expand the GPIO 

I have lag tested with my MiSTer using https://github.com/misteraddons/inputlatency which shows ~1ms.
