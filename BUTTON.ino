void GPIO0() {
  tombol = digitalRead(BUTTON);
  if (tombol == LOW && !buttonPressed) {
    buttonPressed = true;
    buttonPressStart = millis();  // Catat waktu ketika tombol mulai ditekan
  }
  // Jika tombol dilepas
  if (buttonPressed && tombol == HIGH) {
    buttonPressed = false;  // Reset status tombol
  }
  // Jika tombol ditekan selama 3 detik, toggle suhu antara 1 dan 0
  if (buttonPressed && (millis() - buttonPressStart >= 3000)) {
    if (fitur == 0) {
      fitur = 1;  // Set suhu menjadi 1
    } else {
      fitur = 0;  // Set suhu menjadi 0
    }
    buttonPressed = false;  // Reset status tombol
  }
}