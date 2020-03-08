#include <MozziGuts.h>
#include <Oscil.h>
#include <ADSR.h>
#include <mozzi_rand.h>
#include <tables/saw8192_int8.h>
#include <mozzi_midi.h>
#include <MIDI.h>

//-----pins--------------------------------------------------------------------------
#define syncPin 2 //sync pulses from external clock
//---rotary switch------------
#define buttonModePin 3
#define autoPlayModePin 4
#define MIDIinModePin 5
//---arpeggio----------------
#define arpeggioPin 6
//---Octave------------------
#define octavePin 7
//---Time Division-----------
#define timeDivisionPin 11
//---button------------------
#define buttonPin 8
//---analog pins-------------
#define gainPin 0
#define chordPin 1
#define randomnessPin 2
//-----modes--------------------------------------------------------------------------
#define autoPlayMode 1
#define midiInMode 2
#define buttonMode 3

//-----chord formula-------------------------------------------------------------------
byte octave = 0;
byte droneNote = 0; // if it equals 1 the fifth note of the chord is constant
byte chordFormula[5] = {0, 7, 14, 15, 19}; //chord intervals in semitones
byte baseFrq = 46; // tonic midi note code

//-----arpeggio------------------------------------------------------------------------
byte arpeggioMode = 0;
byte arpeggioFormula[8] = {0, 7, 14, 15, 19, 15, 14, 7}; // arpeggio steps in semitones

//-----random notes variables----------------------------------------------------------
byte probability = 0; //percentage of a chance to change the chord
byte timeDivision = 8; //check probability for chanching chord every timeDivision pulses

//-----gain----------------------------------------------------------------------------
int gain = 64;
int firstOscGain = 64; //arpeggio voice gain

//-----sync variables------------------------------------------------------------------
byte syncPulseCount = 0; // counts pulses from external clock
byte arpeggioCount = 0; // counts arppegio steps from 0 to 7
byte arpeggioSteps = 7; // 8 steps from 0 to 7
byte prevSync; //for detection rise edge of sync pulse
byte syncGate = 0;

//-----Button States-------------------------------------------------------------------
byte buttonLastState;
byte buttonState; // 0 = unpressed, 1 = pressed, 2 = hold, 3 = released

//-----MIDI variables------------------------------------------------------------------
byte currentPitch = 0; //cotains the last MIDI note code that has sent from keyboard
byte lastPitch;
byte playMidiNote = 0; //ON - play, OFF - stop

//-----modes---------------------------------------------------------------------------
byte mode;
byte lastMode;

//-----ADSR variables-------------------------------------------------------------------
unsigned int attack, decay, sustain, releaseMs;
byte attackLevel;
byte decayLevel;

#define CONTROL_RATE 128
MIDI_CREATE_DEFAULT_INSTANCE();
ADSR <CONTROL_RATE, AUDIO_RATE> envelope;

Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW1(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW2(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW3(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW4(SAW8192_DATA);
Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW5(SAW8192_DATA);

void setup() {
  //  Serial.begin(9600);
  MIDI.turnThruOff();
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.begin(MIDI_CHANNEL_OMNI); // Listen to all MIDI channel;

  attackLevel = 6;
  decayLevel = 5;
  attack = 100;
  decay = 100;
  sustain = 100;
  releaseMs = 800;

  envelope.setADLevels(attackLevel, decayLevel);
  envelope.setTimes(attack, decay, sustain, releaseMs);

  randSeed();

  pinMode(MIDIinModePin, INPUT_PULLUP);
  pinMode(autoPlayModePin, INPUT_PULLUP);
  pinMode(buttonModePin, INPUT_PULLUP);
  pinMode(arpeggioPin, INPUT_PULLUP);
  pinMode(octavePin, INPUT_PULLUP);
  pinMode(buttonPin, INPUT_PULLUP);
  pinMode(timeDivisionPin, INPUT_PULLUP);
  pinMode(syncPin, INPUT);
  //  pinMode(13, OUTPUT);

  startMozzi(CONTROL_RATE);
  setOscFreq(baseFrq);
}

void loop() {
  audioHook();
}

void updateControl() {

  MIDI.read();
  Sync();
  ReadButtons();
  ReadPots();
  SelectMode();

  //-----AutoPlay mode---------------------------------------------------------------------
  if (mode == autoPlayMode) {
    envelope.noteOn();

    // restart pulse count and arpeggio
    if (buttonState == 1) { //if the button is pressed
      syncPulseCount = 0;
      arpeggioCount = 0;
    }

    if (arpeggioMode == 1) {
      // set the frequency of first oscillator on every sync pulse
      if (syncGate == 1) {
        aSAW1.setFreq(mtof(arpeggioFormula[arpeggioCount] + baseFrq + octave));
      }
    }

    //every "timeDivision" sync pulses throw a random number and check if the chord tonic has to be changed
    if ((syncPulseCount % timeDivision == 0) && (syncGate == 1)) {
      byte isNewTonic = random(0, 100);
      if (isNewTonic >= probability) { //50% chanse to change chord
        baseFrq = int(rand(45, 57)) + octave; //new random tonic from G# to G#
        setOscFreq(baseFrq); // set new chord tonic
      }
    }

    //-----MIDI in mode---------------------------------------------------------------------
  } else if (mode == midiInMode) {

    if (playMidiNote == true) {
      envelope.noteOn();
      if (arpeggioMode == 1) {
        if (syncGate == 1) {
          aSAW1.setFreq(mtof(arpeggioFormula[arpeggioCount] + currentPitch + octave));
        }
      }
    } else {
      envelope.noteOff();
    }

    //-----Button mode----------------------------------------------------------------------
  } else if (mode == buttonMode) {
    if (buttonState == 1) {
      baseFrq = int(rand(45, 57)) + octave; //new random tonic from G# to G#
      setOscFreq(baseFrq); // set new chord tonic
    }
    if (buttonState == 2) {
      envelope.noteOn();

      if (arpeggioMode == 1) {
        if (syncGate == 1) {
          aSAW1.setFreq(mtof(arpeggioFormula[arpeggioCount] + baseFrq + octave));
        }
      }
    }
    if (buttonState == 3) {
      envelope.noteOff();

      if (arpeggioMode == 1) {
        syncPulseCount = 0;
        arpeggioCount = 0;
      }
    }
  }

  envelope.update();
  lastMode = mode;
}

//-----Audio out----------------------------------------------------------------------------
int updateAudio() {
  long asig = (long)
              (aSAW1.next() * firstOscGain) +
              (aSAW2.next() +
               aSAW3.next() +
               aSAW4.next() +
               aSAW5.next()) * gain;
  asig >>= 4;
  return (int) (envelope.next() * asig);
}

//-----MIDI Callbacks-----------------------------------------------------------------------
//---note on-------------
void handleNoteOn(byte channel, byte pitch, byte velocity) {
  if (mode == midiInMode) {
    syncPulseCount = 0;
    //    arpeggioCount = 0;
    currentPitch = pitch;
    playMidiNote = true;
    setOscFreq(currentPitch + octave);
  }
}

//---note off-------------
void handleNoteOff(byte channel, byte pitch, byte velocity) {
  if (mode == midiInMode) {
    if (pitch == currentPitch) {
      playMidiNote = false;
    }
  }
}

//-----Set oscillators frequencies-----------------------------------------------------------
void setOscFreq(byte tonic) {
  aSAW1.setFreq(mtof(tonic + chordFormula[0]));
  aSAW2.setFreq(mtof(tonic + chordFormula[1]));
  aSAW3.setFreq(mtof(tonic + chordFormula[2]));
  aSAW4.setFreq(mtof(tonic + chordFormula[3]));
  if (droneNote == 0) {
    aSAW5.setFreq(mtof(tonic + chordFormula[4]));
  }
  else if (droneNote > 0) {
    aSAW5.setFreq(mtof(chordFormula[4]));
  }
}
