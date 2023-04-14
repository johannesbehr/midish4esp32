/*
  MIDI note player

  This sketch shows how to use the serial transmit pin (pin 1) to send MIDI note data.
  If this circuit is connected to a MIDI synth, it will play the notes
  F#-0 (0x1E) to F#-5 (0x5A) in sequence.

  The circuit:
  - digital in 1 connected to MIDI jack pin 5
  - MIDI jack pin 2 connected to ground
  - MIDI jack pin 4 connected to +5V through 220 ohm resistor
  - Attach a MIDI cable to the jack, then to a MIDI synth, and play music.

  created 13 Jun 2006
  modified 13 Aug 2012
  by Tom Igoe

  This example code is in the public domain.

  https://www.arduino.cc/en/Tutorial/BuiltInExamples/Midi
*/

#include "user.h" 


#define RXD2 16
#define TXD2 17

void setup() {

  // Set MIDI baud rate on Serial 2
  Serial2.begin(31250, SERIAL_8N1, RXD2, TXD2);

  Serial.begin(115200);
  Serial.write("midish4esp32 first Version\r\n");

  playHello();

  unsigned exitcode = user_mainloop();
  Serial.write("midishEsp32 exit.\r\n");
  while(true);
}

#define NOTE_C1 0x3C
#define NOTE_D1 0x3E
#define NOTE_E1 0x40
#define NOTE_F1 0x41
#define NOTE_G1 0x43
#define NOTE_A1 0x45
#define NOTE_H1 0x47
#define NOTE_C2 0x48

void playHello(){
  play(NOTE_C1);
  play(NOTE_D1);
  play(NOTE_E1);
  play(NOTE_F1);
  play(NOTE_G1);
}

void play(int note){
    noteOn(0x90, note, 0x45);
    delay(100);
    noteOn(0x90, note, 0x00);
}

void loop() {
  // play notes from F#-0 (0x1E) to F#-5 (0x5A):
  for (int note = 0x1E; note < 0x5A; note++) {
    //Note on channel 1 (0x90), some note value (note), middle velocity (0x45):
    noteOn(0x90, note, 0x45);
    delay(100);
    //Note on channel 1 (0x90), some note value (note), silent velocity (0x00):
    noteOn(0x90, note, 0x00);
    delay(100);
  }
}

// plays a MIDI note. Doesn't check to see that cmd is greater than 127, or that
// data values are less than 127:
void noteOn(int cmd, int pitch, int velocity) {
  Serial2.write(cmd);
  Serial2.write(pitch);
  Serial2.write(velocity);
}
