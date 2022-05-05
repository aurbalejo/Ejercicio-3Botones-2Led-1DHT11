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
#include <WiFi.h>  // Biblioteca para el control de WiFi
#include <PubSubClient.h> //Biblioteca para conexion MQTT

const char* ssid = "IoT";  // Aquí debes poner el nombre de tu red
const char* password = "cursoiot";  // Aquí debes poner la contraseña de tu red

//Datos del broker MQTT
const char* mqtt_server = "35.157.221.58"; // Si estas en una red local, coloca la IP asignada, en caso contrario, coloca la IP publica
IPAddress server(35,157,221,58);

// Objetos
WiFiClient espClient; // Este objeto maneja los datos de conexion WiFi
PubSubClient client(espClient); // Este objeto maneja los datos de conexion al broker


// Constantes
const int pinBOTON1 = 13; // Manual. Activa la refrigeración al ser presionado
const int pinBOTON2 = 14; // Alta demanda
const int pinBOTON3 = 15; // Sobre carga de funcionamient
const int LED1 = 2;    //  Refrigeración Manual
const int LED2 = 4;    // Refrigeración Automática
const double temAlta = 30.5;  // set point de temperatura Alta
const String TopicTx = "CodigoIoT/SIC/Flow6/Botones";
const String TopicRx = "CodigoIoT/SIC/Flow6/Temperatura";

// Variables
bool EstadoBoton1;              // Estado lógico BOTON1
bool EstadoBoton2, EstadoBoton3;      // Estado lógico BOTON2 Y BOTON3
bool BtnWeb1, BtnWeb2, BtnWeb3;

double TiempoActual, TiempoObjetivo;
float t=0;
long timeNow, timeLast; // Variables de control de tiempo no bloqueante
int data = 0; // Contador
int wait = 2000;  // Indica la espera cada 5 segundos para envío de mensajes MQTT


// Definición de objetos
  #define DHTPIN 12    // Pin 12 se conecta el DHT11
  #define DHTTYPE DHT11   // DHT 11
  DHT dht(DHTPIN, DHTTYPE);

// Condiciones iniciales - Se ejecuta sólo una vez al energizar
void setup() {// Inicio de void setup ()
  Serial.begin (115200);  // Se inicia comunicación serial a 115200 baudios
  Serial.print("Conectar a ");
  Serial.println(ssid);
  
  dht.begin();
  pinMode (pinBOTON1, INPUT_PULLUP); // Configurar el pin 13 BOTON1 como entrada con lógica inversa (PULLUP)
  pinMode (pinBOTON2, INPUT_PULLUP); // Configurar el pin 14 BOTON1 como entrada con lógica inversa (PULLUP)
  pinMode (pinBOTON3, INPUT_PULLUP); // Configurar el pin 15 BOTON1 como entrada con lógica inversa (PULLUP)
  pinMode (LED1, OUTPUT);  // Configurar pin 2 LED1 como salida
  pinMode (LED2, OUTPUT);  // Configurar pin 4 LED2 como salida
  digitalWrite (LED1, LOW);
  digitalWrite (LED2, LOW);
  TiempoObjetivo = millis() + 2000;  // Se asigna al setpoint el tiempo actual en milisegundos y se le suman 2000 milisegundos

  WiFi.begin(ssid, password); // Esta es la función que realiz la conexión a WiFi
 
  while (WiFi.status() != WL_CONNECTED) { // Este bucle espera a que se realice la conexión
    Serial.print(".");  // Indicador de progreso
    delay (500);
  }
  // Cuando se haya logrado la conexión, el programa avanzará, por lo tanto, puede informarse lo siguiente
  Serial.println();
  Serial.println("WiFi conectado");
  Serial.println("Direccion IP: ");
  Serial.println(WiFi.localIP());

  // Si se logro la conexión, encender led
  if (WiFi.status () > 0){
  digitalWrite (LED2, LOW);
  }
  
  delay (1000); // Esta espera es solo una formalidad antes de iniciar la comunicación con el broker

  // Conexión con el broker MQTT
  client.setServer(server, 1883); // Conectarse a la IP del broker en el puerto indicado
  client.setCallback(callback); // Activar función de CallBack, permite recibir mensajes MQTT y ejecutar funciones a partir de ellos
  delay(1500);  // Esta espera es preventiva, espera a la conexión para no perder información

  timeLast = millis (); // Inicia el control de tiempo
  
}// Fin de void setup

void loop() {// Inicio de void loop

  TiempoActual = millis(); // tiempo actual en milisegundos
  
  if (TiempoActual >= TiempoObjetivo){  // esta condición se cunple cada 2000 milisegundos
    lecturaSensor();
  }

  lecturaBotones();
  Logica();
//********************************************
   //Verificar siempre que haya conexión al broker
  if (!client.connected()) {
    reconnect();  // En caso de que no haya conexión, ejecutar la función de reconexión, definida despues del void setup ()
  }
  client.loop(); // Esta función es muy importante, ejecuta de manera no bloqueante las funciones necesarias para la comunicación con el broker
  
//  timeNow = millis(); // Control de tiempo para esperas no bloqueantes
//  if (timeNow - timeLast > wait) { // Manda un mensaje por MQTT cada 2 segundos
//    timeLast = timeNow; // Actualización de seguimiento de tiempo
//    char dataString[8]; // Define una arreglo de caracteres para enviarlos por MQTT, especifica la longitud del mensaje en 8 caracteres
//    dtostrf(t, 2, 2, dataString);  // Esta es una función nativa de leguaje AVR que convierte un arreglo de caracteres en una variable String
//    Serial.print("Temperatura: "); // Se imprime en monitor solo para poder visualizar que el evento sucede
//    Serial.println(dataString);
//    client.publish("CodigoIoT/SIC/Flow6/Temperatura", dataString); // Esta es la función que envía los datos por MQTT, especifica el tema y el valor
//  } 

//*************************************************
}// Fin de void loop

// Funcioes del usuario

void lecturaSensor(){
  t = dht.readTemperature(); // Read temperature as Celsius (the default)
  // Check if any reads failed and exit early (to try again).
  if (isnan(t)) {
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }
  
  Serial.print(F("%  Temperatura: "));
  Serial.print(t);
  Serial.println(F("°C "));
  TiempoObjetivo = millis() +2000; // una vez que imprime temperatura se actualiza nuestro setpoint a los siguientes 20000 milisegundos
  char dataString[8]; // Define una arreglo de caracteres para enviarlos por MQTT, especifica la longitud del mensaje en 8 caracteres    
  dtostrf(t, 2, 2, dataString);  // Esta es una función nativa de leguaje AVR que convierte un arreglo de caracteres en una variable String
  client.publish("CodigoIoT/SIC/Flow6/Temperatura", dataString);
}

void lecturaBotones(){

  EstadoBoton1 = !digitalRead(pinBOTON1); // se guarda en dato el estado lógico del BOTON1 (recuerda que si no est presionado te envía un 1 lógico)
  EstadoBoton2 = !digitalRead(pinBOTON2); // se guarda en dato2 el estado lógico del BOTON2 (recuerda que si no est presionado te envía un 1 lógico)
  EstadoBoton3 = !digitalRead (pinBOTON3); // se guarda en dato3 el estado lógico del BOTON3 (recuerda que si no est presionado te envía un 1 lógico)
   
}

void Logica(){

    //Cuando debe de encender la refrigeración manual
  
  if (BtnWeb1 || (t > temAlta && (EstadoBoton2 || EstadoBoton3)) || EstadoBoton1) {
    digitalWrite (LED1, 1);
    }
  else{
    digitalWrite (LED1, 0);
    }
  
  // Cuando debe de encender Refrigeración automática
  
  if (BtnWeb2 || BtnWeb3 || t > temAlta || ((EstadoBoton2 || EstadoBoton3) && !EstadoBoton1)) {
    digitalWrite (LED2, 1);
    }
  else{
    digitalWrite (LED2, 0);
    }
}


// Esta función permite tomar acciones en caso de que se reciba un mensaje correspondiente a un tema al cual se hará una suscripción
void callback(char* topic, byte* message, unsigned int length) {

  // Concatenar los mensajes recibidos para conformarlos como una varialbe String
  String messageTemp; // Se declara la variable en la cual se generará el mensaje completo  
  for (int i = 0; i < length; i++) {  // Se imprime y concatena el mensaje
    //Serial.print((char)message[i]);
    messageTemp += (char)message[i];
  }

  // El ESP323CAM está suscrito al tema 
  if (String(topic) == TopicTx) {  // En caso de recibirse mensaje en el tema 
    if(messageTemp == "btn1on"){
     // Serial.println("Led encendido");
      BtnWeb1=1;
    }
    else if(messageTemp == "btn1off"){
      //Serial.println("Led apagado");
      BtnWeb1=0;
    }

    else if(messageTemp == "btn2on"){
      //Serial.println("Led apagado");
      BtnWeb2=1;
    }
    else if(messageTemp == "btn2off"){
      //Serial.println("Led apagado");
      BtnWeb2=0;  
    }
    else if(messageTemp == "btn3on"){
      //Serial.println("Led apagado");
      BtnWeb3=1;
    }
    else if(messageTemp == "btn3off"){
      //Serial.println("Led apagado");
      BtnWeb3=0;  
    }
  }
}// fin del void callback

// Función para reconectarse
void reconnect() {
  // Bucle hasta lograr conexión
  while (!client.connected()) { // Pregunta si hay conexión
    Serial.print("Tratando de contectarse...");
    // Intentar reconexión
    if (client.connect("ESP32CAMClient")) { //Pregunta por el resultado del intento de conexión
      Serial.println("Conectado");
      client.subscribe("CodigoIoT/SIC/Flow6/Botones"); // Esta función realiza la suscripción al tema
    }// fin del  if (client.connect("ESP32CAMClient"))
    else {  //en caso de que la conexión no se logre
      Serial.print("Conexion fallida, Error rc=");
      Serial.print(client.state()); // Muestra el codigo de error
      Serial.println(" Volviendo a intentar en 5 segundos");
      // Espera de 5 segundos bloqueante
      delay(5000);
      Serial.println (client.connected ()); // Muestra estatus de conexión
    }// fin del else
  }// fin del bucle while (!client.connected())
}// fin de void reconnect(
