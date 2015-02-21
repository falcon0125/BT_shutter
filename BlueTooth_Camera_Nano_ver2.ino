#include <SoftwareSerial.h>

int LED_status = 0;
SoftwareSerial BT(11, 12);
// Define PINs
const byte KEY_PIN = 4;
const byte SHUTTER_PIN = A5;//A6
const byte LIGHT_PIN = 13;//A6
// Define shutter times
long shutter_start_time = 0;
//
int shutter_status = 0;
int mode_timelapse = 1;
int shutter_duration = 1;
int shutter_interval = 0;
int counter = 0;
//
int *config_list[5];

//innitializing buffer
String buffer ="";

// parse


// setup
void setup() {
  // initialize both serial ports:
  Serial.begin(9600);
  BT.begin(9600);
  pinMode(SHUTTER_PIN,OUTPUT);
  pinMode(LIGHT_PIN,OUTPUT);
  // FOR THE KEY of BLUETOOTH
  //  pinMode(KEY_PIN,OUTPUT);
  //  digitalWrite(KEY_PIN,HIGH);
config_list[0] = &shutter_status;
config_list[1] = &mode_timelapse;
config_list[2] = &shutter_duration;
config_list[3] = &shutter_interval;
config_list[4] = &counter;
}
// parse mesage
/*
1. shutter_status
 2. mode_timelapse
 3. shutter_speed              / in second /
 4. shutter_interval           / in second /
 5. counter                    /int
 @START#
 @STOP#
 */
void parse(String buffer){
  for (int i=0 ; i< 5 ; i++){
    int loc = buffer.indexOf(",");
    if( loc != -1){
      String temp = buffer.substring(0,loc);
      Serial.println(temp);
      Serial.println("->");
      *config_list[i] = temp.toInt();
      buffer = buffer.substring(loc+1);
    }
    else{
      *config_list[i] = buffer.toInt();
    }
  }
}

void event_handler(String msg){
  if (msg.startsWith("@")) { 
    msg.replace("@","");
    if(msg == "start"){
      shutter_status = 1;
      shutter_start_time=millis();
    }
    else if (msg == "stop"){
      shutter_status = 0;
    }
  }
  else{
    parse(msg); 
  }
}



void shutter_control(){
  if ( shutter_status == 0){
    digitalWrite(SHUTTER_PIN,LOW);
    digitalWrite(LIGHT_PIN, LOW);
    return;
  }
  else{
    if(mode_timelapse != 0 && millis() >= shutter_start_time + shutter_interval*1000) {
      shutter_start_time = shutter_start_time + shutter_interval *1000;
    }
    if( shutter_start_time <= millis() && millis() < shutter_start_time +shutter_duration * 1000 ){ 
      digitalWrite(SHUTTER_PIN, HIGH);  
      digitalWrite(LIGHT_PIN, HIGH);  
    }
    if( millis() >= shutter_start_time + shutter_duration * 1000){ 
      digitalWrite(SHUTTER_PIN, LOW);
      digitalWrite(LIGHT_PIN, LOW);  
    }
  }
}
void loop() {
  // Listen from bluetooth
  while(BT.available()){
    int inByte = BT.read();
    if (inByte != '#'){
      buffer += char(inByte);
    }
    else if(inByte =='#'){
      event_handler(buffer);
      Serial.println(buffer);
      buffer ="";
      Serial.println(shutter_status);
      Serial.println(mode_timelapse);
      Serial.println(shutter_duration);
      Serial.println(shutter_interval);
      Serial.println(counter);
      Serial.println("-------------------------------");
    }
  }
  // controll shutter
  shutter_control();
  /*
  Serial.println("shutter_duration = " + shutter_duration );
   Serial.println("shutter_interval = " + shutter_interval );
   Serial.println("----------------------------------------");
   */


}


void toggleLED(){
  if (LED_status == 0){
    LED_status = 1;
    digitalWrite(13,HIGH);
  }
  else if (LED_status == 1){
    LED_status = 0;
    digitalWrite(13,LOW);
  }
}







