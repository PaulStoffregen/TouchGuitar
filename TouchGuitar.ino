#include <Audio.h>
#include <Wire.h>
#include <SD.h>
#include <SPI.h>
#include <SerialFlash.h>

#include "chords.h"

// Special thanks to Matthew Rahtz - http://amid.fish/karplus-strong/

AudioSynthKarplusStrong  string1;
AudioSynthKarplusStrong  string2;
AudioSynthKarplusStrong  string3;
AudioSynthKarplusStrong  string4;
AudioSynthKarplusStrong  string5;
AudioSynthKarplusStrong  string6;
AudioMixer4              mixer1;
AudioMixer4              mixer2;
AudioOutputAnalog        dac1;
AudioConnection          patchCord1(string1, 0, mixer1, 0);
AudioConnection          patchCord2(string2, 0, mixer1, 1);
AudioConnection          patchCord3(string3, 0, mixer1, 2);
AudioConnection          patchCord4(string4, 0, mixer1, 3);
AudioConnection          patchCord5(mixer1, 0, mixer2, 0);
AudioConnection          patchCord6(string5, 0, mixer2, 1);
AudioConnection          patchCord7(string6, 0, mixer2, 2);
AudioConnection          patchCord8(mixer2, 0, dac1, 0);


const int finger_delay = 5;
const int hand_delay = 220;

int chordnum=0;

void setup() {
  AudioMemory(15);

  dac1.analogReference(INTERNAL); // TODO: switch: normal vs loud
  //dac1.analogReference(EXTERNAL);
  mixer1.gain(0, 0.25);
  mixer1.gain(1, 0.25);
  mixer1.gain(2, 0.25);
  mixer1.gain(3, 0.25);
  mixer2.gain(1, 0.25);
  mixer2.gain(2, 0.25);
  delay(700);
}

void strum_up(const float *chord, float velocity);
void strum_dn(const float *chord, float velocity);

bool touched[12];
int avg[12];
int startup=64;

#define VARIANCE_TOUCH    200
#define VARIANCE_UNTOUCH   90

void loop()
{
	int s[12];
	int i, diff, variance;

	s[0] = touchRead(15) << 10;
	s[1] = touchRead(16) << 10;
	s[2] = touchRead(17) << 10;
	s[3] = touchRead(18) << 10;
	s[4] = touchRead(19) << 10;
	s[5] = touchRead(22) << 10;
	s[6] = touchRead(23) << 10;
	s[7] = touchRead(0) << 10;
	s[8] = touchRead(1) << 10;
	s[9] = touchRead(25) << 10;
	s[10] = touchRead(33) << 10;
	s[11] = touchRead(32) << 10;

	if (startup) {
		for (i=0; i < 12; i++) {
			avg[i] += s[i];
		}
		if (--startup == 0) {
			avg[i] /= 64;	
		}
	}
	for (i=0; i < 12; i++) {
		diff = s[i] - avg[i];
		variance = diff / (avg[i] >> 10);
		//Serial.printf("%6d ", variance);
		if (touched[i] == false) {
			if (variance >= VARIANCE_TOUCH) {
				touched[i] = true;
				touch_event(i);
			}
		} else {
			if (variance <= VARIANCE_UNTOUCH) {
				touched[i] = false;
				untouch_event(i);
			}
		}
		if (diff > 0) {
			avg[i] += diff >> 13; // increase average slowly
		} else {
			avg[i] += diff >> 6;  // decrease average rapidly
		}
	}

	//Serial.printf("%8d  %8d", s[0], avg[0]);
	//Serial.println();

	return;
}

void touch_event(int num)
{
	const float *chord;

	Serial.printf("Touch %d\n", num);
	if (num > 5) return;

	if (touched[11]) {
		chord = Amajor;
	} else if (touched[10]) {
		chord = Gmajor;
	} else if (touched[9]) {
		chord = Fmajor;
	} else if (touched[8]) {
		chord = Emajor;
	} else if (touched[7]) {
		chord = Dmajor;
	} else if (touched[6]) {
		chord = Cmajor;
	} else {
		chord = nochord;
	}

	if (num == 0) {
		if (chord[0] > 20.0) string1.noteOn(chord[0], 1.0);
	} else if (num == 1) {
		if (chord[1] > 20.0) string2.noteOn(chord[1], 1.0);
	} else if (num == 2) {
		if (chord[2] > 20.0) string3.noteOn(chord[2], 1.0);
	} else if (num == 3) {
		if (chord[3] > 20.0) string4.noteOn(chord[3], 1.0);
	} else if (num == 4) {
		if (chord[4] > 20.0) string5.noteOn(chord[4], 1.0);
	} else if (num == 5) {
		if (chord[5] > 20.0) string6.noteOn(chord[5], 1.0);
	}
}

void untouch_event(int num)
{
	//Serial.printf("Release %d\n", num);
}




