void ReadPots() {
  //-----gain-----------------------------------------------------------------------------
  int gainValue = mozziAnalogRead(gainPin); //gain
  gain = gainValue >> 3; //scale the range to 0..127 by shifting right 3 times, i.e. dividing by 8

  //-----probability---------------------------------------------------------------------
  int randomnessValue = mozziAnalogRead(randomnessPin); //randomness
  if (randomnessValue >= 0 && randomnessValue < 250) {
    probability = 75; //25%
  } else if (randomnessValue >= 256 && randomnessValue < 507) {
    probability = 50; // 50%
  } else if (randomnessValue >= 512 && randomnessValue < 760) {
    probability = 25; //75%
  } else if (randomnessValue >= 768 && randomnessValue < 1024) {
    probability = 0; // 100%
  }

  //-----chord select knob - 6 different chord options----------------------------------
  int chordValue = mozziAnalogRead(chordPin); //chord
  if (chordValue >= 50 && chordValue < 180) {
    //---m + E drone--------
    chordFormula[0] = 0;
    chordFormula[1] = 7;
    chordFormula[2] = 12;
    chordFormula[3] = 15;
    chordFormula[4] = 64;
    droneNote = 1;
  }
  else if (chordValue >= 200 && chordValue < 280) {
    //---sus2----------------
    chordFormula[0] = 0;
    chordFormula[1] = 7;
    chordFormula[2] = 12;
    chordFormula[3] = 14;
    chordFormula[4] = 19;
    droneNote = 0;
  }
  else if (chordValue >= 300 && chordValue < 380) {
    //---m7(b9)--------------
    chordFormula[0] = 0;
    chordFormula[1] = 15;
    chordFormula[2] = 10;
    chordFormula[3] = 12;
    chordFormula[4] = 13;
    droneNote = 0;
  }
  else if (chordValue >= 400 && chordValue < 480) {
    //---m add9---------------
    chordFormula[0] = 0;
    chordFormula[1] = 7;
    chordFormula[2] = 14;
    chordFormula[3] = 15;
    chordFormula[4] = 19;
    droneNote = 0;
  }
  else if (chordValue >= 500 && chordValue < 580) {
    //---m---------------
    chordFormula[0] = 0;
    chordFormula[1] = 7;
    chordFormula[2] = 12;
    chordFormula[3] = 15;
    chordFormula[4] = 19;
    droneNote = 0;
  }
  else if (chordValue >= 700 && chordValue < 800) {
    //---m9---------------
    chordFormula[0] = 0;
    chordFormula[1] = 7;
    chordFormula[2] = 8;
    chordFormula[3] = 15;
    chordFormula[4] = 17;
    droneNote = 0;
  }

}
