#include <ESP32Servo.h>

// --- กำหนดขา Pin ---
#define TRIG_PIN 5      
#define ECHO_PIN 18     
#define SENSOR_METAL 15 
#define SERVO_PIN 4    
#define BUZZER_PIN 22   

Servo gateServo;

void setup() {
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, 16, 17); // TX ขา 17 ส่งไปบอร์ด 1
  
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
  pinMode(SENSOR_METAL, INPUT_PULLUP);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH); 

  ESP32PWM::allocateTimer(0);
  gateServo.setPeriodHertz(50);
  gateServo.attach(SERVO_PIN, 500, 2400); 

  gateServo.write(0); // ท่าเริ่ม (ปิด)
  Serial.println("Board 2: Ready - Logic Updated");
}

void loop() {
  // 1. อ่านระยะ Ultrasonic (สำหรับนับพลาสติก)
  digitalWrite(TRIG_PIN, LOW); delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH); delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH, 25000);
  int distance = (duration * 0.034) / 2;

  // 2. อ่านค่าโลหะ
  int valMetal = digitalRead(SENSOR_METAL);

  // --- LOGIC 1: ตรวจจับพลาสติก (Ultrasonic) ---
  if (distance > 0 && distance <= 5) {
    Serial2.println("PLASTIC");  // ส่งให้บอร์ด 1 นับ
    Serial.println("Sent: PLASTIC (Servo NOT moving)");
    
    // แจ้งเตือนด้วยเสียงสั้นๆ (ถ้าต้องการ) แต่ไม่สั่ง Servo
    digitalWrite(BUZZER_PIN, LOW); delay(100); digitalWrite(BUZZER_PIN, HIGH);
    
    delay(1500); // หน่วงเวลาเพื่อกันการนับซ้ำ (ขยะตกลงไปแล้ว)
  }

  // --- LOGIC 2: ตรวจจับโลหะ (Metal Sensor) ---
  if (valMetal == LOW) {
    Serial2.println("METAL");    // ส่งให้บอร์ด 1 นับ
    Serial.println("Sent: METAL (Servo IS moving)");

    // สั่ง Servo ทำงาน (เปิดไม้กั้นสำหรับโลหะ)
    digitalWrite(BUZZER_PIN, LOW); // เสียงเตือนยาวกว่านิดนึง
    gateServo.write(90);           // เปิดไม้กั้น
    delay(2000);                   // รอขยะลง
    gateServo.write(0);            // ปิดไม้กั้น
    digitalWrite(BUZZER_PIN, HIGH);
    
    delay(1000); // หน่วงเวลาป้องกันการนับซ้ำ
  }

  delay(50); 
}