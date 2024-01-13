// Import_libraries
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <BlynkSimpleEsp8266.h>

// Defines_pinout
#define pin_buzzer    D5
#define pin_ldr       A0
#define pin_trigger   D1
#define pin_echo      D2
#define led_builtIn   D4
#define led_red       D0        
#define led_yellow    D3        
#define VIN           3.3
#define R             10000
// Blynk_configuration
#define BLYNK_TEMPLATE_ID   "TMPxxxxxx"             // Input_yours
#define BLYNK_TEMPLATE_NAME "Device"                // Input_yours
#define BLYNK_AUTH_TOKEN    "YourAuthToken"         // Input_your_blynk_authorize_token

// Calling_method
BlynkTimer timer;

// Wifi_configuration
char ssid[] = "wifi_ssid";                          // Input_your_wifi_ssid
char pass[] = "wifi_pass";                          // Input_yout_wifi_password

// Initialize_variable_value
const int ultrasonic_threshold = 50;    // Set_HC-SR04_threshold_depend_on_your_cases
const int ldr_threshold = 220;          // Set_LDR_threshold_depend_on_your_cases
int lux_iluminance = 0;
int ldr_value = 0;
int distance = 0;
long duration = 0.00;

String mode;
String status;

void setup() {
  Serial.begin(115200);
  // Initialize_Pinout
  pinMode(pin_trigger, OUTPUT);
  pinMode(pin_buzzer, OUTPUT);
  pinMode(pin_echo, INPUT);
  pinMode(led_builtIn, OUTPUT);
  pinMode(led_yellow, OUTPUT);
  pinMode(led_red, OUTPUT); 
  // Initialize_Wifi
  Serial.print("Connecting to SSID = ");
  Serial.println(ssid);
  // Setup_blynk
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 80);
  timer.setInterval(100L, sendData);
}

void loop() {
  Blynk.run();
  readLight();
  readDistance();
  
  if(distance < ultrasonic_threshold)         digitalWrite(pin_buzzer, HIGH);
  else if(distance >= ultrasonic_threshold)   digitalWrite(pin_buzzer, LOW);
  
  if(lux_iluminance < ldr_threshold){
    digitalWrite(led_red, HIGH);
    digitalWrite(led_yellow, HIGH);
  }else if(lux_iluminance >= ldr_threshold){
    digitalWrite(led_red, LOW);
    digitalWrite(led_yellow, LOW);
  } 

  timer.run();

  if(Blynk.connected()) digitalWrite(led_builtIn, LOW);
  else                  digitalWrite(led_builtIn, HIGH);

  // Serial_Display_LDR
  // Serial.print("Iluminance Value\t= ");
  // Serial.print(lux_iluminance);
  // Serial.print(" Lux\t\t");
  // Serial_Display_HC-SR04
  // Serial.print("Distance\t= ");
  // Serial.print(distance);
  // Serial.println(" cm");
}

// Light_Dependent_Resistor_sensor [LDR]
int readLight(){
  ldr_value = analogRead(pin_ldr);
  float Vout = float(ldr_value)*(VIN/float(1023));    // Conversion_analog_to_voltage
  float RLDR = (R*(VIN-Vout))/Vout;                   // Conversion_voltage_to_resistance
  lux_iluminance = 500/(RLDR/1000);                   // Conversion_resitance_to_lumen 

  return lux_iluminance;
}

// Ultrasonic_rensor [HC-SR04]
int readDistance(){
  delayMicroseconds(2);
  digitalWrite(pin_trigger, HIGH);
  delayMicroseconds(10);
  digitalWrite(pin_trigger, LOW);
  delayMicroseconds(2);
  duration = pulseIn(pin_echo, HIGH);  
  distance = (duration*0.034)/2;

  return distance;
}

// Send_data_to_blynk
void sendData(){
  Blynk.virtualWrite(V0, lux_iluminance);   // V0 : Virtual Pin in Blynk, Check your blynk setup
  Blynk.virtualWrite(V1, distance);         // V1 : Virtual Pin in Blynk, Check your blynk setup
  
  if(lux_iluminance >= ldr_threshold){
    mode = "Lamp OFF";
    Blynk.virtualWrite(V2, mode);
  }else if(lux_iluminance < ldr_threshold){
    mode = "Lamp ON";
    Blynk.virtualWrite(V2, mode);
  }

  if(distance >= ultrasonic_threshold){
    status = "Status : Object Not Detected [clear]";
    Blynk.virtualWrite(V3, status);
  }else if(distance < ultrasonic_threshold){
    status = "Status : Object Detected";
    Blynk.virtualWrite(V3, status);
  }
}