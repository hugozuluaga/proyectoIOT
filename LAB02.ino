
#include <SoftwareSerial.h>
#include <DHT.h>

#include <WiFi.h>
#include <Servo.h>

 
// arduino Rx (pin 2) ---- ESP8266 Tx
// arduino Tx (pin 3) ---- ESP8266 Rx
SoftwareSerial esp8266(3, 2);
int sensor = 4;
const char* server = "api.thingspeak.com";
String apiKey = "2WTAQM38FJEAPZUM";  
int ventilador = 9;
int LED_SISTEMA = 10;
float temp, humedad;
float tempMax = 32.30;
String Data = "";
int encendido = 0;
DHT dht (sensor, DHT11);
Servo servomecatronicos;
int lectura;
const char* host = "maker.ifttt.com";
void setup()
{
  
  Serial.begin(9600);

  esp8266.begin(9600);
  dht.begin();
  pinMode(ventilador, OUTPUT);
  pinMode(LED_SISTEMA, OUTPUT);
  pinMode(13, OUTPUT);
  digitalWrite(LED_SISTEMA, LOW);
  digitalWrite(ventilador, LOW);
  digitalWrite(13, LOW); //Led Arduino
Serial.println("LABEL,hora,temperatura,humedad");
  sendData("AT+RST\r\n", 2000);     // rese t módulo
  sendData("AT+CWMODE=1\r\n", 1000); // configurar como cliente
  //sendData("AT+CWJAP=\"FLIA_CUADROS\",\"Give2807\"\r\n", 8000); //SSID y contraseña para unirse a red
  sendData("AT+CWJAP=\"UNISANGIL YO\",\"12345678\"\r\n", 8000); //SSID y contraseña para unirse a red
  sendData("AT+CIFSR\r\n", 1000);   // obtener dirección IP
  sendData("AT+CIPMUX=1\r\n", 1000); // configurar para multiples conexiones
  sendData("AT+CIPSERVER=1,80\r\n", 1000);        // servidor en el puerto 5050
servomecatronicos.attach(5);

}



void loop()
{
  if (esp8266.available())  // revisar si hay mensaje del ESP8266
  {
    if (esp8266.find("+IPD,")) // revisar si el servidor recibio datos
    {
      delay(1500); // esperar que lleguen los datos hacia el buffer
      int conexionID = esp8266.read() - 48; // obtener el ID de la conexión para poder responder
      Serial.print("***");
      while (esp8266.available() > 0)
      {
        char character = esp8266.read();
        Serial.print(character);
        Data.concat(character);
        if (character == '\n')
        {
          Serial.print("Received: ");
          Serial.println(Data);
          delay(50);
          if (Data.indexOf("PRENDERSISTEMA") > 0) {
            //ENCENDER LED SISTEMA


            
            Serial.print("SISTEMA ENCENDIDO");
            encendido = 1;
            digitalWrite(LED_SISTEMA, HIGH); // Cambiar estado del
            // digitalWrite(13, HIGH); // Encender led Arduino
             servomecatronicos.write(120);
               WiFiClient client; 
           const int httpPort = 80;  
            if (!client.connect(host, httpPort)) {  
                  Serial.println("connection failed");  
            return;} 
          if(Serial.available()){
            
            char a=Serial.read();
            
            if (a=='y'){
    
          
                    String url = "/trigger/ALARMA_TEMPERATURA/with/key/cZdboGYc6nLoAjf53LdI785mkG4Zvd9CHWdNfWdWCGC"; 

          
                    Serial.print("Requesting URL: ");
                    Serial.println(url);
                 
                     client.print(String("GET ") + url + " HTTP/1.1\r\n" + 
                                    "Host: " + host + "\r\n" +   
                                           "Connection: close\r\n\r\n");    
                                    }  
          
          
          else{
            Serial.println("Correct character not pressed.Try again");
            
          }
          }
          
          delay(1000);
             
          }
          

          if (Data.indexOf("APAGARSISTEMA") > 0) {
            Serial.print("SISTEMA APAGADO");
            digitalWrite(LED_SISTEMA, LOW); // Cambiar estado del
            //digitalWrite(13, LOW); // Cambiar estado del
            encendido = 0;
          }

          if (Data.indexOf("TEMPERATURA") > 0) {
            //esp8266.find("GET")
            String sub = Data.substring(22, 24);
            Serial.println(sub);
            Serial.println("----");
            tempMax = sub.toInt();
            Serial.print(tempMax) ;
          }
          // Clear receive buffer so we're ready to receive the next line
          Data = "";
        }
      }
      Serial.print("***");
      // comando para terminar conexión
  /*    String comandoCerrar = "AT+CIPCLOSE=";
      comandoCerrar += conexionID;
      comandoCerrar += "\r\n";
      esp8266.print(comandoCerrar); // enviar el comando al ESP8266*/
    }
  }

  if (encendido == 1) {
    //TEMP
     
    humedad = dht.readHumidity();
    temp = dht.readTemperature();
    Serial.print("Temperatura: ");
    Serial.print(temp);
    Serial.print("ºC Humedad: ");
    Serial.print(humedad);
    Serial.println("%");
    lectura = analogRead(A0);
 Serial.print("DATA, TIME,");
 Serial.print(temp);
 Serial.print(",");
        Serial.println(humedad);
    delay(1000);
    

    // Comprobamos si ha habido algún error en la lectura
    if (isnan(humedad) || isnan(temp) ) {
      Serial.println("Error obteniendo los datos del sensor DHT11");
      return;
    }



    if (temp >= tempMax) {

  servomecatronicos.write(20);
      digitalWrite(ventilador, HIGH); // Cambiar estado del
      //digitalWrite (ventilador, HIGH);
      Serial.print("Ventilador activo. Temperatura: ");
      
      Serial.print(temp);
      Serial.print(" ºC ");
      delay(1000);
    }
    else {
      digitalWrite (ventilador, LOW);
       servomecatronicos.write(120);
    }
  }

}

/*
  Enviar comando al esp8266 y verificar la respuesta del módulo, todo esto dentro del tiempo timeout
*/


void sendData(String comando, const int timeout)
{
  long int time = millis(); // medir el tiempo actual para verificar timeout
  esp8266.print(comando); // enviar el comando al ESP8266
  while ( (time + timeout) > millis()) //mientras no haya timeout
  {
    while (esp8266.available()) //mientras haya datos por leer
    {
      // Leer los datos disponibles
      char c = esp8266.read(); // leer el siguiente caracter
      Serial.print(c);
    }
    
  }
  return;
  
}
