#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <Keypad.h>
#include <SoftwareSerial.h>

#define sim800 Serial  // SIM800L using default UART0: TX=GPIO1, RX=GPIO3

LiquidCrystal_I2C lcd(0x27, 16, 2);
RTC_DS1307 rtc;

#define RELAY_PIN 4
#define BUZZER_PIN 5

// RFID using SoftwareSerial (EM-18 TX to GPIO16)
SoftwareSerial rfidSerial(16, 17); // Only RX from EM-18

const String validRFID = "55001AA37894";
const String supervisor = "+918329311831";
const String authority1 = "+917350709147";
const String authority2 = "+917249209387";

char keys[4][3] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[4] = {27, 26, 25, 12};
byte colPins[3] = {14, 33, 32};

Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, 4, 3);

String generatedOTP = "";
String enteredOTP = "";

unsigned long otpGeneratedTime = 0;
bool otpMode = false;
bool rfidScanned = false;
bool boxOpened = false;  //  Track if box has been opened

void setup() {
  Serial.begin(9600);           // SIM800L using UART0
  rfidSerial.begin(9600);       // RFID
  lcd.init();
  lcd.backlight();

  pinMode(RELAY_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, HIGH);   //  Lock solenoid initially
  digitalWrite(BUZZER_PIN, LOW);

  if (!rtc.begin()) {
    lcd.print("RTC NOT FOUND");
    while (1);
  }

  lcd.setCursor(0, 0);
  lcd.print("PAPER LEAK");
  lcd.setCursor(0, 1);
  lcd.print("DETECTION SYSTEM");
  delay(2000);
  lcd.clear();
}

void loop() {
  if (!rfidScanned && !boxOpened) {
    lcd.setCursor(0, 0);
    lcd.print("Scan RFID...");
    if (rfidSerial.available()) {
      String rfidData = "";
      while (rfidSerial.available()) {
        rfidData += (char)rfidSerial.read();
        delay(5);
      }
      rfidData.trim();
      if (rfidData == validRFID) {
        rfidScanned = true;
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("RFID Valid");
        sendSMS("RFID verified. Supervisor access granted.");
        generateOTP();
      } else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Invalid RFID!");
        delay(2000);
        lcd.clear();
      }
    }
  } else if (!boxOpened) {
    readKeypad();
  }
}

void generateOTP() {
  generatedOTP = "";
  for (int i = 0; i < 4; i++) {
    generatedOTP += String(random(0, 10));
  }
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("OTP: " + generatedOTP);
  delay(5000);
  lcd.clear();
  otpGeneratedTime = millis();
  otpMode = true;
}

void readKeypad() {
  if (otpMode) {
    char key = keypad.getKey();
    if (key) {
      if (key == '#') {
        checkOTP();
      } else if (key == '*') {
        enteredOTP = "";
      } else {
        if (enteredOTP.length() < 4) {
          enteredOTP += key;
        }
      }

      lcd.setCursor(0, 0);
      lcd.print("Enter OTP:    ");
      lcd.setCursor(0, 1);
      lcd.print(enteredOTP + "    ");
    }

    if (millis() - otpGeneratedTime > 60000) {
      enteredOTP = "";
      generateOTP();
    }
  }
}

void checkOTP() {
  if (enteredOTP == generatedOTP) {
    DateTime now = rtc.now();
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Box Opened:");
    lcd.setCursor(0, 1);
    lcd.print(now.hour());
    lcd.print(":");
    lcd.print(now.minute());
    
    digitalWrite(RELAY_PIN, LOW);   //  Unlock solenoid
    delay(5000);
    // Optional: Lock again after some time
    // digitalWrite(RELAY_PIN, HIGH);  //  Re-lock if needed

    //  Send timestamped alert to higher authorities
    String timeStr = String("Box opened at: ") + String(now.hour()) + ":" + String(now.minute()) + " on " +
                     String(now.day()) + "/" + String(now.month()) + "/" + String(now.year());
    sendSMS(timeStr);

    //  Show thank you message
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Thank You!");
    
    otpMode = false;
    boxOpened = true;
    enteredOTP = "";
  } else {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Wrong OTP!");
    digitalWrite(BUZZER_PIN, HIGH);
    sendSMS("Wrong OTP entered! Suspected paper leak.");
    delay(3000);
    digitalWrite(BUZZER_PIN, LOW);
    enteredOTP = "";
    otpMode = false;
    delay(60000);
    generateOTP();
  }
}

void sendSMS(String message) {
  sim800.println("AT+CMGF=1");  // Set SMS text mode
  delay(100);

  sim800.print("AT+CMGS=\"");
  sim800.print(authority1);  // First authority
  sim800.println("\"");
  delay(100);
  sim800.print(message);
  sim800.write(26); // Ctrl+Z
  delay(5000);

  sim800.print("AT+CMGS=\"");
  sim800.print(authority2);  // Second authority
  sim800.println("\"");
  delay(100);
  sim800.print(message);
  sim800.write(26); // Ctrl+Z
  delay(5000);

  // Optional debug
  while (sim800.available()) {
    Serial.write(sim800.read());
  }
}
