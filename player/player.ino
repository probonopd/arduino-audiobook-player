
// Using an Arduino Pro Mini (with voltage regulator)
// proved to produce too many unwanted noises.
// Hence using a bare ATmega168P chip without oscillator.
// Core and chip pinout: https://github.com/MCUdude/MiniCore
// Not using a serial programmer, uploading directly using USBASP with Ctrl-Shift-U
// Permissions: sudo chmod 666 /dev/bus/usb/*/*
// An ATTiny would also do (with another core)
// Files must be in:
// 01/0001_SomeOptionalTitle.mp3

#include <SoftwareSerial.h>
#include <DFMiniMp3.h>
#include <EEPROMWearLevel.h>

// See https://github.com/PRosenb/EEPROMWearLevel
#define EEPROM_LAYOUT_VERSION 0
#define AMOUNT_OF_INDEXES 2
#define INDEX_CONFIGURATION_VAR1 0

// Clap sensor
#define CLAPIN  2 // pin must be interrupt-capable
#define CLAP_DELAY 1000 // min gap between claps to trigger
volatile boolean clap = false; // clap detected state - "volatile" means that ISR writes it
unsigned long clap_time, last_clap_time = 0; // clap time records

// Advance declaration so that we can refer to the function
// before we define it
void play();

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

int16_t track;
const long interval = 5000; // interval at which to ask the player for the current track (milliseconds)
unsigned long previousMillis = 0;

void setup()
{
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(CLAPIN, INPUT_PULLUP);
  attachInterrupt(                  // register Interrupt Service Routine (ISR):
    digitalPinToInterrupt(CLAPIN),  //   pin to watch for interrupt
    heard_clap,                     //   void function to call on interrupt
    FALLING                         //   trigger interrupt on HIGH → LOW change
  );
  EEPROMwl.begin(EEPROM_LAYOUT_VERSION, AMOUNT_OF_INDEXES);
  Serial.begin(9600); // For debugging with
  mp3.begin();

  digitalWrite(LED_BUILTIN, HIGH);
  delay(1000);
  digitalWrite(LED_BUILTIN, LOW);
  delay(100);
  // myMP3.volume(20);
  play(); // This is non-blocking
}

void loop()
{
  if (clap) { // we heard a clap from ISR
    clap = false; // make sure we don't trigger again too soon
    last_clap_time = clap_time; // store old clap time
    clap_time = millis(); // note current clap time
    if (clap_time - last_clap_time > CLAP_DELAY) { // if the last clap was some time ago:
      Serial.println("Clap!"); //   notify
      mp3.nextTrack();
    }
  }

  mp3.loop();
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    askPlayerForTrack();

  }
}

void play() {
  EEPROMwl.get(INDEX_CONFIGURATION_VAR1, track);
  Serial.print("EEPROMwl.get(INDEX_CONFIGURATION_VAR1, track); ");
  Serial.println(track);
  // FILES ON SD NEED TO BE IN THIS FORMAT: 01/0001_Something.mp3
  mp3.playFolderTrack16(1, track);
}


void askPlayerForTrack() {
  int16_t current_track;
  current_track = mp3.getCurrentTrack(DfMp3_PlaySource_Sd);
  EEPROMwl.get(INDEX_CONFIGURATION_VAR1, track);
  // If the player answered with anoth track than the last one we knew, save it and blink short
  if (current_track != track) {
    Serial.print("mp3.getCurrentTrack(DfMp3_PlaySource_Sd): ");
    Serial.println(current_track);
    // Currently playing track is 0 if no card is inserted, let's not save that!
    if (current_track > 0) {
      Serial.println("Saving to EEPROM");
      EEPROMwl.put(INDEX_CONFIGURATION_VAR1, current_track);
      digitalWrite(LED_BUILTIN, HIGH);
      delay(100);
      digitalWrite(LED_BUILTIN, LOW);
      delay(100);
    }
  }
}

void heard_clap() {
  clap = true; // just set clap state in ISR and leave as soon as possible
}
