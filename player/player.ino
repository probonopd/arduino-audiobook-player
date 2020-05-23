
// Using an Arduino Pro Mini (with voltage regulator)
// proved to produce too many unwanted noises.
// Hence using a bare ATmega168P chip without oscillator.
// Core and chip pinout: https://github.com/MCUdude/MiniCore
// Not using a serial programmer, uploading directly using USBASP with Ctrl-Shift-U
// Permissions: sudo chmod 666 /dev/bus/usb/*/*
// Most likely a smaller ATMega or even ATTiny might do
// Library: https://github.com/PowerBroker2/DFPlayerMini_Fast

#include <SoftwareSerial.h>
#include <DFPlayerMini_Fast.h>
#include <EEPROM.h>
#include <avr/sleep.h>

SoftwareSerial mySerial(10, 11); // RX, TX
DFPlayerMini_Fast myMP3;

int addrOfTrack = 0;
int16_t track;
const long interval = 5000; // interval at which to ask the player for the current track (milliseconds)
unsigned long previousMillis = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  mySerial.begin(9600);
  myMP3.begin(mySerial);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  // myMP3.volume(20);
  play(); // This is non-blocking
}

void loop()
{
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    askPlayerForTrack();

  }
}

void play() {

  track = EEPROM.read(addrOfTrack);
  // Reset track number to sensible value
  // if (track > myMP3.numUsbTracks() ) {
  //  track  = 0;
  // }

  // TO BE DEBUGGED: WE HAVE AN ISSUE WHEN THE FIRST TRACK IS SELECTED.
  // WHY? DO FILES ON SD NEED TO START WITH mp3/0000.mp3?

  myMP3.playFromMP3Folder(track + 1);
}


void askPlayerForTrack() {
  int16_t current_track;
  current_track = myMP3.currentSdTrack();
  track = EEPROM.read(addrOfTrack);
  // If the player answered with anoth track than the last one we knew, save it and blink short
  if (current_track != track) {
    EEPROM.write(addrOfTrack, current_track);
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
}
