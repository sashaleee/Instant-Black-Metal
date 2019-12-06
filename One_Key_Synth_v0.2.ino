#include <MozziGuts.h>
#include <Oscil.h>
#include <EventDelay.h>
#include <ADSR.h>
#include <mozzi_rand.h>
#include <tables/cos8192_int8.h>
#include <tables/saw8192_int8.h>
#include <mozzi_midi.h>

#define CONTROL_RATE 64

ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

int baseFrq;
int mltpl = 3;
int vol = 8;
byte newLength = 0;

//long lastNote = 0;
//long time_ = 0;
//long duration = 3800000;

EventDelay newNote;

// harmonics
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW1(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW2(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW3(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW4(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW5(SAW8192_DATA);

// volume controls
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol1(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol2(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol3(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol4(COS8192_DATA);
Oscil<COS8192_NUM_CELLS, CONTROL_RATE> kVol5(COS8192_DATA);

char v1, v2, v3, v4, v5;
unsigned int attack, decay, sustain, release_ms;
byte attack_level;
byte decay_level;

void setup() {
  //  Serial.begin(9600);

  newNote.set(4000);

  attack_level = 8;
  decay_level = 0;
  attack = 300;
  decay = 100;
  sustain = 100;
  release_ms = 500;

  envelope.setADLevels(attack_level, decay_level);
  envelope.setTimes(attack, decay, sustain, release_ms);
  randSeed();
  pinMode(0, INPUT_PULLUP);
  pinMode(1, INPUT_PULLUP);
  pinMode(2, INPUT_PULLUP);
  pinMode(3, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);
  pinMode(7, INPUT_PULLUP);
  pinMode(8, INPUT_PULLUP);


  kVol1.setFreq(4.43f); // more of a pulse
  kVol2.setFreq(0.0245f);
  kVol3.setFreq(0.019f);
  kVol4.setFreq(0.07f);
  kVol5.setFreq(0.047f);
  v1 = v2 = v3 = v4 = v5 = 64;

  startMozzi(CONTROL_RATE);
}

void loop() {
  audioHook();
}

void updateControl() {
  static int buttonPrevious;
  int buttonCurrent = digitalRead(8);

  boolean BM = digitalRead(0);
  boolean pad = digitalRead(1);
  boolean drone = digitalRead(2);
  boolean autoPilot = digitalRead(3);
  boolean midiIn = digitalRead(6);

  if (BM == false) {
    vol = 4;
    v1 = v2 = v3 = v4 = v5 = 64;
  } else if (drone == false) {
    vol = 8;
  } else if (pad == false) {
    vol = 7;
    v1 = v2 = v3 = v4 = v5 = 64;
  } else if (autoPilot == false) {
    v1 = v2 = v3 = v4 = v5 = 64;
    vol = 4;
    envelope.noteOn();
    if (newNote.ready()) {
      baseFrq = int(rand(55, 68));
      aSAW1.setFreq(mtof(baseFrq));
      aSAW2.setFreq(mtof(baseFrq + 7));
      aSAW3.setFreq(mtof(baseFrq + 12));
      aSAW4.setFreq(mtof(baseFrq + 15));
      aSAW5.setFreq(mtof(76));
      newLength = random(0, 100);
      //      Serial.println(newLength);
      if (newLength > 75) {
        newNote.set(1000);
      } else if (newLength > 40) {
        newNote.set(2000);
      } else if (newLength <= 40 ) {
        newNote.set(8000);
      }
      newNote.start();
    }
    //        if (mozziMicros() - lastNote > duration) {
    //          baseFrq = int(rand(55, 68));
    //          aSAW1.setFreq(mtof(baseFrq));
    //          aSAW2.setFreq(mtof(baseFrq + 7));
    //          aSAW3.setFreq(mtof(baseFrq + 12));
    //          aSAW4.setFreq(mtof(baseFrq + 15));
    //          aSAW5.setFreq(mtof(76));
    //          lastNote = mozziMicros();
    //        }

  } else if (midiIn == false) {
  }

  if (buttonCurrent == HIGH && autoPilot == true) {
    envelope.noteOff();
  }
  if (buttonCurrent == LOW && autoPilot == true) {
    envelope.noteOn();
    if (drone == false) {
      v1 = kVol1.next();
      v2 = kVol2.next();
      v3 = kVol3.next();
      v4 = kVol4.next();
      v5 = kVol5.next() >> 1;
    }
  }

  if (buttonPrevious == HIGH && buttonCurrent == LOW) {
    if (BM == false) {
      baseFrq = int(rand(55, 68));
      aSAW1.setFreq(mtof(baseFrq));
      aSAW2.setFreq(mtof(baseFrq + 7));
      aSAW3.setFreq(mtof(baseFrq + 12));
      aSAW4.setFreq(mtof(baseFrq + 15));
      aSAW5.setFreq(mtof(76));
    }
    if (drone == false) {
      baseFrq = rand(1000, 2000);
      aSAW1.setFreq(baseFrq / (0 + mltpl));
      aSAW2.setFreq(baseFrq / (1 + mltpl));
      aSAW3.setFreq(baseFrq / (2 + mltpl));
      aSAW4.setFreq(baseFrq / (3 + mltpl));
      aSAW5.setFreq(baseFrq / (0 + mltpl));
    }
    if (pad == false) {
      baseFrq = int(rand(40, 53));
      aSAW1.setFreq(mtof(baseFrq));
      aSAW2.setFreq(mtof(baseFrq + 7));
      aSAW3.setFreq(mtof(baseFrq + 24));
      aSAW4.setFreq(mtof(baseFrq + 17));
      aSAW5.setFreq(mtof(baseFrq + 19));
    }
  }
  envelope.update();
  buttonPrevious = buttonCurrent;
}

int updateAudio() {

  long asig = (long)
              aSAW1.next() * v1 +
              aSAW2.next() * v2 +
              aSAW3.next() * v3 +
              aSAW4.next() * v4 +
              aSAW5.next() * v5;
  asig >>= vol;
  return (int) (envelope.next() * asig);
}
