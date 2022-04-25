/*
 * 3botones 2leds DHT11
 * Por: ARTURO URBALEJO
 * Fecha: 20 de abril de 2022
 * 
 * Características

  *Sensor de temperatura y humedad
  *Boton1: Manual. Activa la refrigeración al ser presionado
  *Boton2: Alta demanda
  *Boton3: Sobre carga de funcionamiento

  *Led1: Refrigeracion manual
  *Led2: Refrigeración automática
  
  *Que se encienda la refrigeracion manual cada que se presione el boton de manual
  
  *La refrigeracion manual tiene prioridad a la refrigeracion automática.
  
  *Si la termperatura es alta (mayor a 28, por ejemplo), se activa la refrigeracion automática
  
  *Si tengo alta demanda o sobre carga, se activa la refrigeración automática

  *Si tengo alta demanda o sobrecarga de funcionamiento y ademas temperatura alta, se activan ambas refrigeraciones

  *Botones 14, 15, 13
  *Leds 4, 2
  *DHT11 12
 */

// Bibliotecas
  #include "DHT.h"

// Constantes
const int BOTON1 = 13;
const int BOTON2 = 14;
const int BOTON3 = 15;
const int LED1 = 2;  
const int LED2 = 4;   

// Variables
int dato;
int dato2, dato3;
double timeStart, timeFinish;

// Definición de objetos
  #define DHTPIN 12
  #define DHTTYPE DHT11   // DHT 11
  DHT dht(DHTPIN, DHTTYPE);

// Condiciones iniciales - Se ejecuta sólo una vez al energizar
void setup() {// Inicio de void setup ()
  Serial.begin (115200);
  Serial.println("Conectado... ");
  dht.begin();
  pinMode (BOTON1, INPUT_PULLUP); // Configurar el pin 4 BOTON como entrada
  pinMode (BOTON2, INPUT_PULLUP);
  pinMode (BOTON3, INPUT_PULLUP);
  pinMode (LED1, OUTPUT);  // Configurar pin 2 LED como salida
  pinMode (LED2, OUTPUT);
  digitalWrite (LED1, LOW);
  digitalWrite (LED2, LOW);
  timeStart = millis() +2000;
  
}// Fin de void setup

// Cuerpo del programa - Se ejecuta constamente
void loop() {// Inicio de void loop
  //delay(2000);
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  timeFinish = millis();
  if (timeFinish >= timeStart){
    Serial.print(F("%  Temperatura: "));
    Serial.print(t);
    Serial.println(F("°C "));
    timeStart = millis() +2000;
  }
  dato = digitalRead(BOTON1);
  //digitalWrite (LED1, !dato);
  dato2 = digitalRead(BOTON2);
  dato3 = digitalRead (BOTON3);

  if ((t > 28.5 && (!dato2 || !dato3)) || !dato) {
    digitalWrite (LED1, 1);
    }
  else{
    digitalWrite (LED1, 0);
    }
  
  //digitalWrite (LED2, !dato2);
  if (t > 28.5 || ((!dato2 || !dato3) && dato)) {
    digitalWrite (LED2, 1);
    }
  else{
    digitalWrite (LED2, 0);
    }  
  
}// Fin de void loop

// Funcioes del usuario
