# tt-robot

## Steps

* Use arduino to control 2 spin motors (pwm fans from computer CPU) to put spin on balls, one for top and other for under spin;
* Read the RPM of motors
* use a stepper motor to feed balls to the spin motors

## TODO

* Programa de treino
* Beeps

## Updates

* Monitorando a voltagem da fonte de alimentação
* Substituido os motores de fan por ESC+BRUSHLESS
* Botão de pausa sem trava
* Sensor de bola com infravermelho
* Alimentador de bola como motor de passo mais potente (4,2kgf.cm) e driver A4988

## Parts

* Arduino nano ATmega328
* Driver A4988 para motor de passo
* Sensor infravermelho
* Motor De Passo Nema 17 4,2 Kgf.cm 12v 17hs4401, eixo 5mm
* Flange para motor de passo (disco inferior)
* 2 servos MG996R
* adaptador de disco plastico do servo: d20.5mm, parafuso soberba que veio junto do servo
* 2 motores A2212/13T, 1000KV
* 2 ESC HW30A
* anel de EVA para o disco

* display ??
* segundo arduino ??
* encoder rotativo ??

## Features

Robo lançador de bolas programável e cesta coletora realimentada. 

Ajustes:
* efeito (top e under spin);
* velocidade do lancamento
* direção (direita/esquerda, cima e baixo)
* frequência (15 a 90 bolas por minuto).

Outras características
* Botão de pausa/continue com debounce (A0)
* Todos os ajustes/controles são realizados pela porta Serial
* Ajustes de posição da cabeça por meio de servos 
* Lancamento de bola com ESC + motor brushless
* Beep de indicação
* Alimentação de 12V, conversor LM7805 para 5V
* Alimentador de bolas por meio de disco girante horizontal com 4 bolas por girando
* Contador de bolas lançadas
* Monitoramento da qualidade da fonte de alimentação (A2)

