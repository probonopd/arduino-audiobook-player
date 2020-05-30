
// Using an Arduino Pro Mini (with voltage regulator)
// proved to produce too many unwanted noises.
// Hence using a bare ATmega168P chip without oscillator.
// Core and chip pinout: https://github.com/MCUdude/MiniCore
// Not using a serial programmer, uploading directly using USBASP with Ctrl-Shift-U
// Permissions: sudo chmod 666 /dev/bus/usb/*/*
// An ATTiny would also do (with another core)
// Files must be in:
// 01/0001_SomeOptionalTitle.mp3

#include <Arduino.h>
#include <DebounceEvent.h>

#define BUTTON_PIN          4

#include <SoftwareSerial.h>
#include <DFMiniMp3.h>
#include <EEPROMWearLevel.h>

// See https://github.com/PRosenb/EEPROMWearLevel
#define EEPROM_LAYOUT_VERSION 0
#define AMOUNT_OF_INDEXES 2
#define INDEX_CONFIGURATION_VAR1 0

// Advance declaration so that we can refer to the function
// before we define it
void play();

// We keep track of the time at which the last track finished playing
// Only if the button is pressed within a certain amount of time after this,
// we increment the track.
// Otherwise the listener may have fallen asleep duing the last track and may
// hence want to hear the last track again.
unsigned long millisWhenLastTrackStartedOrFinished = 0;
unsigned long sleepThresholdMillis = 60000; // 1 minute

// Callback class, its member methods will get called
class Mp3Notify
{
  public:
    static void PrintlnSourceAction(DfMp3_PlaySources source, const char* action)
    {
      if (source & DfMp3_PlaySources_Sd)
      {
        Serial.print("SD Card, ");
      }
      if (source & DfMp3_PlaySources_Usb)
      {
        Serial.print("USB Disk, ");
      }
      if (source & DfMp3_PlaySources_Flash)
      {
        Serial.print("Flash, ");
      }
      Serial.println(action);
    }
    static void OnError(uint16_t errorCode)
    {
      Serial.println();
      Serial.print("Com Error, see DfMp3_Error for code meaning: ");
      Serial.println(errorCode);
    }
    static void OnPlayFinished(DfMp3_PlaySources source, uint16_t track)
    {
      Serial.print("Play finished for #");
      Serial.println(track);
      millisWhenLastTrackStartedOrFinished = millis();
    }
    static void OnPlaySourceOnline(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "online");
      play();
    }
    static void OnPlaySourceInserted(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "inserted");
      play();
    }
    static void OnPlaySourceRemoved(DfMp3_PlaySources source)
    {
      PrintlnSourceAction(source, "removed");
    }
};

// Instantiate a DFMiniMp3 object
SoftwareSerial secondarySerial(10, 11); // RX, TX
DFMiniMp3<SoftwareSerial, Mp3Notify> mp3(secondarySerial);

uint16_t track;

// This callback function is called when the button is clicked
void callback(uint8_t pin, uint8_t event, uint8_t count, uint16_t length) {
  if(length > 0) {
    Serial.print("Event : "); Serial.print(event);
    Serial.print(" Count : "); Serial.print(count);
    Serial.print(" Length: "); Serial.print(length);
    Serial.println();

    if(count==1 && length<500){
      Serial.println("Single click detected: Play");
      if (millis() - millisWhenLastTrackStartedOrFinished < sleepThresholdMillis ) {
        saveTrack(track+1);
      }
      play();
    }

    if(count==2 && length<500){
      Serial.println("Double click detected: last");
      saveTrack(track-1);
      play();
    }

    if(count==7 && length>5000){
      Serial.println("7 clicks detected the last of which is >5 sec: Reset to track 1");
      saveTrack(1);
      play();
    }

  }
}

DebounceEvent button = DebounceEvent(BUTTON_PIN, callback, BUTTON_PUSHBUTTON | BUTTON_DEFAULT_HIGH | BUTTON_SET_PULLUP);

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);

  EEPROMwl.begin(EEPROM_LAYOUT_VERSION, AMOUNT_OF_INDEXES);
  Serial.begin(9600); // For debugging with
  mp3.begin();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  mp3.setVolume(22); // Set default volume
  play(); // This is non-blocking
}

void loop()
{
  mp3.loop();
  button.loop();
}

void play() {
  millisWhenLastTrackStartedOrFinished = millis();
  EEPROMwl.get(INDEX_CONFIGURATION_VAR1, track);
  Serial.print("Track from EEPROM: ");
  Serial.println(track);
  // FILES ON SD NEED TO BE IN THIS FORMAT: 01/0001_Something.mp3
  mp3.playFolderTrack16(1, track);
}


void saveTrack(int newTrack) {
{
      Serial.println("Saving to EEPROM");
      EEPROMwl.put(INDEX_CONFIGURATION_VAR1, newTrack);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    
  }
}

