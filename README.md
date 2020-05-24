# arduino-audiobook-player

Arduino-based audiobook player with a focus on simplicity

* When powered on, plays the recently played track
* When SD card inserted, plays the recently played track
* When next button is pressed (or sensor is actuated), plays the track after the recently played track
* Whenever a different track is played, that track is saved to EEPROM as the recently played track (with wear leveling to greatly incrase the lifespan of the EEPROM)

Possible future extensions:

* Smart sleep detection with last track repeat: If the listener has fallen asleep, then pressing the button should not play the next track but the recently played track (again)
* Distinguish between short press, long press, and double press
* Possibly IR remote control (but this waters down the concept of simplicity)
* Switch between multiple audiobooks
* Set the MP3 module into energy saving mode and wake it up again
* Set the ATmega into energy saving mode and wake it up again
* 3D printed enclosure waevguide/"Transmission line" https://www.youtube.com/watch?v=uujaBjf3Uwg (Also see http://www.mh-audio.nl/Calculators/TML.html)

## Software

Libraries
* https://github.com/PRosenb/EEPROMWearLevel
* https://github.com/Makuna/DFMiniMp3

## Hardware

Using an Arduino Pro Mini (with voltage regulator) proved to produce too many unwanted noises. Hence using a bare ATmega 168P chip without oscillator.

To upload to the bare chip, I am not using a serial programmer, but upload directly from the Arduino IDE using a USBASP with Ctrl-Shift-U. For this to work, I need to set permissions: `sudo chmod 666 /dev/bus/usb/*/*`

* [DFPlayer Mini MP3 Player module](https://wiki.dfrobot.com/DFPlayer_Mini_SKU_DFR0299) with on-board class D amplifier and SD card reader (3 EUR)
* microSD card (4 EUR)
* ATmega 168P bare chip (without oscillator). Core and chip pinout: https://github.com/MCUdude/MiniCore (2 EUR)
* Speaker (3 EUR)
* 3D printed housing, e.g., variation of "3D printed transmission line" - is it worth the effort? Does the speaker need to be glued in? https://www.instructables.com/id/3D-Printed-Transmission-Line-Speaker/
* Breadboard (2 EUR)
* Wires
* Clap sensor or other interesting sensor in addition to a play button?

Note: An ATTiny might possibly do as shown by https://github.com/accidentalrebel/abplayer-mini but in my test with an ATTiny45 flash storage was too small to use the EEPROMWearLevel library, and the lack of a hardware serial port complicates debugging.
