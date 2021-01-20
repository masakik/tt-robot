#ifndef ttRobot_h
#define ttRobot_h

// Included libraries from library manager
#include <AccelStepper.h>
#include <Cmd.h>
#include <timer.h>
#include <timerManager.h>
#include <Servo.h>

// Include de bibliotecas locais
#include "Config.h"
#include "Sound.h"


typedef struct {
  int top, under, elev, azim;
} Head;

typedef struct {
  float vcc_adj;
} Config;


void ttRobotSetup();
void ttRobotLoop();

void cmdHelp(int arg_cnt, char **args);
void cmdProgram(int arg_cnt, char **args);
void cmdFreq(int arg_cnt, char **args);
void cmdFeederPause(int arg_cnt, char **args);
void cmdFeederCont(int arg_cnt, char **args);
void cmdStatus(int arg_cnt, char **args);
void cmdTopSpin(int arg_cnt, char **args);
void cmdUnderSpin(int arg_cnt, char **args);

void cmdStatus(void);
void readVcc();
void pollBallSensor();
void pollPauseBtn();
void pollBallFeed();

#endif
