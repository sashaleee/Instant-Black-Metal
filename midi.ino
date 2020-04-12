//-----MIDI Callbacks-----------------------------------------------------------------------

//---note on-------------
void handleNoteOn(byte channel, byte pitch, byte velocity) {
  envelope.noteOff();
  envelope.update();
  currentPitch = pitch;
  playMidiNote = true;

  if (pitch > 24) {
    setOscFreq(currentPitch + octave);
  }

  if (pitch == 0) {
    baseFrq = int(rand(45, 63)); //new random tonic from G# to G#
    setOscFreq(baseFrq + octave); // set new chord tonic
  }
  if (pitch == 1) {
    baseFrq = int(rand(57, 74)); //new random tonic from G# to G#
    setOscFreq(baseFrq + octave); // set new chord tonic
  }

  if (pitch > 1 && pitch < 4) {

    currentChord = byte(rand(0, 6));

    if (pitch == 2) {
      baseFrq = int(rand(45, 63)); //new random tonic from G# to G#
      setOscFreq(baseFrq + octave); // set new chord tonic
    }

    if (pitch == 3) {
      baseFrq = int(rand(57, 74)); //new random tonic from G# to G#
      setOscFreq(baseFrq + octave); // set new chord tonic
    }
  }

  noteIsOn = true;
  envelope.noteOn();
  envelope.update();
}

//---note off-------------
void handleNoteOff(byte channel, byte pitch, byte velocity) {
  if (pitch == currentPitch) {
    playMidiNote = false;
    noteIsOn = false;
    envelope.noteOff();
    noteDelay.start(releaseMs + 50);
  }

}

void handleControlChange(byte channel, byte number, byte value) {

  switch (number) {
    //octave
    case 55:
      if (value > 100) {
        octave = 12;
      } else {
        octave = 0;
      }
      break;
    //select chord
    case 19:
      switch (value) {
        case 0:
          currentChord = 0;
          //---m + E drone--------
          break;
        case 20:
          //---sus2----------------
          currentChord = 1;
          break;
        case 40:
          //-----------------
          currentChord = 2;
          break;
        case 60:
          //---m add9---------------
          currentChord = 3;
          break;
        case 80:
          //---m---------------
          currentChord = 4;
          break;
        case 100:
          //---m9---------------
          currentChord = 5;
          break;
      }
      break;
    //AR envelope
    case 0:
      attack = 10 + (value * 8);
      envelope.setAttackTime(attack);
      break;

    case 1:
      releaseMs = 30 + (value * 16);
      envelope.setReleaseTime(releaseMs);
      break;
    // edit chord mode
    case 64:
      editCurrentChord = !editCurrentChord;
      break;
    //chord editing
    case 2:
      if (editCurrentChord > 0) {
        chords[currentChord][0] = map(value, 0, 127, 0, 24);
      }
      break;
    case 3:
      if (editCurrentChord > 0) {
        chords[currentChord][1] = map(value, 0, 127, 0, 24);
      }
      break;
    case 4:
      if (editCurrentChord > 0) {
        chords[currentChord][2] = map(value, 0, 127, 0, 24);
      }
      break;
    case 5:
      if (editCurrentChord > 0) {
        chords[currentChord][3] = map(value, 0, 127, 0, 24);
      }
      break;
    case 6:
      if (editCurrentChord > 0) {
        chords[currentChord][4] = map(value, 0, 127, 0, 24);
      }
      break;
    case 7:
      if (editCurrentChord > 0) {
        chords[currentChord][5] = map(value, 0, 127, 0, 24);
      }
      break;
  }
}
