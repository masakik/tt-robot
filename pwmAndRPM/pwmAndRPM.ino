//https://forum.arduino.cc/index.php?topic=382482.0

//fan speed sensor wire attached to digital pin 2 with a 10kohm pullup resistor
//fan PWM control wire attached directly to digital pin 9

#include <PWM.h> //include PWM library http://forum.arduino.cc/index.php?topic=117425.0

volatile int half_revolutions; //allow half_revolutioins to be accesed in intterupt
int rpm;                       //set rpmhttps://forum.arduino.cc/index.php?topic=382482.0 as an integer

void setup()
{
  InitTimersSafe();                             //not sure what this is for, but I think i need it for PWM control?
  bool success = SetPinFrequencySafe(9, 25000); //set frequency to 25kHz
  pwmWrite(9, 51);                              // 51=20% duty cycle, 255=100% duty cycle

  pinMode(2, INPUT); //set RPM pin to digital input
  half_revolutions = 0;
  rpm = 0;

  Serial.begin(9600);
}

void loop()
{
  sei();                                                      //enable intterupts
  attachInterrupt(digitalPinToInterrupt(2), fan_rpm, RISING); //record pulses as they rise
  delay(10000);
  detachInterrupt(digitalPinToInterrupt(2));
  cli(); //disable intterupts

  rpm = (half_revolutions / 2) * 6;

  Serial.print("SPEED: ");
  Serial.print(rpm);
  Serial.println("rpm");
  Serial.println();

  rpm = 0;
  half_revolutions = 0;
}

void fan_rpm()
{
  ++half_revolutions; //increment before returning value
}
