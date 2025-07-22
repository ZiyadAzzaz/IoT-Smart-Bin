// Smart Trash Bin
#define BLYNK_TEMPLATE_ID ""
#define BLYNK_TEMPLATE_NAME "Smart Trash Can"
#define BLYNK_AUTH_TOKEN ""

#include <WiFi.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "DHTesp.h"

// --- WiFi Credentials ---
char ssid[] = "";
char pass[] = "";

// --- LCD Setup ---
LiquidCrystal_I2C lcd(0x27, 16, 2);
#define SDA_PIN 21
#define SCL_PIN 22

// --- Servo Setup ---
Servo myServo;
#define SERVO_PIN 25

// --- MQ-2 Setup ---
#define MQ2_PIN 33

// --- Ultrasonic Sensor Setup ---
#define TRIG_PIN 14
#define ECHO_PIN 27

// --- Buzzer ---
#define BUZZER_PIN 23

// Manual override: true = user wants buzzer OFF regardless of sensor
bool manualBuzzerOff = false;

// Blynk button handler (V2) to toggle manual override
BLYNK_WRITE(V2) {
  int buttonState = param.asInt();
  if (buttonState == 1) {
    manualBuzzerOff = true;  // User switched buzzer OFF manually
    Serial.println("Buzzer manually turned OFF");
  } else {
    manualBuzzerOff = false; // User allows buzzer automatic operation
    Serial.println("Buzzer manual override OFF, auto mode enabled");
  }
}

void setup() {
  Serial.begin(115200);

  Blynk.begin(BLYNK_AUTH_TOKEN, ssid, pass);

  Wire.begin(SDA_PIN, SCL_PIN);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  lcd.print("Smart Trash Can");

  myServo.setPeriodHertz(50);
  myServo.attach(SERVO_PIN, 500, 2400);

  delay(2000); // MQ2 warm-up

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  delay(1000);
  lcd.clear();
}

void loop() {
  Blynk.run();

  int smoke = analogRead(MQ2_PIN);
  Serial.print("Smoke Reading: ");
  Serial.println(smoke);

  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  int distance = duration * 0.034 / 2;
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  if (smoke > 1000 || distance < 10) {
    myServo.write(90);
    Serial.println("Servo: Open");
  } else {
    myServo.write(0);
    Serial.println("Servo: Closed");
  }

  // Buzzer logic with manual override:
  if (smoke > 2000 && !manualBuzzerOff) {
    digitalWrite(BUZZER_PIN, HIGH);
  } else {
    digitalWrite(BUZZER_PIN, LOW);
  }

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Smoke:");
  lcd.print(smoke);
  lcd.setCursor(0, 1);
  lcd.print("Dist:");
  lcd.print(distance);
  lcd.print("cm");

  Blynk.virtualWrite(V0, smoke);
  Blynk.virtualWrite(V1, distance);

  delay(1000);
}
const int DHT_PIN = 23;
LiquidCrystal_I2C lcd(0x27, 16, 2);
DHTesp dhtSensor;
void setup() {
  dhtSensor.setup(DHT_PIN, DHTesp::DHT22);
  
  lcd.init();
  lcd.backlight();
  
  lcd.setCursor(0, 0);
  lcd.print("Temp:");
  lcd.setCursor(10, 0);
  lcd.print("RH:");
}
void loop() {
  TempAndHumidity data = dhtSensor.getTempAndHumidity();
  lcd.setCursor(0, 1);
  lcd.print(data.temperature, 1);
  lcd.print((char)223);
  lcd.print("C  ");
  lcd.setCursor(10, 1);
  lcd.print(data.humidity, 1);
  lcd.print("%  ");
  delay(2000);
}