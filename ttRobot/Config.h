
// Servos *************************************************
#define TOP_PIN 9
#define UNDER_PIN 10
#define AZIMUTE_PIN 11
#define ELEVATION_PIN 12

// Sensor de bola com modulo infravermelho ****************
#define BALL_FREQ_MIN 15
#define BALL_FREQ_MAX 90

#define BALL_SENSOR_PIN A1       // o sensor envia LOW se detectou bola, então a lógica é invertida
#define BALL_SENSOR_DEBOUNCE 200 // debounce do sensor de bola (ms)

// motor de passo ****************************************
#define STEP_PIN 2
#define DIR_PIN 3
#define FEEDER_DELAY 250 // continua girando por mais um tempo (ms)
#define SPEED 200 // velocidade nominal de rotação (em pps)

// Botão do buzzer para Sound ****************************
#define BUZZER_PIN A3

// Botão de pausa do feeder ******************************
#define FEEDER_PAUSE_PIN A0     // pino do botão de pausar o ballFeeder
#define FEEDER_PAUSE_DEBOUNCE 500 // debounce do botão de pause (ms)

// Vcc monitor *******************************************
#define VCC_MONITOR_PIN A2
#define VCC_ADJ 1.247
