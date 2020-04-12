#include <MozziGuts.h>
#include <Oscil.h>
#include <ADSR.h>
#include <mozzi_rand.h>
#include <tables/saw8192_int8.h>
#include <mozzi_midi.h>
#include <MIDI.h>
#include <EventDelay.h>

#define CONTROL_RATE 256
MIDI_CREATE_DEFAULT_INSTANCE();
ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW1(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW2(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW3(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW4(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW5(SAW8192_DATA);


//chords
byte editCurrentChord = 0;
byte currentChord = 0; //0..5
byte chords[6][6] = {{0, 7, 12, 15, 64, 5}, {0, 7, 12, 14, 19, 0}, {0, 0, 0, 0, 0, 0}, {0, 7, 14, 15, 19, 0}, {0, 7, 12, 15, 19, 0}, {0, 7, 8, 15, 17, 0}};
byte baseFrq = 46; // tonic midi note coder

//gain
int gain = 4;

//octave
byte octave = 0;

//MIDI variables
byte currentPitch = 0; //cotains the last MIDI note code that has sent from keyboard
byte lastPitch;
byte playMidiNote = 0; //ON - play, OFF - stop

//ADSR variables
unsigned int attack, decay, sustain, releaseMs;
byte attackLevel;
byte decayLevel;

//pots readings
int gainValue, oldGainValue;
int atackValue, oldAtackValue;
int releaseValue, oldReleaseValue;
int chordValue, oldChordValue;
boolean nothing = true;
boolean noteIsOn = false;
EventDelay noteDelay;

void setup() {

  for (int i = 2; i < 8; i++) {
    pinMode(i, INPUT_PULLUP);
  }

  pinMode(11, OUTPUT); //LED pin
  digitalWrite(11, HIGH); //turn LED on

  MIDI.turnThruOff();
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.begin(MIDI_CHANNEL_OMNI);

  attackLevel = 127;
  decayLevel = 127;

  attack = 300;
  decay = 10;
  sustain = 65535;
  releaseMs = 2500;

  envelope.setADLevels(attackLevel, decayLevel);
  envelope.setTimes(attack, decay, sustain, releaseMs);

  randSeed();

  startMozzi(CONTROL_RATE);
  setOscFreq(baseFrq + octave);
}

void loop() {
  audioHook();
}

void updateControl() {

  MIDI.read();

  if (noteDelay.ready()) {
    nothing = true;
  } else {
    nothing = false;
  }

  readPots();

  envelope.update();
}

//-----Audio out----------------------------------------------------------------------------
int updateAudio() {
  if (nothing == true && noteIsOn == false) {
    return 0;
  } else {
    long asig = (long)
                (aSAW1.next() +
                 aSAW2.next() +
                 aSAW3.next() +
                 aSAW4.next() +
                 aSAW5.next()) * gain;
    asig >>= 8;
    return (int) (envelope.next() * asig);
  }
}

//-----Set oscillators frequencies-----------------------------------------------------------
void setOscFreq(byte tonic) {
  if (chords[currentChord][5] == 0) {
    aSAW1.setFreq(mtof(tonic + chords[currentChord][0]));
    aSAW2.setFreq(mtof(tonic + chords[currentChord][1]));
    aSAW3.setFreq(mtof(tonic + chords[currentChord][2]));
    aSAW4.setFreq(mtof(tonic + chords[currentChord][3]));
    aSAW5.setFreq(mtof(tonic + chords[currentChord][4]));
  }
  if (chords[currentChord][5] == 1) {
    aSAW1.setFreq(mtof(chords[currentChord][0]));
    aSAW2.setFreq(mtof(tonic + chords[currentChord][1]));
    aSAW3.setFreq(mtof(tonic + chords[currentChord][2]));
    aSAW4.setFreq(mtof(tonic + chords[currentChord][3]));
    aSAW5.setFreq(mtof(tonic + chords[currentChord][4]));
  }
  if (chords[currentChord][5] == 2) {
    aSAW1.setFreq(mtof(tonic + chords[currentChord][0]));
    aSAW2.setFreq(mtof(chords[currentChord][1]));
    aSAW3.setFreq(mtof(tonic + chords[currentChord][2]));
    aSAW4.setFreq(mtof(tonic + chords[currentChord][3]));
    aSAW5.setFreq(mtof(tonic + chords[currentChord][4]));
  }
  if (chords[currentChord][5] == 3) {
    aSAW1.setFreq(mtof(tonic + chords[currentChord][0]));
    aSAW2.setFreq(mtof(tonic + chords[currentChord][1]));
    aSAW3.setFreq(mtof(chords[currentChord][2]));
    aSAW4.setFreq(mtof(tonic + chords[currentChord][3]));
    aSAW5.setFreq(mtof(tonic + chords[currentChord][4]));
  }
  if (chords[currentChord][5] == 4) {
    aSAW1.setFreq(mtof(tonic + chords[currentChord][0]));
    aSAW2.setFreq(mtof(tonic + chords[currentChord][1]));
    aSAW3.setFreq(mtof(tonic + chords[currentChord][2]));
    aSAW4.setFreq(mtof(chords[currentChord][3]));
    aSAW5.setFreq(mtof(tonic + chords[currentChord][4]));
  }
  if (chords[currentChord][5] == 5) {
    aSAW1.setFreq(mtof(tonic + chords[currentChord][0]));
    aSAW2.setFreq(mtof(tonic + chords[currentChord][1]));
    aSAW3.setFreq(mtof(tonic + chords[currentChord][2]));
    aSAW4.setFreq(mtof(tonic + chords[currentChord][3]));
    aSAW5.setFreq(mtof(chords[currentChord][4]));
  }
}
