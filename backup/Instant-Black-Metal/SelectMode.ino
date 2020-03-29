void SelectMode() {
  if (digitalRead(buttonModePin) == LOW) {
    mode = buttonMode;
  } else if (digitalRead(autoPlayModePin) == LOW) {
    mode = autoPlayMode;
  } else if (digitalRead(MIDIinModePin) == LOW) {
    mode = midiInMode;
  }
}
