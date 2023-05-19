#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <WiFi.h>
#include <WiFiClient.h>
#include <BlynkSimpleEsp32.h>
#include <ESP32Servo.h>
#include <DHT.h>
const int DHTPin = 25;
DHT dht(DHTPin, DHT22);
Servo myServo;
#define ServoPin 4  //D4

#define BLYNK_TEMPLATE_ID "TMPL6ParGzkaX"
#define BLYNK_TEMPLATE_NAME "ĐATN"
#define BLYNK_AUTH_TOKEN "E7ykdJuXYwaAQTaw-wUq4Oj4Z8AhY1xh"
//char ssid[] = "TP-Link_46A4";
//char pass[] = "63728705";
char ssid[] = "BEAUTIFUL";
char pass[] = "123456777";
char auth[] = BLYNK_AUTH_TOKEN;
LiquidCrystal_I2C lcd(0x27, 20, 4);

int led1 = 16;  //D16
int led2 = 17;

#define RST_PIN 2
#define SS_PIN 5

byte readCard[4];
//String MasterTag = "C11CEE1D";
String MasterTag = "CB52952F";  // REPLACE this Tag ID with your Tag ID!!!
String tagID = "";

// Create instances
MFRC522 mfrc522(SS_PIN, RST_PIN);

int khigas, doamdat1, doamdat2;
float temp;
#define button1 1    // chân D18
#define button2 3    // chân D19
#define thietbi1 27  //15 // chân D15//ngat
#define thietbi2 32  //2 // chân D2 //ngat
#define thietbi3 33
#define thietbi4 13
#define thietbi5 14
boolean bt1_state = HIGH;
boolean bt2_state = HIGH;
String status1;
String status2;
int st1;
int st2;
//unsigned long times = 0;
unsigned long time1 = 0;
unsigned long time2 = 0;
//unsigned long time3 = 0;
BLYNK_WRITE(V3) {
  int p = param.asInt();

  digitalWrite(thietbi1, p);
}
BLYNK_WRITE(V4) {
  int p1 = param.asInt();

  digitalWrite(thietbi2, p1);
}
byte degree[8] = {
  0B01110,
  0B01010,
  0B01110,
  0B00000,
  0B00000,
  0B00000,
  0B00000,
  0B00000
};
void setup() {
  Blynk.begin(auth, ssid, pass, "blynk.cloud", 8080);
  //Serial.begin(9600);
  myServo.attach(ServoPin);
  myServo.write(0);
  //Serial.begin(9600);
  lcd.init();
  lcd.backlight();
  SPI.begin();         // SPI bus
  mfrc522.PCD_Init();  // MFRC522
  lcd.createChar(2, degree);
  dht.begin();
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);
  pinMode(thietbi1, OUTPUT);
  pinMode(thietbi2, OUTPUT);
  pinMode(thietbi3, OUTPUT);
  pinMode(thietbi4, OUTPUT);
  pinMode(thietbi5, OUTPUT);
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
}

void loop() {
  Blynk.run();

  if ((unsigned long)(millis() - time1) > 1000) {
    RFID();
    time1 = millis();
  }
  bt();
  if ((unsigned long)(millis() - time2) > 2000) {
    cambien();
    time2 = millis();
  }
  //RFID();
  //bt();
  hienthi();
  //cambien();
  //delay(1000);
}

//Read new tag if available
boolean getID() {
  // Getting ready for Reading PICCs
  if (!mfrc522.PICC_IsNewCardPresent()) {  //If a new PICC placed to RFID reader continue
    return false;
  }
  if (!mfrc522.PICC_ReadCardSerial()) {  //Since a PICC placed get Serial and continue
    return false;
  }
  tagID = "";
  for (uint8_t i = 0; i < 4; i++) {  // The MIFARE PICCs that we use have 4 byte UID
    //readCard[i] = mfrc522.uid.uidByte[i];
    tagID.concat(String(mfrc522.uid.uidByte[i], HEX));  // Adds the 4 bytes in a single String variable
  }
  tagID.toUpperCase();
  mfrc522.PICC_HaltA();  // Stop reading
  return true;
}
void RFID() {
  while (getID()) {
    if (tagID == MasterTag) {
      digitalWrite(led1, HIGH);
      myServo.write(100);
      digitalWrite(led2, LOW);
    } else {
      digitalWrite(led2, HIGH);
      digitalWrite(led1, LOW);
    }
    //Serial.println(tagID);
    delay(2000);
    digitalWrite(led2, LOW);
    digitalWrite(led1, LOW);
    myServo.write(0);
  }
}

void cambien() {
  //khi gas
  analogReadResolution(10);
  khigas = analogRead(A0);
  if (khigas > 200)
    digitalWrite(thietbi5, HIGH);
  else
    digitalWrite(thietbi5, LOW);
  Blynk.virtualWrite(V2, khigas);

  //dht22
  temp = dht.readTemperature();
  if (temp > 30)
    digitalWrite(thietbi4, HIGH);
  else
    digitalWrite(thietbi4, LOW);
  Blynk.virtualWrite(V0, temp);

  //do am dat
  doamdat1 = analogRead(A3);  // đọc giá trị độ ẩm đất từ cảm biến
  doamdat2 = map(doamdat1, 0, 1023, 100, 0);
  if (doamdat2 <= 10)
    digitalWrite(thietbi3, HIGH);
  if (doamdat2 >= 30)
    digitalWrite(thietbi3, LOW);
  Blynk.virtualWrite(V1, doamdat2);
  Blynk.virtualWrite(V5, digitalRead(thietbi3));
  Blynk.virtualWrite(V6, digitalRead(thietbi4));
  Blynk.virtualWrite(V7, digitalRead(thietbi5));
}

void bt() {
  if (digitalRead(button1) == LOW)  //1
  {
    if (bt1_state == HIGH) {
      digitalWrite(thietbi1, !digitalRead(thietbi1));
      Blynk.virtualWrite(V3, digitalRead(thietbi1));
      bt1_state = LOW;
    }
  } else
    bt1_state = HIGH;

  st1 = digitalRead(thietbi1);
  if (st1 == 1)
    status1 = "ON ";
  else
    status1 = "OFF";

  if (digitalRead(button2) == LOW)  //2
  {
    if (bt2_state == HIGH) {
      digitalWrite(thietbi2, !digitalRead(thietbi2));
      Blynk.virtualWrite(V4, digitalRead(thietbi2));
      bt2_state = LOW;
    }
  } else
    bt2_state = HIGH;

  st2 = digitalRead(thietbi2);
  if (st2 == 1)
    status2 = "ON ";
  else
    status2 = "OFF";
}

void hienthi() {
  lcd.setCursor(0, 0);
  lcd.print("DO AM DAT: ");
  lcd.setCursor(11, 0);
  lcd.print(doamdat2);
  lcd.print("%  ");

  lcd.setCursor(0, 1);
  lcd.print("NHIET DO: ");
  lcd.print(temp);
  lcd.write(2);
  lcd.print("C  ");

  lcd.setCursor(0, 2);
  lcd.print("CO Co2: ");
  lcd.print(khigas);
  lcd.print("PPM   ");

  lcd.setCursor(0, 3);
  lcd.print("TB1:");
  lcd.setCursor(5, 3);
  lcd.print(status1);

  lcd.setCursor(10, 3);
  lcd.print("TB2:");
  lcd.setCursor(15, 3);
  lcd.print(status2);
}
