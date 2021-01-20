#include "ttRobot.h"

// Servos *************************************************
Servo azimute;
Servo elevation;
Servo top;
Servo under;

// Sensor de bola com modulo infravermelho ****************
unsigned int ball_freq = 60; // bolas por minuto

unsigned int ball_interval = 60000 / ball_freq;
unsigned long ball_prev_time = millis();
unsigned long ball_current_time;
bool ball_prev_state = 0;
unsigned int ball_counter = 0;

bool ball_soft_pause = true;

// motor de passo ***************************************
unsigned long pause_btn_prev_time = millis();
AccelStepper feeder(AccelStepper::DRIVER, STEP_PIN, DIR_PIN);

// Botão do buzzer
Sound sound(BUZZER_PIN);

// timers *************************************************
Timer timerPrintStatus;

// Auxiliar para Serial.print
char buffer[100];

// PSU voltage measure
unsigned int vcc_monitor;
//float vcc_adj = 1.247; // vcc_multimetro/valor_lido, depende do divisor de tensão


// Programação ********************************************
Head pgm;
Head ps1_sp3_sn0 = {20, 20, 10, 10};

Config cfg;

// ********************************************************
void ttRobotSetup()
{
  // start serial port
  Serial.begin(9600);
  Serial.println("Table Tennis Robot");

  // Vcc monitor
  cfg.vcc_adj = VCC_ADJ;
  pinMode(VCC_MONITOR_PIN, INPUT);

  // programa
  pgm = ps1_sp3_sn0;

  // servos
  top.attach(TOP_PIN);
  under.attach(UNDER_PIN);
  azimute.attach(AZIMUTE_PIN);
  elevation.attach(ELEVATION_PIN);

  // Stepper
  feeder.setMaxSpeed(1000.0);
  feeder.setSpeed(SPEED);
  pinMode(FEEDER_PAUSE_PIN, INPUT_PULLUP);

  // sensor de bola
  pinMode(BALL_SENSOR_PIN, INPUT);

  // cmdSerial
  cmdInit(&Serial);
  cmdAdd("help", cmdHelp);
  cmdAdd("pgm", cmdProgram);
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

  // Sound
  sound.play(SOUND_MI, 1000);
}

// Main function ***************************
void ttRobotLoop(void)
{
  cmdPoll();
  TimerManager::instance().update();
  feeder.runSpeed();
  pollBallSensor();
  pollPauseBtn();
  pollBallFeed();
}

void cmdHelp(int arg_cnt, char **args)
{
  Serial.println("status");
  Serial.println("pgm 1..9");
  Serial.println("pause");
  Serial.println("cont");
  sprintf(buffer, "freq %i..%i (bolas por minuto)", BALL_FREQ_MIN, BALL_FREQ_MAX);
  Serial.println(buffer);
  Serial.println("top 0..100 (%)");
  Serial.println("under 0..100 (%)");

  // futuros
  Serial.println("elev -30..30 (graus)");
  Serial.println("azim -30..30 (graus)");
}

void cmdProgram(int arg_cnt, char **args)
{
  int pgm = 1;
  if (arg_cnt > 1)
  {
    pgm = cmdStr2Num(args[1], 10);
  }
  Serial.print("pgm ");
  Serial.println(pgm);
}

void cmdFreq(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    ball_freq = max(min(cmdStr2Num(args[1], 10), BALL_FREQ_MAX), BALL_FREQ_MIN); // limita entre BALL_FREQ_MIN e BALL_FREQ_MAX
    ball_interval = 60000 / ball_freq;
  }
  Serial.print("freq ");
  Serial.println(ball_freq);
}

void cmdTopSpin(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    top.write(max(min(cmdStr2Num(args[1], 10), 100), 0));
  }
  sprintf(buffer, "Top %i%%", top.read());
  Serial.println(buffer);
}

void cmdUnderSpin(int arg_cnt, char **args)
{
  if (arg_cnt > 1)
  {
    under.write(max(min(cmdStr2Num(args[1], 10), 100), 0));
  }
  sprintf(buffer, "Under %i%%", under.read());
  Serial.println(buffer);
}

void cmdFeederPause(int arg_cnt, char **args)
{
  ball_soft_pause = true;
  sound.pause();
}

void cmdFeederCont(int arg_cnt, char **args)
{
  ball_soft_pause = false;
  sound.pause();
}

void cmdStatus(int arg_cnt, char **args)
{
  readVcc();
  sprintf(buffer, "run: %i; count %i; freq %i; top %i%%; under %i%%; vcc %i",
          !ball_soft_pause, ball_counter, ball_freq,
          top.read(),
          under.read(),
          vcc_monitor
         );
  Serial.println(buffer);
}

void cmdStatus() {
  cmdStatus(0, 0);
}

void readVcc()
{
  unsigned int sum = 0;
  for (int i = 0; i < 20; i++)
  {
    sum = sum + analogRead(VCC_MONITOR_PIN);
  }
  vcc_monitor = map(sum / 20, 0, 1023, 0, 13000) * cfg.vcc_adj;
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
  if ((!digitalRead(BALL_SENSOR_PIN) != ball_prev_state))
  {
    if ((ball_prev_state == LOW)  & (millis() - ball_current_time > BALL_SENSOR_DEBOUNCE))
    {
      ball_prev_state = HIGH;
      ball_current_time = millis();
      ball_counter++;

      int top_degree = random(0, 100);
      top.write(top_degree);
      under.write(top_degree);
      Serial.println(top_degree);
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
void pollPauseBtn()
{
  if ((digitalRead(FEEDER_PAUSE_PIN) == LOW) && (millis() - pause_btn_prev_time > FEEDER_PAUSE_DEBOUNCE))
  {
    ball_soft_pause = !ball_soft_pause;
    pause_btn_prev_time = millis();
    Serial.println("Botão de pausa pressionado");
    Serial.print("run ");
    Serial.println(!ball_soft_pause);
    sound.pause();
  }
}

/**
   Lança bola a cada intervalo de tempo com a opção de pausa
*/
void pollBallFeed()
{
  // vamos verificar o intervalo de lançamento && soft pause
  if (((millis() - ball_prev_time) > ball_interval) && !ball_soft_pause)
  {
    // se passou intervalo gira
    feeder.setSpeed(SPEED);
    ball_prev_time = ball_current_time;
  }
  else if (millis() - ball_prev_time > FEEDER_DELAY)
  {
    // depois de começar a girar vamos aguardar um pouco antes de parar
    // assim a bola se afasta do sensor evitando ler a mesma bola
    feeder.setSpeed(0);
  }
}
