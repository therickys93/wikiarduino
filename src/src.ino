#include <Ethernet.h>
#include <redisClient.h>
#include <Utilities.h>
#include "version.h"

#define BUFFER_LENGTH   100
#define NUMBER_OF_LED   8
#define LED_START_INDEX PIN_2
#define LED_STOP_INDEX  PIN_9
#define SENSORS_NUMBER  6

byte arduino_mac_address[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte arduino_ip_address[]  = {192, 168, 15, 210};

IPAddress redis_server(192, 168, 15, 12);
RedisClient redis_client(redis_server);

char digital_key[10] = "arduino";
char a0_key[15] = "";
char a1_key[15] = "";
char a2_key[15] = "";
char a3_key[15] = "";
char a4_key[15] = "";
char a5_key[15] = "";

char keys[SENSORS_NUMBER] = {a0_key, a1_key, a2_key, a3_key, a4_key, a5_key};

char redis_buffer[BUFFER_LENGTH];
bool redis_internet_ok = true;

static int timeout() {
  int c;
  int _startMillis = millis();
    do {
        c = Serial.read();
        if(c >= 0)
            return c;
        yield();
    } while(millis() - _startMillis < 1000);
    return -1;
}

static String readSerialString(char terminator) {
    String ret;
    int c = timeout();
    Serial.print((char)c);
    while(c >= 0 && c != terminator) {
        ret += (char) c;
        c = timeout();
        Serial.print((char)c);
    }
    return ret;
}

void setup() {
  Ethernet.begin(arduino_mac_address, arduino_ip_address);
  Serial.begin(57600);
  delay(1000);
  Serial.setTimeout(5000);
  Serial.print("WikiArduino in ascolto @ ");
  Serial.println(Ethernet.localIP());

  for(int index = LED_START_INDEX; index < (LED_START_INDEX + NUMBER_OF_LED); index++){
    pinOutput(index);
  }

  sprintf(a0_key, "%s_a0", digital_key);
  sprintf(a1_key, "%s_a1", digital_key);
  sprintf(a2_key, "%s_a2", digital_key);
  sprintf(a3_key, "%s_a3", digital_key);
  sprintf(a4_key, "%s_a4", digital_key);
  sprintf(a5_key, "%s_a5", digital_key);
  
  Serial.println("Pronto");
  if(redis_client.connect()){
    Serial.println("connect() -- ok");
  } else {
    Serial.println("connect() -- errore");
    redis_internet_ok = false;
  }
  if(redis_internet_ok){
   Serial.println("dopo connect()");
    if(redis_client.GET(digital_key)){
     Serial.println("GET(chiave) -- ok");
   } else {
     Serial.println("GET(chiave) -- errore");
     redis_internet_ok = false;
    }
    // the problem is at the line below!!!
    uint16_t result = redis_client.resultBulk(redis_buffer, BUFFER_LENGTH);
    Serial.println(result);
    Serial.println(redis_buffer);
  } else {
    Serial.println("Partito senza una connessione internet");
  }
  Serial.print("WikiArduino console: ");
  Serial.print(MAJOR, DEC);
  Serial.print(".");
  Serial.print(MINOR, DEC);
  Serial.print(".");
  Serial.print(PATCH, DEC);
  Serial.print("-g");
  Serial.println(COMMIT, HEX);
  Serial.println("Invia comando 'h' per una lista di comandi.");
  Serial.print("> ");
}

void loop() {
  if(redis_internet_ok){
    delay(500);
    if(redis_client.GET(digital_key)){
      Serial.println("GET() -- ok");
      redis_internet_ok = true;
    } else {
      Serial.println("GET() -- errore");
      redis_internet_ok = false; 
    }
    redis_client.resultBulk(redis_buffer, BUFFER_LENGTH);
    Serial.println(redis_buffer);
    if(strlen(redis_buffer) > 0){
      if(strlen(redis_buffer) == NUMBER_OF_LED){
        for(int index = LED_START_INDEX; index < (LED_START_INDEX + NUMBER_OF_LED); index++){
          char value = redis_buffer[index - LED_START_INDEX];
          if(value == '0'){
            pinLow(index);
          } else if(value == '1'){
            pinHigh(index);
          } else if(value == '2'){
            pinLow(index);
            delay(100);
            pinHigh(index);
            delay(100);
            pinLow(index);
            redis_buffer[index - LED_START_INDEX] = '0';
            redis_client.startSET(digital_key);
            redis_client.sendArg(redis_buffer);
            redis_client.endSET();
          } else {
            Serial.println("valore non conosciuto");
          }
        }
      } else {
        Serial.println("stringa o troppo lunga o troppo corta");
      }
    } else {
      Serial.println("nessun valore ricevuto");
    }
    // send sensors data to redis server
    for(int i = 0; i < SENSORS_NUMBER; i++){
      if(0 == i){
        // from temperature sensor
        int reading = analogRead(i);  
        float voltage = reading * 5.0;
        voltage /= 1024.0;
        sprintf(redis_buffer, "%f", voltage);
      } else {
        sprintf(redis_buffer, "%d", analogRead(i));
      }
      redis_client.startSET(keys[i]);
      redis_client.sendArg(redis_buffer);
      redis_client.endSET();
    }
  }
  if (Serial.available() > 0) {
    int led;
    String serial_buffer = readSerialString(13);
    switch(serial_buffer[0]){
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
        if(serial_buffer[1] - '0' >= LED_START_INDEX && serial_buffer[1] - '0' <= LED_STOP_INDEX){
          Serial.print("accendi led: ");
          Serial.println(serial_buffer[1] - '0', DEC);
          pinHigh(serial_buffer[1] - '0');
        }
        break;
      case 's':
        if(serial_buffer[1] - '0' >= LED_START_INDEX && serial_buffer[1] - '0' <= LED_STOP_INDEX){
          Serial.print("spegni led: ");
          Serial.println(serial_buffer[1] - '0', DEC);
          pinLow(serial_buffer[1] - '0');
        }
        break;
      case 'o':
        if(serial_buffer[1] - '0' >= LED_START_INDEX && serial_buffer[1] - '0' <= LED_STOP_INDEX){
          Serial.print("apri/chiudi led: ");
          Serial.println(serial_buffer[1] - '0', DEC);
          pinLow(serial_buffer[1] - '0');
          delay(100);
          pinHigh(serial_buffer[1] - '0');
          delay(100);
          pinLow(serial_buffer[1] - '0');
        }
        break; 
      default:
        break;
    }
    Serial.print("> ");
  }
}
