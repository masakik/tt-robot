// Include the library
#include "FanController.h"
#include "AccelStepper.h"
#include "Cmd.h"
#include "timer.h"
#include "timerManager.h"

// Sensor de bola com modulo infravermelho ****************
unsigned int ball_freq = 60; // bolas por minuto

#define BALL_DEBOUNCE_DELAY 250 // the debounce time (ms)
#define BALL_PIN 6              // o sensor envia LOW se detectou bola, então a lógica é invertida

unsigned int ball_interval = 60000 / ball_freq;
unsigned long ball_prev_time = millis();
unsigned long ball_current_time;
bool ball_prev_state = 0;
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
#define FEEDER_PAUSE_PIN 8     // pino do botão de pausar o ballFeeder
#define PAUSE_BTN_DEBOUNCE 500 // debounce do botão de pause (ms)
unsigned long pause_btn_prev_time = millis();
#define SPEED 200 // velocidade nominal de rotação (em pps)

AccelStepper feeder(AccelStepper::DRIVER, stepPin, dirPin);

// timers *************************************************
Timer timerPrintStatus;

// Auxiliar para Serial.print
char buffer[100];

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
  cmdAdd("freq", cmdFreq);
  cmdAdd("pause", cmdFeederPause);
  cmdAdd("cont", cmdFeederCont);
  cmdAdd("status", cmdStatus);
  cmdAdd("top", cmdTopSpin);
  cmdAdd("under", cmdUnderSpin);
  //cmdAdd("elev", cmdElevation);
  //cmdAdd("azim", cmdAzimute);

  // timers
  timerPrintStatus.setInterval(10000);
  timerPrintStatus.setCallback(cmdStatus);

  TimerManager::instance().start();
}

// Main function ***************************
void loop(void)
{
  cmdPoll();
  TimerManager::instance().update();
  feeder.runSpeed();
  pollBallSensor();
  ballFeedEach(ball_interval);
  poolPauseBtn();
}

void cmdHelp(int arg_cnt, char **args)
{
  Serial.println("status");
  Serial.println("pause");
  Serial.println("cont");
  Serial.println("freq 15..90 (bolas por minuto)");
  Serial.println("top 0..100 (%)");
  Serial.println("under 0..100 (%)");

  // futuros
  Serial.println("elev -30..30 (graus)");
  Serial.println("azim -30..30 (graus)");
}

void cmdFreq(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    ball_freq = max(min(cmdStr2Num(args[1], 10), 90), 20); // min=20, max=90
    ball_interval = 60000 / ball_freq;
  }
  Serial.print("freq ");
  Serial.println(ball_freq);
}

void cmdTopSpin(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    top.setDutyCycle(max(min(cmdStr2Num(args[1], 10), 100), 0));
  }
  sprintf(buffer, "Top %i%%", top.getDutyCycle());
  Serial.println(buffer);
}

void cmdUnderSpin(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    under.setDutyCycle(max(min(cmdStr2Num(args[1], 10), 100), 0));
  }
  sprintf(buffer, "Under %i%%", under.getDutyCycle());
  Serial.println(buffer);
}

void cmdFeederPause(int arg_cnt, char **args)
{
  ball_soft_pause = true;
}

void cmdFeederCont(int arg_cnt, char **args)
{
  ball_soft_pause = false;
}

void cmdStatus(int arg_cnt, char **args)
{
  sprintf(buffer, "run: %i; count %i; freq %i; top %i%%-%iRPM; under %i%%-%iRPM",
          !ball_soft_pause, ball_counter, ball_freq,
          top.getDutyCycle(), top.getSpeed(),
          under.getDutyCycle(), under.getSpeed()
         );
  Serial.println(buffer);
}

void cmdStatus(){
  cmdStatus(0,0);
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
      ball_prev_state = HIGH;
      ball_current_time = millis();
      ball_counter++;
      Serial.println(ball_counter);
    }
    else
    {
      ball_prev_state = LOW;
    }
  }
}

/**
   Verifica se o botão de pause foi pressionado e muda o estado de pausa
   Como ele é pull_up, compara com LOW
*/
void poolPauseBtn()
{
  if ((digitalRead(FEEDER_PAUSE_PIN) == LOW) && (millis() - pause_btn_prev_time > PAUSE_BTN_DEBOUNCE))
  {
    ball_soft_pause = !ball_soft_pause;
    pause_btn_prev_time = millis();
    Serial.println("Botão de pausa pressionado");
  }
}

/**
   Lança bola a cada intervalo de tempo com a opção de pausa
*/
void ballFeedEach(int ball_interval)
{
  // vamos verificar o intervalo de lançamento && soft pause
  if (((millis() - ball_prev_time) > ball_interval) && !ball_soft_pause)
  {
    // se passou intervalo gira
    feeder.setSpeed(SPEED);
    ball_prev_time = ball_current_time;
  }
  else if (millis() - ball_prev_time > BALL_DEBOUNCE_DELAY)
  {
    // depois de começar a girar vamos aguardar um pouco antes de parar
    // assim a bola se afasta do sensor evitando ler a mesma bola
    feeder.setSpeed(0);
  }
}
