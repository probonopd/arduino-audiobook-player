# arduino-audiobook-player

Arduino-based audiobook player with a focus on simplicity

## Software

Libraries
* https://github.com/PRosenb/EEPROMWearLevel
* https://github.com/Makuna/DFMiniMp3

## Hardware

Using an Arduino Pro Mini (with voltage regulator) proved to produce too many unwanted noises. Hence using a bare ATmega 168P chip without oscillator.

To upload to the bare chip, I am not using a serial programmer, but upload directly from the Arduino IDE using a USBASP with Ctrl-Shift-U. For this to work, I need to set permissions: `sudo chmod 666 /dev/bus/usb/*/*`

* ATmega 168P bare chip (without oscillator). Core and chip pinout: https://github.com/MCUdude/MiniCore. An ATTiny would do as shown by https://github.com/accidentalrebel/abplayer-mini
* Speaker, e.g., Ekulit BL 77
* 3D printed housing, e.g., variation of "3D printed transmission line" - is it worth the effort? Does the speaker need to be glued in? https://www.instructables.com/id/3D-Printed-Transmission-Line-Speaker/
* Clap sensor or other interesting sensor in addition to a play button?
