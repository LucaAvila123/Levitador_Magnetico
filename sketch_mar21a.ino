//Programa: Conectando Sensor Ultrassonico HC-SR04 ao Arduino
//Autor: MakerHero

//Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>

//Define os pinos para o trigger e echo
#define pino_trigger 4
#define pino_echo 5
#define X0 16.5

//Inicializa o sensor nos pinos definidos acima
Ultrasonic ultrasonic(pino_trigger, pino_echo);
float cmMsec, soma, media, erro, erroPrev = 0;
long microsec;
int controle, Kp = 110, Kd;


void setup()
{
  Serial.begin(9600);
  Serial.println("Lendo dados do sensor...");
  pinMode(3, OUTPUT);
  pinMode(A0, INPUT);
  pinMode(A1, INPUT);
  TCCR2A = (1 << COM2B1) | (1 << WGM21) | (1 << WGM20); // Modo Fast PWM, não-invertido
  TCCR2B = (1 << CS20); // Prescaler de 8
   
   // Define o valor do registrador OCR2B para controlar o duty cycle
  OCR2B = 255;
}

void loop()
{
  Kd = analogRead(A3) / 8;
  Kp = analogRead(A0) / 4;
  soma = 0;
  //Le as informacoes do sensor, em cm e pol
  for(int i = 0;i < 20; i++){
    microsec = ultrasonic.timing();
    cmMsec = ultrasonic.convert(microsec, Ultrasonic::CM);
    soma += cmMsec;
    delay(2);
  } 
  media = soma / 20;

  erro = X0 - media;
  
  controle = (erro * Kp) + ((erroPrev - erro) * Kd);

  OCR2B = 128 + controle;

  //Serial.print("Media de distancia em cm: ");
  Serial.print("Kd: ");
  Serial.print(Kd);
  Serial.print(" Kp: ");
  Serial.println(Kp);
  //OCR2B = analogRead(A0) / 4;
  erroPrev = erro;
  delay(50);
}