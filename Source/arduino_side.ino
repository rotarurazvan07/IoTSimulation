#include<SoftwareSerial.h>
SoftwareSerial SUART(2, 3);
/*
Reset: Pin 4 is the reset pin, which can be used to restart the 555’s
timing operation. Like the trigger input, reset is an active low input.
Thus, pin 4 must be connected to the supply voltage for the 555 timer
to operate. If pin 4 is momentarily grounded, the 555 timer’s operation
is interrupted and won’t start again until it’s triggered again via pin 2.*/
#define buzzer_enable 8

void setup()
{
  pinMode(buzzer_enable,OUTPUT);
  digitalWrite(buzzer_enable,LOW);
  Serial.begin(115200); //enable Serial Monitor
  SUART.begin(115200); //enable SUART Port
  
}
float temperature;
float humidity;

void loop()
{
  /* Read serial data and update the global temp and hum variables */
  if (SUART.available() != 0){
    digitalWrite(buzzer_enable,HIGH);
    String message = SUART.readString();
    Serial.println(message);
    if(message != "\n"){
      update_data(message);
 
      Serial.print("Got temp:");
      Serial.print(temperature);
      Serial.print("\tGot hum:");
      Serial.println(humidity);
    }
  }
  else
    digitalWrite(buzzer_enable,LOW);
}

void update_data(String message){
  float auxTemp, auxHum;
  auxTemp = (message.substring(message.lastIndexOf("<") + 1,message.indexOf(","))).toFloat();  
  auxHum = (message.substring(message.indexOf(",") + 1,message.indexOf(">"))).toFloat();
  
  if(auxTemp < -20 || auxTemp > 60 || auxTemp == 0.00){
    Serial.println("Got invalid temperature");
  }
  else
    temperature = auxTemp;
    
  if(auxHum <= 0 || auxHum >= 100 || auxHum == 0.00){
    Serial.println("Got invalid humidity");
  }
  else
    humidity = auxHum;
}
