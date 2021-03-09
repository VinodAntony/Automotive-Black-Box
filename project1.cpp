#include <AHT10.h>
#include "WiFi.h"
#include <ThingSpeak.h>

#define WIFI_TIMEOUT_MS 20000
#define WIFI_NETWORK "BSNL_AP"
#define WIFI_PASSWORD "qwer8606"

#define CHANNEL_ID 1303618
#define CHANNEL_API_KEY "YRNMHPL1VQ394LHX"

WiFiClient client;

void connectToWifi(){
  Serial.print("Connecting to wifi");
  WiFi.mode(WIFI_STA);//station mode
  WiFi.begin(WIFI_NETWORK,WIFI_PASSWORD);

  unsigned long startAttemptTime = millis();// sets current time in ms since start of exectution

  //keep looping while not connected and haven't reached the timeout
  while(WiFi.status()!=WL_CONNECTED && millis() - startAttemptTime < WIFI_TIMEOUT_MS){
    Serial.print(".");
    delay(100);
  }

  if(WiFi.status() != WL_CONNECTED){
    Serial.println("Failed");
  }
  else{
    Serial.print("Connected");
    Serial.print(WiFi.localIP());
  }
} 
//AHT10 sensor
//ESP32
//      GPIO21/D21 = SDA
//      GPIO22/D22 = SCL
AHT10 myAHT10;
float tempValue = 0;
float humiValue = 0;
//const int SDA = D21;
//const int SCL = D22;
void AHT10_Status(){
   if(myAHT10.begin(SDA,SCL)){
    Serial.println("setup OK");
  }
  else{
    Serial.println("setup error");
  }
}
//for ultrasonic
const int trigPin = 2;
const int echoPin = 5;
long duration;
int distance;
///////////////////

void ULTRASONIC_SETUP(){
  pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
  pinMode(echoPin, INPUT); // Sets the echoPin as an Input
}

void ULTRASONIC_LOOP(){
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  duration = pulseIn(echoPin, HIGH);
  distance= duration*0.034/2;
  Serial.print("Distance: ");
  Serial.println(distance);
}

//sw420
const int vibration = 15;
int vibration_detected;
void VIBRATION_SETUP(){
  pinMode(vibration, INPUT);
}
void VIBRATION_LOOP(){
  vibration_detected = digitalRead(vibration);
  if(vibration_detected)
    Serial.println("VIBRATION DETECTED");
  else
    Serial.println("NO VIBRATION");
}
//mq3
const int alcohol = 4;
int not_toxicated;
void ALCOHOL_SETUP(){
  pinMode(alcohol, INPUT);
}
void ALCOHOL_LOOP(){
  not_toxicated = digitalRead(alcohol); // high if sober, low if drunk
  if(not_toxicated)
    Serial.println("No Alcohol");
  else
    Serial.println("Alcohol detected");
}

#define WIFI_TIMEOUT_MS 20000

//posting time
unsigned long preMillis = 0;
const long interval = 1000; // Ex: 1 minute =  1 * 60 * 1000 = 60000

void AHT10_LOOP(){
   unsigned long currentMillis = millis();
 
  if(currentMillis - preMillis >= interval){
    preMillis = currentMillis;
    tempValue = myAHT10.readTemperature(AHT10_FORCE_READ_DATA);        
    humiValue = myAHT10.readHumidity(AHT10_FORCE_READ_DATA); 
    Serial.print("Temperature: -->");
    Serial.println(tempValue);
    Serial.print("Humidity: --->");
    Serial.println(humiValue);
  }
}
void setup() {
  // put your setup code here, to run once:

  Serial.begin(115200);
  connectToWifi();
  ALCOHOL_SETUP();
  ULTRASONIC_SETUP();
  VIBRATION_SETUP();
  AHT10_Status();
  ThingSpeak.begin(client);
  delay(1000);
}


void loop(){

  ULTRASONIC_LOOP();
  AHT10_LOOP();
  VIBRATION_LOOP();
  ALCOHOL_LOOP();
  ThingSpeak.setField(1,tempValue);
  ThingSpeak.setField(2,humiValue);
  ThingSpeak.setField(3,distance);
  ThingSpeak.setField(4,vibration_detected);
  ThingSpeak.setField(5,not_toxicated);

  if(!not_toxicated || (distance==2 )&&(vibration_detected==1) || tempValue>50)
  ThingSpeak.writeFields(CHANNEL_ID,CHANNEL_API_KEY);
  
  delay(5000);
}