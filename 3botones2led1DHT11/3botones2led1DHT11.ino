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
const int BOTON1 = 13; // Manual. Activa la refrigeración al ser presionado
const int BOTON2 = 14; // Alta demanda
const int BOTON3 = 15; // Sobre carga de funcionamient
const int LED1 = 2;    //  Refrigeración Manual
const int LED2 = 4;    // Refrigeración Automática

// Variables
int dato;              // Estado lógico BOTON1
int dato2, dato3;      // Estado lógico BOTON2 Y BOTON3
double timeStart, timeFinish;

// Definición de objetos
  #define DHTPIN 12    // Pin 12 se conecta el DHT11
  #define DHTTYPE DHT11   // DHT 11
  DHT dht(DHTPIN, DHTTYPE);

// Condiciones iniciales - Se ejecuta sólo una vez al energizar
void setup() {// Inicio de void setup ()
  Serial.begin (115200);  // Se inicia comunicación serial a 115200 baudios
  Serial.println("Conectado... ");
  dht.begin();
  pinMode (BOTON1, INPUT_PULLUP); // Configurar el pin 13 BOTON1 como entrada con lógica inversa (PULLUP)
  pinMode (BOTON2, INPUT_PULLUP); // Configurar el pin 14 BOTON1 como entrada con lógica inversa (PULLUP)
  pinMode (BOTON3, INPUT_PULLUP); // Configurar el pin 15 BOTON1 como entrada con lógica inversa (PULLUP)
  pinMode (LED1, OUTPUT);  // Configurar pin 2 LED1 como salida
  pinMode (LED2, OUTPUT);  // Configurar pin 4 LED2 como salida
  digitalWrite (LED1, LOW);
  digitalWrite (LED2, LOW);
  timeStart = millis() +2000;  // Se asigna a timeStart el tiempo actual en milisegundos y se le suman 2000 milisegundos
  
}// Fin de void setup

// Cuerpo del programa - Se ejecuta constamente
void loop() {// Inicio de void loop
  //delay(2000);
  
  float t = dht.readTemperature(); // Read temperature as Celsius (the default)

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  timeFinish = millis(); // tiempo actual en milisegundos
  if (timeFinish >= timeStart){  // esta condición se cunple cada 2000 milisegundos
    Serial.print(F("%  Temperatura: "));
    Serial.print(t);
    Serial.println(F("°C "));
    timeStart = millis() +2000; // una vez que imprime temperatura se actualiza nuestro setpoint a los siguientes 20000 milisegundos
  }
  dato = digitalRead(BOTON1); // se guarda en dato el estado lógico del BOTON1 (recuerda que si no est presionado te envía un 1 lógico)
  //digitalWrite (LED1, !dato); 
  dato2 = digitalRead(BOTON2); // se guarda en dato2 el estado lógico del BOTON2 (recuerda que si no est presionado te envía un 1 lógico)
  dato3 = digitalRead (BOTON3); // se guarda en dato3 el estado lógico del BOTON3 (recuerda que si no est presionado te envía un 1 lógico)
  
  //Cuando debe de encender la refrigeración manual
  // (t > 28.5 && (!dato2 || !dato3))
  //Si temperatura es alta y cualquiera de (alta demanda o sobrecarga) se enciende refrigeración manual
  // o 
  // (|| !dato) si se presiona el boton de manual
  if ((t > 28.5 && (!dato2 || !dato3)) || !dato) {
    digitalWrite (LED1, 1);
    }
  else{
    digitalWrite (LED1, 0);
    }
  
  //digitalWrite (LED2, !dato2);
  // Cuando debe de encender Refrigeración automática
  // t > 28.5  con temperatura alta
  // o 
  // ((!dato2 || !dato3) && dato)
  // Si se tiene alta demanda o sobrecarga y que no este presionado BOTON1 
  // de manual por la prioridad
  if (t > 28.5 || ((!dato2 || !dato3) && dato)) {
    digitalWrite (LED2, 1);
    }
  else{
    digitalWrite (LED2, 0);
    }  
  
}// Fin de void loop

// Funcioes del usuario
