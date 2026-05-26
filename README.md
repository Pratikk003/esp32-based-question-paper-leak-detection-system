# esp32-based-question-paper-leak-detection-system
ESP32-based Question Paper Leak Detection System using RFID, OTP Authentication, RTC DS1307, GSM Alerts, Solenoid Lock, and Real-Time Monitoring.
# Question Paper Leak Detection System

## Overview
An ESP32-based security system designed to prevent unauthorized access to confidential question papers. The system combines RFID authentication, OTP verification, RTC-based timestamp logging, GSM alerts, and an electronic solenoid lock to provide a multi-layered security solution.

## Features
- RFID-based supervisor authentication
- OTP generation and verification via keypad
- Solenoid lock access control
- RTC-based date and time logging
- GSM SMS alerts to authorities
- 16x2 I2C LCD user interface
- Buzzer alarm for unauthorized attempts
- Real-time access monitoring and notifications

## Hardware Components
- ESP32 DevKit V1
- EM-18 RFID Reader
- 4×3 Matrix Keypad
- DS1307 RTC Module
- SIM800L GSM Module
- 16×2 LCD with I2C
- Solenoid Lock
- Relay Module
- Buzzer

## System Workflow
1. Supervisor scans an authorized RFID card.
2. System validates the RFID tag.
3. A 4-digit OTP is generated.
4. User enters the OTP using the keypad.
5. If OTP is valid:
   - Solenoid lock is unlocked.
   - Access timestamp is recorded.
   - SMS notification is sent to authorities.
6. If OTP is invalid:
   - Buzzer alarm is activated.
   - Alert SMS is sent to authorities.
   - Access is denied and a new OTP is generated after timeout.

## Applications
- Examination Centers
- Educational Institutions
- Secure Document Storage
- Government Offices
- Confidential Record Protection

## Technologies Used
- Embedded C/C++
- ESP32
- RFID Authentication
- GSM Communication
- RTC Time Management
- Embedded Security Systems

## Future Enhancements
- Cloud-based monitoring dashboard
- Biometric authentication
- Mobile application integration
- Access log storage on database
- Wi-Fi based notifications

## Author
**Pratik Kambire**  
Embedded & IoT Developer | ESP32 | Raspberry Pi | Embedded Systems
