/**
  Sound.h - Library for reproduce sounds on the table tennis robot.
  Created by Masakik on 01/2021
*/

#ifndef Sound_h
#define Sound_h
#include "Arduino.h"

// 5a oitava
#define SOUND_DO 528
#define SOUND_RE 592
#define SOUND_MI 665
#define SOUND_FA 705
#define SOUND_SOL 791
#define SOUND_LA 888
#define SOUND_SI 997

class Sound
{
  public:
    Sound(int pin);
    void play(int note, int duration);
    void play(int note);
    void pause(void);

  private:
    int _pin;
};

#endif
