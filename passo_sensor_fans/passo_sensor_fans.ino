// Include the library
#include "FanController.h"
#include "AccelStepper.h"
#include "Cmd.h"
#include "timer.h"
#include "timerManager.h"

// Sensor de bola com modulo infravermelho ****************
unsigned int ball_freq = 60;    // bolas por minuto
#define BALL_DEBOUNCE_DELAY 200 // the debounce time (ms); increase if the output flickers

// o sensor envia LOW se detectou bola, então a lógica é invertida
#define BALL_PIN 6

unsigned int ball_interval = 60000 / ball_freq;
unsigned long ball_prev_time = millis();
unsigned long ball_current_time;
int ball_prev_state = 0;
int ball_counter = 0;
bool ball_soft_pause = false;

// Motores top e under spin ********************************

// Sensor wire is plugged into port 2 on the Arduino.
// For a list of available pins on your board,
// please refer to: https://www.arduino.cc/en/Reference/AttachInterrupt
#define SENSOR_PIN 2
#define SENSOR_PIN2 3

// Choose a threshold in milliseconds between readings.
// A smaller value will give more updated results,
// while a higher value will give more accurate and smooth readings
#define SENSOR_THRESHOLD 1000

// PWM pin (4th on 4 pin fans)
#define TOP_PWM_PIN 9
#define UNDER_PWM_PIN 10

FanController top(SENSOR_PIN, SENSOR_THRESHOLD, TOP_PWM_PIN);
FanController under(SENSOR_PIN2, SENSOR_THRESHOLD, UNDER_PWM_PIN);

// motor de passo ***************************************
#define stepPin 4
#define dirPin 5

// pino do botão de pausar o ballFeeder
#define FEEDER_PAUSE_PIN 8

// velocidade de rotação (em pps)
#define SPEED 200

AccelStepper feeder(AccelStepper::DRIVER, stepPin, dirPin);

// timers *************************************************
// timer do serialprint
unsigned long timer1 = millis();

// timer sem uso por enquanto
Timer timerPrintStatus;
Timer timerBallFeed;

// ********************************************************
void setup(void)
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Fan Controller Library Demo");

  // fan
  top.begin();
  under.begin();

  // Stepper
  feeder.setMaxSpeed(1000.0);
  feeder.setSpeed(SPEED);

  pinMode(FEEDER_PAUSE_PIN, INPUT_PULLUP);

  // sensor de bola
  pinMode(BALL_PIN, INPUT);

  // cmdSerial
  cmdInit(&Serial);
  cmdAdd("help", cmdHelp);
  cmdAdd("freq", cmdUpdateFreq);
  cmdAdd("pause", cmdFeederPause);
  cmdAdd("cont", cmdFeederCont);
  cmdAdd("status", cmdStatus);


  // timer
  timerPrintStatus.setInterval(10000);
  timerPrintStatus.setCallback(printStatus);

  TimerManager::instance().start();
}

/*
   Main function
*/
void loop(void)
{
  cmdPoll();
  TimerManager::instance().update();
  pollBallSensor();
  feeder.runSpeed();
  ballFeedEach(ball_interval);
}

void cmdHelp(int arg_cnt, char **args)
{
  //Stream *s = cmdGetStream();
  Serial.println("freq 20..90");
  Serial.println("status");
  Serial.println("pause");
  Serial.println("cont");
}

void cmdUpdateFreq(int arg_cnt, char **args)
{
  if (arg_cnt > 1) {
    cmdGetStream()->println("Hello world.");
    ball_freq = cmdStr2Num(args[1], 10);
    ball_interval = 60000 / ball_freq;
  }
  Serial.print("freq ");
  Serial.println(ball_freq);
}

void cmdFeederPause(int arg_cnt, char **args)
{
  ball_soft_pause = true;
}

void cmdFeederCont(int arg_cnt, char **args)
{
  ball_soft_pause = false;
}

void cmdStatus(int arg_cnt, char **args) {
  printStatus();
}

void printStatus() {
  Serial.print("run ");
  Serial.println(digitalRead(FEEDER_PAUSE_PIN) && !ball_soft_pause);

  Serial.print("count ");
  Serial.println(ball_counter);

  Serial.print("freq ");
  Serial.println(ball_freq);

  Serial.print("top ");
  Serial.println(top.getDutyCycle());

  Serial.print("under ");
  Serial.println(under.getDutyCycle());
}


/**
   Verifica o sensor de bola se foi lançado ou não
   Como não tem interrupção disponível utilizou-se uma variável de estado.
   O lancamento é na borda de subida.
   Como a lógica é invertida vamos corrigir isso na leitura.
   Incrementa o contador de bolas
   (Utiliza debounce para minimizar repeticoes mas parece estar com problemas)
*/
void pollBallSensor()
{
  // verifica se lancou bola
  if (!digitalRead(BALL_PIN) != ball_prev_state)
  {
    if (ball_prev_state == LOW)
    {
      // vamos fazer o debounce
      if (millis() - ball_current_time > BALL_DEBOUNCE_DELAY)
      {
        ball_prev_state = HIGH;
        ball_current_time = millis();
        ball_counter++;
      }
    }
    else
    {
      ball_prev_state = LOW;
    }
  }
}

/**
   Lança bola a cada intervalo de tempo com a opção de pausa por botão
*/
void ballFeedEach(int ball_interval)
{
  // vamos verificar o botão de pause && o intervalo de lançamento && soft pause
  if (digitalRead(FEEDER_PAUSE_PIN) && ((millis() - ball_prev_time) > ball_interval) && !ball_soft_pause)
  {
    // se passou intervalo gira
    feeder.setSpeed(SPEED);
    ball_prev_time = ball_current_time;
    return;
  }
  if (millis() - ball_prev_time > BALL_DEBOUNCE_DELAY) {
    // senão para de girar
    feeder.setSpeed(0);
  }

}
