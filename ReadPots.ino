void readPots() {
  //gain
  gainValue = mozziAnalogRead(A0); //gain
  gain = gainValue >> 4;

  //attack
  atackValue = mozziAnalogRead(A1); //gain
  if (abs(atackValue - oldAtackValue) > 8) {
    attack = 10 + atackValue;
    oldAtackValue = atackValue;
    envelope.setAttackTime(attack);
  }

  //release
  releaseValue = mozziAnalogRead(A2); //gain
  if (abs(releaseValue - oldReleaseValue) > 8) {
    releaseMs = 30 + releaseValue * 2;
    oldReleaseValue = releaseValue;
    envelope.setReleaseTime(releaseMs);
  }

  //chord
  //-----chord select knob - 6 different chord options----------------------------------
  for (int i = 2; i < 8; i++) {
    if (digitalRead(i) == LOW) {
      chordValue = i;
    }
  }
  //oldChordValue
  if (chordValue != oldChordValue) {
    if (chordValue == 2) {
      //---m + E drone--------
      chordFormula[0] = 0;
      chordFormula[1] = 7;
      chordFormula[2] = 12;
      chordFormula[3] = 15;
      chordFormula[4] = 64;
      droneNote = 1;
    }
    else if (chordValue == 3) {
      //---sus2----------------
      chordFormula[0] = 0;
      chordFormula[1] = 7;
      chordFormula[2] = 12;
      chordFormula[3] = 14;
      chordFormula[4] = 19;
      droneNote = 0;
    }
    else if (chordValue == 4) {
      //---m7(b9)--------------
      //      chordFormula[0] = 0;
      //      chordFormula[1] = 15;
      //      chordFormula[2] = 10;
      //      chordFormula[3] = 12;
      //      chordFormula[4] = 13;
      //      droneNote = 0;
      chordFormula[0] = 0;
      chordFormula[1] = 0;
      chordFormula[2] = 0;
      chordFormula[3] = 0;
      chordFormula[4] = 0;
      droneNote = 0;
    }
    else if (chordValue == 5) {
      //---m add9---------------
      chordFormula[0] = 0;
      chordFormula[1] = 7;
      chordFormula[2] = 14;
      chordFormula[3] = 15;
      chordFormula[4] = 19;
      droneNote = 0;
    }
    else if (chordValue == 6) {
      //---m---------------
      chordFormula[0] = 0;
      chordFormula[1] = 7;
      chordFormula[2] = 12;
      chordFormula[3] = 15;
      chordFormula[4] = 19;
      droneNote = 0;
    }
    else if (chordValue == 7) {
      //---m9---------------
      chordFormula[0] = 0;
      chordFormula[1] = 7;
      chordFormula[2] = 8;
      chordFormula[3] = 15;
      chordFormula[4] = 17;
      droneNote = 0;
    }
    oldChordValue = chordValue;
  }
}
