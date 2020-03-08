void Sync() {
  byte  sync = digitalRead(syncPin); //sync pulse edge detection

  if (sync != prevSync) {
    if (sync == HIGH) {
      syncPulseCount++;
      arpeggioCount++;
      if (arpeggioCount > arpeggioSteps) {
        arpeggioCount = 0;
      }
      syncGate = 1;
    }
    else {
      syncGate = 0;
    }
  }
  prevSync = sync;
}
