#define BLYNK_TEMPLATE_ID "TMPL6nSa-73Q7"
#define BLYNK_TEMPLATE_NAME "Tongkat Tunanetra"
#define BLYNK_AUTH_TOKEN "W21wg1eHNBwTmFYWbHSlI4exF0wl7O7b"

// #include <WiFi.h>
// #include <Blynk.h> 
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <BlynkSimpleEsp8266.h>
#include <ESP8266WiFi.h>

// WiFi credentials
char ssid[] = "Mantq";
char pass[] = "12345678";

// NTP setup
WiFiUDP udp;
NTPClient timeClient(udp, "pool.ntp.org", 7 * 3600, 60000); // Offset UTC +7 (Yogyakarta) and update every 60 seconds

// Pin setup
int buttonPin = 10;    
int buttonState = 0;   
int lastButtonState = 0; 
bool isOn = false;
int buzzerPin = 15;
const int trigpin = 14;     
const int echopin = 12;     

unsigned long duration;     
float distance;             

void setup() {
  Serial.begin(115200);
  
  // Connecting to Wi-Fi and Blynk
  Serial.println("Connecting to Blynk...");
  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("WiFi connected!"); 
  } else {
    Serial.println("WiFi connection failed!");
  }

  // Initialize NTP client
  timeClient.begin();

  // Wait until the NTP time is updated
  while (!timeClient.update()) {
    Serial.println("Menunggu waktu dari NTP...");
    delay(1000);  // Wait for 1 second and try again
  }

  // Setup pin modes
  pinMode(buttonPin, INPUT);  
  pinMode(buzzerPin, OUTPUT); 
  pinMode(trigpin, OUTPUT);
  pinMode(echopin, INPUT);
  
  // Initial log to Blynk
  handleLog();

  Serial.println("Blynk connected!");
}

void handleLog() {
  String message = "Aktif";
  
  // Get current time from NTP
  String currentTime = timeClient.getFormattedTime();

  Serial.println("Terminal: " + currentTime);

  // Send status and time to Blynk
  Blynk.virtualWrite(V0, message);
  Blynk.virtualWrite(V1, currentTime);
}

void loop() {
  Blynk.run();  // Run Blynk to maintain connection
  timeClient.update();  // Update NTP time

  // Read button state
  buttonState = digitalRead(buttonPin);

  // Toggle on/off status when the button is pressed
  if (buttonState == HIGH && lastButtonState == LOW) {
    isOn = !isOn;  // Toggle status ON/OFF
    if (isOn) {                
      Serial.println("Status: ON");

      // Buzzer feedback for turning ON
      for (int i = 0; i < 3; i++) {
        tone(buzzerPin, 1000);  
        delay(200);              
        noTone(buzzerPin);    
        delay(200);           
      }        
    } else {
      Serial.println("Status: OFF");

      // Buzzer feedback for turning OFF
      tone(buzzerPin, 1000);      
      delay(1000);                
      noTone(buzzerPin);          
    }
  }

  // Check distance if system is ON
  if (isOn) {
    // Trigger ultrasonic sensor
    digitalWrite(trigpin, LOW);
    delayMicroseconds(2);
    digitalWrite(trigpin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigpin, LOW);

    duration = pulseIn(echopin, HIGH); 
    distance = duration * 0.034 / 2;    // Calculate distance

    Serial.print("Jarak: ");
    Serial.println(distance);

    // If an object is closer, speed up the buzzer tone
    if (distance <= 150) {
      int delayTime = map(distance, 0, 150, 2, 700);  // Map distance to delay time
      tone(buzzerPin, 1250);  
      delay(250);          
      noTone(buzzerPin);      
      delay(delayTime);     
    } else {
      noTone(buzzerPin);      
      delay(500);          
    }

    // Send the current distance and status to Blynk
    String distanceMessage = "Jarak: " + String(distance) + " cm";
  }

  // Store last button state
  lastButtonState = buttonState;

  delay(200);  // Delay to debounce the button
}
