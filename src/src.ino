#include <SPI.h>
#include <Ethernet.h>
#include <redisClient.h>

#define BUFFER_LENGTH   100
#define NUMBER_OF_LED   8
#define LED_START_INDEX 2

byte arduino_mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte arduino_ip[]  = {192, 168, 15, 210};

IPAddress server(192, 168, 15, 12);
RedisClient client(server);

char key[10] = "arduino";
char buffer[BUFFER_LENGTH];
  
void setup() {
  Ethernet.begin(arduino_mac, arduino_ip);
  Serial.begin(57600);
  delay(1000);
  Serial.print("WikiArduino partito @ ");
  Serial.println(Ethernet.localIP());

  for(int index = LED_START_INDEX; index < (LED_START_INDEX + NUMBER_OF_LED); index++){
    pinMode(index, OUTPUT);
  }
  
  Serial.println("Pronto");
  if(client.connect()){
    Serial.println("connect() -- ok");
  } else {
    Serial.println("connect() -- FAILED");
  }
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
}

void loop() {
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
          delay(500);
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
