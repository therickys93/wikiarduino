#include <SPI.h>
#include <Ethernet.h>
#include <redisClient.h>

#define BUFFER_LENGTH 100
#define NUMBER_OF_LED 2

// led collegati al pin 2 e al 4
int red_pin   = 2;
int green_pin = 4;

byte arduino_mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
byte arduino_ip[]  = {192, 168, 15, 210};

IPAddress server(192, 168, 15, 12);
RedisClient client(server);

char key[10] = "arduino";
char buffer[BUFFER_LENGTH];
  
void setup() {
  Ethernet.begin(arduino_mac, arduino_ip);
  Serial.begin(57600);
  delay(5000);
  Serial.print("WikiArduino partito @ ");
  Serial.println(Ethernet.localIP());

  pinMode(red_pin, OUTPUT);
  pinMode(green_pin, OUTPUT);
  
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
  if(client.GET(key)){
    Serial.println("GET() -- ok");
  } else {
    Serial.println("GET() -- FAILED");  
  }
  client.resultBulk(buffer, BUFFER_LENGTH);
  Serial.println(buffer);
  if(strlen(buffer) > 0){
    if(strlen(buffer) == NUMBER_OF_LED){
      char pin2 = buffer[0];
      char pin4 = buffer[1];
      if(pin2 == '0'){
        digitalWrite(red_pin, LOW); 
      } else if(pin2 == '1'){
        digitalWrite(red_pin, HIGH);
      } else if(pin4 == '0'){
        digitalWrite(green_pin, LOW);
      } else if(pin4 == '1'){
        digitalWrite(green_pin, HIGH);
      } else {
        Serial.println("no right value");
      }
    } else {
      Serial.println("string not equals to led");
    }
  } else {
    Serial.println("no value given");
  }
}
