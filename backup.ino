//#include <MozziGuts.h>
//#include <Oscil.h>
//#include <ADSR.h>
//#include <mozzi_rand.h>
//#include <tables/saw8192_int8.h>
//#include <mozzi_midi.h>
//#include <MIDI.h>
//#include <EventDelay.h>
//
//#define CONTROL_RATE 256
//MIDI_CREATE_DEFAULT_INSTANCE();
//ADSR <CONTROL_RATE, AUDIO_RATE> envelope;
//
//Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW1(SAW8192_DATA);
//Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW2(SAW8192_DATA);
//Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW3(SAW8192_DATA);
//Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW4(SAW8192_DATA);
//Oscil<SAW8192_NUM_CELLS, AUDIO_RATE> aSAW5(SAW8192_DATA);
//
////pins
//byte octavePin = 7;
//
////chord formula
//byte droneNote = 0; // if it equals 1 the fifth note of the chord is constant
//byte chordFormula[5] = {0, 7, 14, 15, 19}; //chord intervals in semitones
//byte baseFrq = 46; // tonic midi note coder
//
////gain
//int gain = 4;
//
////octave
//byte octave = 0;
//
////MIDI variables
//byte currentPitch = 0; //cotains the last MIDI note code that has sent from keyboard
//byte lastPitch;
//byte playMidiNote = 0; //ON - play, OFF - stop
//
////ADSR variables
//unsigned int attack, decay, sustain, releaseMs;
//byte attackLevel;
//byte decayLevel;
//
////pots readings
//int gainValue, oldGainValue;
//int atackValue, oldAtackValue;
//int releaseValue, oldReleaseValue;
//int chordValue, oldChordValue;
//boolean nothing = true;
//boolean noteIsOn = false;
//EventDelay noteDelay;
//
//void setup() {
//  pinMode(octavePin, INPUT_PULLUP);
//
//  for (int i = 2; i < 8; i++) {
//    pinMode(i, INPUT_PULLUP);
//  }
//
//  pinMode(10, OUTPUT); //LED pin
//  digitalWrite(10, HIGH); //turn LED on
//
//  MIDI.turnThruOff();
//  MIDI.setHandleNoteOn(handleNoteOn);
//  MIDI.setHandleNoteOff(handleNoteOff);
//  MIDI.setHandleControlChange(handleControlChange);
//  MIDI.begin(MIDI_CHANNEL_OMNI);
//
//  attackLevel = 127;
//  decayLevel = 127;
//
//  attack = 300;
//  decay = 10;
//  sustain = 65535;
//  releaseMs = 2500;
//
//  envelope.setADLevels(attackLevel, decayLevel);
//  envelope.setTimes(attack, decay, sustain, releaseMs);
//
//  randSeed();
//
//  startMozzi(CONTROL_RATE);
//  setOscFreq(baseFrq + octave);
//}
//
//void loop() {
//  audioHook();
//}
//
//void updateControl() {
//
//  MIDI.read();
//
//  if (noteDelay.ready()) {
//    nothing = true;
//  } else {
//    nothing = false;
//  }
//
//  readPots();
//
//  envelope.update();
//}
//
////-----Audio out----------------------------------------------------------------------------
//int updateAudio() {
//  if (nothing == true && noteIsOn == false) {
//    return 0;
//  } else {
//    long asig = (long)
//                (aSAW1.next() +
//                 aSAW2.next() +
//                 aSAW3.next() +
//                 aSAW4.next() +
//                 aSAW5.next()) * gain;
//    asig >>= 8;
//    return (int) (envelope.next() * asig);
//  }
//}
//
////-----MIDI Callbacks-----------------------------------------------------------------------
//
////---note on-------------
//void handleNoteOn(byte channel, byte pitch, byte velocity) {
//  envelope.noteOff();
//  envelope.update();
//  currentPitch = pitch;
//  playMidiNote = true;
//  if (pitch > 24) {
//    setOscFreq(currentPitch + octave);
//  } else {
//    baseFrq = int(rand(45, 57)); //new random tonic from G# to G#
//    setOscFreq(baseFrq + octave); // set new chord tonic
//  }
//  noteIsOn = true;
//  envelope.noteOn();
//  envelope.update();
//
//}
//
////---note off-------------
//void handleNoteOff(byte channel, byte pitch, byte velocity) {
//  if (pitch == currentPitch) {
//    playMidiNote = false;
//    noteIsOn = false;
//    envelope.noteOff();
//    noteDelay.start(releaseMs + 50);
//  }
//
//}
//
//void handleControlChange(byte channel, byte number, byte value) {
//
//  switch (number) {
//    case 55:
//      if (value > 100) {
//        octave = 12;
//      } else {
//        octave = 0;
//      }
//      break;
//
//    case 19:
//      switch (value) {
//        case 0:
//          //---m + E drone--------
//          chordFormula[0] = 0;
//          chordFormula[1] = 7;
//          chordFormula[2] = 12;
//          chordFormula[3] = 15;
//          chordFormula[4] = 64;
//          droneNote = 1;
//          break;
//
//        case 20:
//          //---sus2----------------
//          chordFormula[0] = 0;
//          chordFormula[1] = 7;
//          chordFormula[2] = 12;
//          chordFormula[3] = 14;
//          chordFormula[4] = 19;
//          droneNote = 0;
//          break;
//
//        case 40:
//          //---m7(b9)--------------
//          //          chordFormula[0] = 0;
//          //          chordFormula[1] = 15;
//          //          chordFormula[2] = 10;
//          //          chordFormula[3] = 12;
//          //          chordFormula[4] = 13;
//          //          droneNote = 0;
//
//          //---note---------------
//          chordFormula[0] = 0;
//          chordFormula[1] = 0;
//          chordFormula[2] = 0;
//          chordFormula[3] = 0;
//          chordFormula[4] = 0;
//          droneNote = 0;
//          break;
//
//        case 60:
//          //---m add9---------------
//          chordFormula[0] = 0;
//          chordFormula[1] = 7;
//          chordFormula[2] = 14;
//          chordFormula[3] = 15;
//          chordFormula[4] = 19;
//          droneNote = 0;
//          break;
//
//        case 80:
//          //---m---------------
//          chordFormula[0] = 0;
//          chordFormula[1] = 7;
//          chordFormula[2] = 12;
//          chordFormula[3] = 15;
//          chordFormula[4] = 19;
//          droneNote = 0;
//          break;
//
//        case 100:
//          //---m9---------------
//          chordFormula[0] = 0;
//          chordFormula[1] = 7;
//          chordFormula[2] = 8;
//          chordFormula[3] = 15;
//          chordFormula[4] = 17;
//          droneNote = 0;
//          break;
//      }
//      break;
//
//    case 0:
//      attack = 10 + (value * 8);
//      envelope.setAttackTime(attack);
//      break;
//
//    case 1:
//      releaseMs = 30 + (value * 16);
//      envelope.setReleaseTime(releaseMs);
//      break;
//  }
//}
//
////-----Set oscillators frequencies-----------------------------------------------------------
//void setOscFreq(byte tonic) {
//  aSAW1.setFreq(mtof(tonic + chordFormula[0]));
//  aSAW2.setFreq(mtof(tonic + chordFormula[1]));
//  aSAW3.setFreq(mtof(tonic + chordFormula[2]));
//  aSAW4.setFreq(mtof(tonic + chordFormula[3]));
//  if (droneNote == 0) {
//    aSAW5.setFreq(mtof(tonic + chordFormula[4]));
//  }
//  else if (droneNote > 0) {
//    aSAW5.setFreq(mtof(chordFormula[4]));
//  }
//}
//



//void readPots() {
//  //gain
//  gainValue = mozziAnalogRead(A0); //gain
//  gain = gainValue >> 4;
//
//  //attack
//  atackValue = mozziAnalogRead(A1); //gain
//  if (abs(atackValue - oldAtackValue) > 8) {
//    attack = 10 + atackValue;
//    oldAtackValue = atackValue;
//    envelope.setAttackTime(attack);
//  }
//
//  //release
//  releaseValue = mozziAnalogRead(A2); //gain
//  if (abs(releaseValue - oldReleaseValue) > 8) {
//    releaseMs = 30 + releaseValue * 2;
//    oldReleaseValue = releaseValue;
//    envelope.setReleaseTime(releaseMs);
//  }
//
//  //chord
//  //-----chord select knob - 6 different chord options----------------------------------
//  for (int i = 2; i < 8; i++) {
//    if (digitalRead(i) == LOW) {
//      chordValue = i;
//    }
//  }
//  //oldChordValue
//  if (chordValue != oldChordValue) {
//    if (chordValue == 2) {
//      //---m + E drone--------
//      chordFormula[0] = 0;
//      chordFormula[1] = 7;
//      chordFormula[2] = 12;
//      chordFormula[3] = 15;
//      chordFormula[4] = 64;
//      droneNote = 1;
//    }
//    else if (chordValue == 3) {
//      //---sus2----------------
//      chordFormula[0] = 0;
//      chordFormula[1] = 7;
//      chordFormula[2] = 12;
//      chordFormula[3] = 14;
//      chordFormula[4] = 19;
//      droneNote = 0;
//    }
//    else if (chordValue == 4) {
//      //---m7(b9)--------------
//      //      chordFormula[0] = 0;
//      //      chordFormula[1] = 15;
//      //      chordFormula[2] = 10;
//      //      chordFormula[3] = 12;
//      //      chordFormula[4] = 13;
//      //      droneNote = 0;
//      chordFormula[0] = 0;
//      chordFormula[1] = 0;
//      chordFormula[2] = 0;
//      chordFormula[3] = 0;
//      chordFormula[4] = 0;
//      droneNote = 0;
//    }
//    else if (chordValue == 5) {
//      //---m add9---------------
//      chordFormula[0] = 0;
//      chordFormula[1] = 7;
//      chordFormula[2] = 14;
//      chordFormula[3] = 15;
//      chordFormula[4] = 19;
//      droneNote = 0;
//    }
//    else if (chordValue == 6) {
//      //---m---------------
//      chordFormula[0] = 0;
//      chordFormula[1] = 7;
//      chordFormula[2] = 12;
//      chordFormula[3] = 15;
//      chordFormula[4] = 19;
//      droneNote = 0;
//    }
//    else if (chordValue == 7) {
//      //---m9---------------
//      chordFormula[0] = 0;
//      chordFormula[1] = 7;
//      chordFormula[2] = 8;
//      chordFormula[3] = 15;
//      chordFormula[4] = 17;
//      droneNote = 0;
//    }
//    oldChordValue = chordValue;
//  }
//}
