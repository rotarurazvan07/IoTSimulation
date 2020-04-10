#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <SoftwareSerial.h>

SoftwareSerial SUART(4,5);
const char *ssid = "UPCB3837A7";
const char *pass = "6yeevzekpsBf";
const char *url = "https://api.thingspeak.com";
const char *url_ip = "http://52.86.196.73";
const char *key = "P7JOMGTS11RKHGHM";

volatile int seconds = 0;
volatile int download_interval = 0;

float temperature;
float humidity;
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
  SUART.begin(115200);

  
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  wifi_init();
  timer_init();

}

void get_data(){
    Serial.println("Getting data");
    HTTPClient http;

    String query = String(url_ip) +"/channels/844317/field/1/last.json";
    Serial.println(query);
    http.begin(query);
    Serial.print(http.GET());
    Serial.print("\t");
    String response = http.getString();
    Serial.println(response);
    http.end();

    temperature = (response.substring(response.lastIndexOf(":")+2,response.lastIndexOf("\""))).toFloat();

    query = String(url_ip) +"/channels/844317/field/2/last.json";
    Serial.println(query);
    http.begin(query);
    Serial.print(http.GET());
    Serial.print("\t");
    response = http.getString();
    Serial.println(response);
    http.end();

    humidity = (response.substring(response.lastIndexOf(":")+2,response.lastIndexOf("\""))).toFloat();
}

void loop() {
  if(download_interval >= 30){
    if(WiFi.status() != WL_CONNECTED){
      WiFi.begin(ssid, pass);
      while(WiFi.status() != WL_CONNECTED){
        delay(500);
      }
    }
    get_data();

    Serial.println(temperature);
    Serial.println(humidity);

    String message = "<<<" + String(temperature) + "," + String(humidity) + ">>>";
    SUART.println(message);
    download_interval = 0;
  }
}

void ISR_tick(){
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    if(++seconds >= 10){
        download_interval++;
        seconds = 0;
    }
}
