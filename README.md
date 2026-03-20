Smart Eco: Waste-to-Points Web Application
Smart Eco เป็นเว็บแอปพลิเคชันนวัตกรรมเพื่อสิ่งแวดล้อมที่เปลี่ยนการรีไซเคิลขยะให้เป็นเรื่องสนุกและคุ้มค่า โดยระบบจะบันทึกการทิ้งขยะ (พลาสติก/โลหะ) จากตู้ IoT และแปลงเป็นแต้มสะสม เพื่อนำไปแลกรับของรางวัลและสิทธิพิเศษต่างๆ

ฟีเจอร์ที่น่าสนใจ (Key Features)
Real-time Dashboard: แสดงแต้มสะสมและระดับเลเวล (Bronze, Silver, Gold) พร้อมระบบคำนวณการลดก๊าซคาร์บอน (CO2)
IoT Smart Collection: เชื่อมต่อกับตู้รับแลกขยะอัตโนมัติ บันทึกจำนวนขวดพลาสติกและโลหะลงฐานข้อมูลทันที
History & Tracking: ระบบประวัติการทำรายการแบบละเอียด แยกสีชัดเจนระหว่าง "การสะสม" (สีเขียว) และ "การแลกรางวัล" (สีแดง)
Redemption Store: ร้านค้าออนไลน์สำหรับแลกของรางวัล เช่น E-book และสิทธิพิเศษอื่นๆ
Smart Notification: ระบบแจ้งปัญหาการใช้งาน เชื่อมต่อผ่าน n8n Webhook เพื่อแจ้งเตือนแอดมินทาง LINE Messaging API แบบทันที
Multi-language Support: รองรับการใช้งานได้ทั้งภาษาไทยและภาษาอังกฤษ (TH/EN)

เทคโนโลยีที่ใช้ (Tech Stack)
Frontend: HTML5, CSS3 (Custom UI), Modern JavaScript (ES6+)
Database: Google Firebase Realtime Database
Automation: n8n.io (Workflow Automation)
API Integration: LINE Messaging API (Line Bot)
Tools: Git, GitHub, VS Code

ส่วนประกอบของ Hardware (IoT)
ในโปรเจกต์นี้ประกอบด้วยตู้ขยะอัจฉริยะ 2 จุด
1. **B1japan:** โค้ดสำหรับตู้สะสมขวดพลาสติก/โลหะ  
2. **B2_feb14a:** โค้ดสำหรับตู้สะสม (เวอร์ชันปรับปรุงล่าสุด)

อุปกรณ์ที่ใช้:
* ไมโครคอนโทรลเลอร์: ESP32 / ESP8266
* เซนเซอร์: เซ็นเซอร์ตรวจับโลหะ, Ultrasonic (วัดระดับขยะ)
* การเชื่อมต่อ: เชื่อมต่อผ่าน WiFi ไปยัง Firebase Realtime Database

จัดทำโดย
ณิตญา คำสมศรี (ครีม) (ID:67010974005)
ปาริชาติ ทัพธานี (เจี๊ยบ)(ID:67010974010)
นักศึกษาคณะบัญชีและการจัดการ มหาวิทยาลัยมหาสารคาม
