void ReadButtons() {
  //-----button-------------------------------------------------------------------------
  byte buttonCurrentState = digitalRead(buttonPin); //button
  if (buttonCurrentState == LOW && buttonLastState == HIGH) {
    buttonState = 1; // pressed
  }
  if (buttonCurrentState == LOW && buttonLastState == LOW) {
    buttonState = 2; //hold
  }
  if (buttonCurrentState == HIGH && buttonLastState == LOW) {
    buttonState = 3; //released
  }
  if (buttonCurrentState == HIGH && buttonLastState == HIGH) {
    buttonState = 0; //unpressed
  }
  buttonLastState = buttonCurrentState;

  //-----octave-------------------------------------------------------------------------
  byte octaveValue = digitalRead(octavePin); //octave
  if (octaveValue == HIGH) {
    octave = 0;
  } else if (octaveValue == LOW) {
    octave = 12;
  }

  //-----time division-------------------------------------------------------------------
  byte timeDivisionValue = digitalRead(timeDivisionPin); //button
  if (timeDivisionValue == HIGH) {
    timeDivision = 8;
  } else if (timeDivisionValue == LOW) {
    timeDivision = 4;
  }

  //-----arpeggio------------------------------------------------------------------------
  byte arpeggioValue = digitalRead(arpeggioPin); //arpeggio
  if (arpeggioValue == HIGH) {
    arpeggioMode = 0;
    firstOscGain = gain;
  } else if (arpeggioValue == LOW) {
    arpeggioMode = 1;
    firstOscGain = gain + 32;
  }

}
