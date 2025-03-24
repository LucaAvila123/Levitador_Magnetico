//Programa: Conectando Sensor Ultrassonico HC-SR04 ao Arduino
//Autor: MakerHero

//Carrega a biblioteca do sensor ultrassonico
#include <Ultrasonic.h>

/*** Carrega as bibliotecas e definições para as funções da interface gráfica no InfluxDB ***/
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
// WiFi AP SSID
#define WIFI_SSID "SSID"
// WiFi password
#define WIFI_PASSWORD "PASSWORD"
// URL do influxdb
#define INFLUXDB_URL "http://localhost:8086"
#define INFLUXDB_TOKEN "WAHU2NA34uJI9rWKQEiNPUT-yomPtcNGGr8IDzG-s_7aYWgzYC3RbHIev5RdluRGNpMC__hNuMSkdVlyUXDJLQ=="
#define INFLUXDB_ORG "myorg"
#define INFLUXDB_BUCKET "Levitador Magnetico"
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
Point results("Control variables");
/*** Terminadas as definições ***/

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
  /*** Valores setados da interface gráfica ***/
  // Setup wifi
  WiFi.mode(WIFI_STA);
  wifiMulti.addAP(WIFI_SSID, WIFI_PASSWORD);

  Serial.print("Connecting to wifi");
  while (wifiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(100);
  }
  Serial.println();

  // Add tags
  results.addTag("Tag1", "Kp");
  results.addTag("Tag2", "Kd");
  results.addTag("Tag3", "erro");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  /*** Terminado de setar os valores da interface gráfica ***/

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

  // limpar o buffer da parte da interface gráfica
  // results.clearFields();

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
  if (!client.writePoint(Kd)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  Serial.print(" Kp: ");
  Serial.println(Kp);
  if (!client.writePoint(Kp)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  //OCR2B = analogRead(A0) / 4;
  erroPrev = erro;

  if (!client.writePoint(erro)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }
  delay(50);
}