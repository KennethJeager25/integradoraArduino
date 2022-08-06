//LIBRERIAS//
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"

//#define WLAN_SSID "INFINITUM4381_2.4"
//#define WLAN_PASS ""

#define WLAN_SSID       "No c"//red wifi
#define WLAN_PASS       "willyuntornado"//contraseña wifi

/*** Adafruit.io Setup *****/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "ErenJeager"
#define AIO_KEY         "aio_HMvT56czGthWZeTnL3Q9jnZ1YC1b"
#define AIO_GROUP       ""

/** Global State (you don't need to change this!) **/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/**** Feeds *****/

//Adafruit_MQTT_Publish potValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/potValue");

Adafruit_MQTT_Publish distValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"inte_distancia");
Adafruit_MQTT_Publish distPuerta = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"distancia_puerta");
Adafruit_MQTT_Publish movimientoValue = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"inte_movimiento");

// Setup a feed called 'ledBrightness' for subscribing to changes.

Adafruit_MQTT_Subscribe AlarmaBrightness = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/"AIO_GROUP"led");

/*** Sketch Code ****/

void MQTT_connect();

//SALIDA//
uint8_t ledPin = D4;
uint8_t led2 = D3;
const int Trigger = D2;   //Pin digital 2 para el Trigger del sensor
const int Echo = D1;   //Pin digital 3 para el Echo del sensor
const int Trigger2 = D5;   //Pin digital 2 para el Trigger del sensor
const int Echo2 = D6; //Pin digital 3 para el Echo del sensor
const int PIR = D7;
const int buzzer = D8;

//VARIABLES//
uint16_t potAdcValue = 0;
uint16_t distVarValue = 0;
uint16_t distVarValue2 = 0;
uint16_t ledBrightValue = 0;
uint16_t contador=0;
long tiempoUltimaLectura=0;
char mov[50];
char puerta[50];
String mensaje="MOVIMIENTO";
String men="PUERTA ABIERTA";
float interruptor=0;


void setup() {
  Serial.begin(9600);
  delay(10);

  //CONFIGRACION DE COMPONENTES//
  pinMode(Trigger, OUTPUT); //pin como salida
  pinMode(Echo, INPUT);  //pin como entrada
  digitalWrite(Trigger, LOW);//Inicializamos el pin con 0
  pinMode(Trigger2, OUTPUT); //pin como salida
  pinMode(Echo2, INPUT);  //pin como entrada
  digitalWrite(Trigger2, LOW);//Inicializamos el pin con 0
  pinMode(ledPin,OUTPUT);
  digitalWrite(ledPin,HIGH);
  pinMode(led2,OUTPUT);
  digitalWrite(led2,LOW);
 
  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(ledPin,HIGH);
    delay(200);
    Serial.print(".");
    digitalWrite(ledPin,LOW);
    delay(300);
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); 
  Serial.println(WiFi.localIP());

  mqtt.subscribe(&AlarmaBrightness);
  
  }


//METODO VOID//

void loop() {

  MQTT_connect();
  
   Adafruit_MQTT_Subscribe *subscription;
          while ((subscription = mqtt.readSubscription(200))) {
            //if (subscription == &AlarmaBrightness) {
            Serial.print(F("Got LED Brightness : "));
            ledBrightValue = atoi((char *)AlarmaBrightness.lastread);
            Serial.println((char *)AlarmaBrightness.lastread);
            //analogWrite(ledPin, ledBrightValue);
              if (strcmp((char *)AlarmaBrightness.lastread, "ON") == 0) {  
                    
               interruptor=1;

              Serial.println("ON");
              Serial.println(strcmp((char *)AlarmaBrightness.lastread, "ON"));
              delay(2000);
        
            }//FIN IF BOTON "ON"
            else if (strcmp((char *)AlarmaBrightness.lastread, "OFF") == 0) {
              interruptor=0;
              digitalWrite(led2, LOW); 
              Serial.println("OFF");
              Serial.println(strcmp((char *)AlarmaBrightness.lastread, "OFF"));
              delay(2000);
              }//FIN DEL BOTON OFF//
        
      }//FIN DEL CICLO WHILE
            
    //}

    //ENVIO DE DATOS//

  if(millis()-tiempoUltimaLectura>1000){

          distVarValue=distancia();
          distVarValue2=distancia2();
          mensaje.toCharArray(mov,50);
          men.toCharArray(puerta,50);

          if(distVarValue>0&&distVarValue<=10)
          {
              Serial.println(F("OK! MOVIMIENTO"));
                   Serial.println("MOVIMIENTO");
                  if (! movimientoValue.publish(mov)) {
                      Serial.println(F("Failed movimiento"));
                  }else {
                      Serial.println(F("OK! movimiento"));
                      Serial.print("movimiento:");
                      Serial.println(mensaje);
               }//FIN DE LECTURA DE MOVIMIENTO//
               if (! distValue.publish(distVarValue)) {
                  Serial.println(F("Failed Distancia"));
                } else {
                  Serial.println(F("OK! Distancia"));
                  Serial.print("distVarValue:");
                  Serial.println(distVarValue);
               }//FIN DE LA CONDICION DE LEER DISTANCIA//
                digitalWrite(ledPin,HIGH);  
          }//FIN DE LA CONDICION DE DISTANCIA//
          else
          {
                digitalWrite(ledPin,LOW);
          }//FIN DE LA CONDICION DE DISTANCIA SALIDA//

          if(distVarValue2>0 && distVarValue2<=22 && interruptor==1)
            {
                 if (! distPuerta.publish(puerta)) {
                     Serial.println(F("Failed Distancia"));
                 }else {
                      Serial.println(F("OK! Distancia"));
                      Serial.print("distVarValue:");
                      Serial.println(puerta);
                 }        
                      digitalWrite(led2, HIGH);
             }
           else if(interruptor==0)
           {
            digitalWrite(led2, LOW);
           }


    contador=0;
    tiempoUltimaLectura=millis();
  }
  delay(100);
}


//METODO DE CONEXION//

void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}



//CALCULAR DISTANCIA//

long distancia(){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger, LOW);
  t = pulseIn(Echo, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  return d;  
}

//CALCULAR DISTANCIA PUERTA//
long distancia2(){
  long t; //timepo que demora en llegar el eco
  long d; //distancia en centimetros

  digitalWrite(Trigger2, HIGH);
  delayMicroseconds(10);          //Enviamos un pulso de 10us
  digitalWrite(Trigger2, LOW);
  t = pulseIn(Echo2, HIGH); //obtenemos el ancho del pulso
  d = t/59;             //escalamos el tiempo a una distancia en cm
  return d;  
}
