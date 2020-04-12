void readPots() {
  //gain
  gainValue = mozziAnalogRead(A0);
  gain = gainValue >> 4;

  //attack
  atackValue = mozziAnalogRead(A1);
  if (abs(atackValue - oldAtackValue) > 8) {
    attack = 10 + atackValue;
    oldAtackValue = atackValue;
    envelope.setAttackTime(attack);
  }

  //release
  releaseValue = mozziAnalogRead(A2);
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
      currentChord = 0;
    }
    else if (chordValue == 3) {
      //---sus2----------------
      currentChord = 1;
    }
    else if (chordValue == 4) {
      //---m7(b9)--------------
      currentChord = 2;
    }
    else if (chordValue == 5) {
      //---m add9---------------
      currentChord = 3;
    }
    else if (chordValue == 6) {
      //---m---------------
      currentChord = 4;
    }
    else if (chordValue == 7) {
      //---m9---------------
      currentChord = 5;
    }
    oldChordValue = chordValue;
  }
}
