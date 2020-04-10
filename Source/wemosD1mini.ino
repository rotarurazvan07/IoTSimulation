#include <DHTesp.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

DHTesp dht;
const char *ssid = "UPCB3837A7";
const char *pass = "6yeevzekpsBf";
const char *url = "https://api.thingspeak.com";
const char *url_ip = "http://52.86.196.73";
const char *key = "P7JOMGTS11RKHGHM";
float tempAvg = 0;
float humAvg = 0;
int reading = 0;
volatile int seconds = 0;
boolean read_temp_tick = false;
volatile int upload_interval = 0;

void sensor_init(){
  dht.setup(D5, DHTesp::DHT11);
  Serial.println();
  Serial.println("Humidity\t\tTemperature");
}
void timer_init(){
  noInterrupts();
  timer1_attachInterrupt(ISR_tick);
  timer1_enable(TIM_DIV16, TIM_EDGE, TIM_LOOP);
  timer1_write(5000000);
  interrupts();
}
void wifi_init(){
  delay(1000);
  WiFi.mode(WIFI_OFF);
  delay(1000);
  WiFi.mode(WIFI_STA);

  WiFi.begin(ssid, pass);
  Serial.print("Connecting");
  while(WiFi.status() != WL_CONNECTED){
    delay(500);
    Serial.print(".");  
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
}
void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  wifi_init();
  sensor_init();
  timer_init();
}

void read_temp(){
  delay(dht.getMinimumSamplingPeriod());
  
  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  
  tempAvg += temperature;
  humAvg += humidity;
  reading++;
}

void loop() {
  if(read_temp_tick){
    read_temp();
    read_temp_tick = false;  
  }
  
  if(upload_interval >= 30){

    if(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);
      while(WiFi.status() != WL_CONNECTED){
        delay(500);
      }
    }

    
    Serial.print(humAvg/reading);
    Serial.print("\t\t");
    Serial.println(tempAvg/reading);
    send_data(tempAvg/reading,humAvg/reading);
    humAvg = 0;
    tempAvg = 0;
    reading = 0;
    upload_interval = 0;
  }
  
}

void send_data(float temp, float hum){
  HTTPClient http;

  String query = String(url_ip) + "/update?api_key=" + String(key)
               + "&field1=" + String(temp) + "&field2=" + String(hum);
  Serial.println(query);
  http.begin(query);
  Serial.print(http.GET());
  Serial.print("\t");
  Serial.println(http.getString());
  http.end();
}

void ISR_tick(){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    if(++seconds >= 10){
        upload_interval++;
        seconds = 0;
        read_temp_tick = true;
    }
}
