/**
  Sound.cpp - Library for reproduce sounds on the table tennis robot.
  Created by Masakik on 01/2021
*/
#include "Sound.h"
#include "Arduino.h"

Sound::Sound(int pin)
{
  pinMode(pin, OUTPUT);
  _pin = pin;
}

void Sound::play(int note, int duration = 500)
{
  tone(_pin, note, duration);
}

void Sound::play(int note) {
  Sound::play(note, 500);
}

void Sound::pause() {
  play(SOUND_LA, 150);
}
