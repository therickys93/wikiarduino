#include <SPI.h>
#include <Ethernet.h>
#include <redisClient.h>
#include "version.h"

#define BUFFER_LENGTH   100
#define NUMBER_OF_LED   8
#define LED_START_INDEX 2

byte arduino_mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte arduino_ip[]  = {192, 168, 15, 210};

IPAddress server(192, 168, 15, 12);
RedisClient client(server);

char key[10] = "arduino";
char buffer[BUFFER_LENGTH];
bool redis_internet_ok = true;
  
void setup() {
  Ethernet.begin(arduino_mac, arduino_ip);
  Serial.begin(57600);
  delay(1000);
  Serial.print("WikiArduino in ascolto @ ");
  Serial.println(Ethernet.localIP());

  for(int index = LED_START_INDEX; index < (LED_START_INDEX + NUMBER_OF_LED); index++){
    pinMode(index, OUTPUT);
  }
  
  Serial.println("Pronto");
  if(client.connect()){
    Serial.println("connect() -- ok");
  } else {
    Serial.println("connect() -- FAILED");
    redis_internet_ok = false;
  }
  if(redis_internet_ok){
   Serial.println("After connect()");
    if(client.GET(key)){
     Serial.println("GET('arduino') -- ok");
   } else {
     Serial.println("GET('arduino') -- FAILED");  
    }
    Serial.println("After GET('arduino')");
    // the problem is at the line below!!!
    uint16_t result = client.resultBulk(buffer, BUFFER_LENGTH);
    Serial.println(result);
    Serial.println("After resultBulk(buffer, BUFFER_LENGTH)");
    Serial.println(buffer);
  } else {
    Serial.println("Start without internet and redis connection");
  }
}

void loop() {
  if(redis_internet_ok){
    delay(500);
    if(client.GET(key)){
      Serial.println("GET() -- ok");
    } else {
      Serial.println("GET() -- FAILED");  
    }
    client.resultBulk(buffer, BUFFER_LENGTH);
    Serial.println(buffer);
    if(strlen(buffer) > 0){
      if(strlen(buffer) == NUMBER_OF_LED){
        for(int index = LED_START_INDEX; index < (LED_START_INDEX + NUMBER_OF_LED); index++){
          char value = buffer[index - LED_START_INDEX];
          if(value == '0'){
            digitalWrite(index, LOW);
          } else if(value == '1'){
            digitalWrite(index, HIGH);
          } else if(value == '2'){
            digitalWrite(index, LOW);
            delay(100);
            digitalWrite(index, HIGH);
            delay(100);
            digitalWrite(index, LOW);
            buffer[index - LED_START_INDEX] = '0';
            client.startSET(key);
            client.sendArg(buffer);
            client.endSET();
          } else {
            Serial.println("no right value");
          }
        }
      } else {
        Serial.println("string not equals to led");
      }
    } else {
      Serial.println("no value given");
    }
  }
  if (Serial.available() > 0) {
    int led;
    Serial.readBytesUntil(13, buffer, BUFFER_LENGTH);
    Serial.print("ricevuto: ");
    Serial.println(buffer);
    switch(buffer[0]){
      case 'h':
        Serial.println("Lista dei comandi: ");
        Serial.println("h      --> mosta questo messaggio.");
        Serial.println("a<arg> --> accende led in posizione <arg>.");
        Serial.println("s<arg> --> spegne led in posizione <arg>.");
        Serial.println("o<arg> --> apre/chiude led in posizione <arg>.");
        Serial.println("v      --> mostra la versione del codice.");
        break;
      case 'v':
        Serial.print("WikiArduino: (");
        Serial.print(MAJOR, DEC);
        Serial.print(".");
        Serial.print(MINOR, DEC);
        Serial.print(".");
        Serial.print(PATCH, DEC);
        Serial.print("-g");
        Serial.print(COMMIT, HEX);
        Serial.println(")");
        break;
      case 'a':
        if(buffer[1] - 48 >= 2 && buffer[1] - 48 <= 9){
          Serial.print("accendi led: ");
          Serial.println(buffer[1] - 48, DEC);
          digitalWrite(buffer[1] - 48, HIGH);
        }
        break;
      case 's':
        if(buffer[1] - 48 >= 2 && buffer[1] - 48 <= 9){
          Serial.print("spegni led: ");
          Serial.println(buffer[1] - 48, DEC);
          digitalWrite(buffer[1] - 48, LOW);
        }
        break;
      case 'o':
        if(buffer[1] - 48 >= 2 && buffer[1] - 48 <= 9){
          Serial.println("apri/chiudi led: ");
          Serial.print(buffer[1] - 48, DEC);
          digitalWrite(buffer[1] - 48, LOW);
          delay(100);
          digitalWrite(buffer[1] - 48, HIGH);
          delay(100);
          digitalWrite(buffer[1] - 48, LOW);
        }
        break; 
      default:
        break;
    }
  }
}
