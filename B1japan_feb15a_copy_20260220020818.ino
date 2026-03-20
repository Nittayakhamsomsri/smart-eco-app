#include <SPI.h>
#include <Adafruit_GFX.h>
#include <Adafruit_ILI9341.h>
#include <Keypad.h>
#include <WiFi.h>
#include <FirebaseESP32.h>

// --- TFT Pins ---
#define TFT_CS   15
#define TFT_DC    2
#define TFT_RST   4
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC, TFT_RST);

#define BUZZER_PIN 22
#define RXD2 16
#define TXD2 17

// --- Variables ---
int plasticCount = 0; 
int metalCount = 0;   
String inputCode = ""; 
String reportUserPhone = ""; 
const int maxCodeLength = 10;

// --- Keypad Config ---
const byte ROWS = 4;
const byte COLS = 4;
char keys[ROWS][COLS] = {
  {'1', '2', '3', 'A'}, 
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'}, 
  {'*', '0', '#', 'D'}
};
byte rowPins[ROWS] = {13, 5, 14, 27};
byte colPins[COLS] = {26, 25, 33, 32};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// --- WiFi & Firebase ---
#define WIFI_NAME "jiab"
#define WIFI_PASSWORD "12345678"
#define FIREBASE_HOST "smart-eco-app-default-rtdb.asia-southeast1.firebasedatabase.app"
#define FIREBASE_AUTH "ol3nXLLpSEABBbTKFaJvVSBdyNEwBrwwdhXNPUNM"

FirebaseData firebaseData;
FirebaseAuth auth;
FirebaseConfig config;

enum ScreenState { MAIN_MENU, ENTER_CODE, HELP_PAGE, ABOUT_PAGE, RESULT_PAGE, COLLECTING, REPORT_MENU, REPORT_PHONE };
ScreenState currentState = MAIN_MENU;

// --- Helper Functions ---
void beep(int duration) {
  digitalWrite(BUZZER_PIN, LOW);
  delay(duration);
  digitalWrite(BUZZER_PIN, HIGH);
}

void resetSystem() {
  inputCode = ""; reportUserPhone = ""; plasticCount = 0; metalCount = 0;
  currentState = MAIN_MENU; 
  drawMainMenu();
}

void showErrorMessage(String msg) {
  tft.fillScreen(ILI9341_RED);
  tft.drawRect(10, 10, 300, 220, ILI9341_WHITE);
  tft.setCursor(65, 80); tft.setTextColor(ILI9341_WHITE); tft.setTextSize(4);
  tft.println("ERROR!");
  tft.setCursor(40, 140); tft.setTextSize(2);
  tft.println(msg);
  beep(1000); 
  delay(3000);
  resetSystem();
}

// --- UI Display Functions ---
void drawMainMenu() {
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRect(5, 5, 310, 230, ILI9341_MAGENTA);
  tft.setCursor(60, 30); tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(3);
  tft.println("SMART ECO");
  tft.setTextSize(2); tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(30, 70);  tft.println("[A] : Start Entry");
  tft.setCursor(30, 105); tft.println("[B] : How to Use");
  tft.setCursor(30, 140); tft.println("[C] : Credits");
  tft.setTextColor(ILI9341_RED);
  tft.setCursor(30, 175); tft.println("[D] : Report Problem");
}

void drawCodeScreen(String title) {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 20); tft.setTextColor(ILI9341_ORANGE); tft.setTextSize(2);
  tft.println(title);
  updateInputDisplay();
}

void updateInputDisplay() {
  tft.fillRect(20, 60, 280, 40, ILI9341_BLACK);
  tft.setCursor(20, 70); tft.setTextColor(ILI9341_GREEN); tft.setTextSize(3);
  tft.println(inputCode);
}

void drawHelpPage() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 30); tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(3);
  tft.println("HOW TO USE");
  tft.setTextSize(2); tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(20, 80); tft.println("1. Press [A] Start");
  tft.setCursor(20, 110); tft.println("2. Input Phone No.");
  tft.setCursor(20, 140); tft.println("3. Drop Trash");
  tft.setCursor(20, 170); tft.println("4. Press [#] Finish");
  tft.setCursor(20, 210); tft.setTextSize(1); tft.println("Press [*] to return");
}

void drawAboutPage() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(20, 30); tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(3);
  tft.println("CREDITS");
  tft.setTextSize(1); tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(20, 80); tft.println("- Nittaya Khamsomsri");
  tft.setCursor(20, 100); tft.println("- Parichat Thaptanee");
  tft.setCursor(20, 140); tft.setTextSize(2); tft.println("Advisor:");
  tft.setTextSize(1); tft.setCursor(20, 170); tft.println("- Sompoch Thongnamthiang");
  tft.setCursor(20, 210); tft.println("Press [*] to return");
}

void drawReportMenu() {
  tft.fillScreen(ILI9341_BLACK);
  tft.drawRect(5, 5, 310, 230, ILI9341_RED);
  tft.setCursor(50, 30); tft.setTextColor(ILI9341_RED); tft.setTextSize(3);
  tft.println("REPORT ISSUE");
  tft.setTextSize(2); tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(20, 80);  tft.println("[A] : Points error");
  tft.setCursor(20, 120); tft.println("[B] : Bin is full");
  tft.setCursor(20, 160); tft.println("[C] : Others");
}

void showSuccess() {
  tft.fillScreen(ILI9341_GREEN);
  tft.setCursor(50, 40); tft.setTextColor(ILI9341_BLACK); tft.setTextSize(3);
  tft.println("READY!"); 
  updateCollectingDisplay();
}

void updateCollectingDisplay() {
  tft.fillRect(20, 90, 280, 100, ILI9341_GREEN); 
  tft.setTextColor(ILI9341_BLACK); tft.setTextSize(3);
  tft.setCursor(40, 110); tft.print("Plastic: "); tft.println(plasticCount);
  tft.setCursor(40, 150); tft.print("Metal  : "); tft.println(metalCount);
}

// --- ฟังก์ชันที่แก้ไข: เพิ่มการแสดงเบอร์โทรศัพท์ยืนยันในหน้าสรุปผล ---
void showFinalSuccess() {
  tft.fillScreen(ILI9341_BLUE);
  tft.drawRect(5, 5, 310, 230, ILI9341_WHITE);
  
  tft.setCursor(65, 20); tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(3);
  tft.println("SUMMARY"); 

  // แสดงเบอร์โทรศัพท์ผู้ใช้งาน
  tft.setCursor(40, 60); tft.setTextSize(2); tft.setTextColor(ILI9341_CYAN);
  tft.print("User: "); tft.println(inputCode); 
  tft.drawLine(40, 78, 280, 78, ILI9341_WHITE); 

  tft.setTextColor(ILI9341_WHITE);
  tft.setCursor(40, 95); tft.print("Plastic : "); tft.print(plasticCount); tft.println(" pcs");
  tft.setCursor(40, 130); tft.print("Metal   : "); tft.print(metalCount); tft.println(" pcs");
  
  int score = (plasticCount * 5) + (metalCount * 10);
  tft.setCursor(40, 175); tft.setTextSize(3); tft.setTextColor(ILI9341_ORANGE);
  tft.print("SCORE: +"); tft.println(score);
  
  tft.setTextSize(1); tft.setCursor(65, 215); tft.setTextColor(ILI9341_LIGHTGREY); 
  tft.println("Press [*] to Home");
}

// --- Firebase Functions ---
void sendReportToFirebase(String problemText) {
  tft.fillScreen(ILI9341_RED); 
  tft.setCursor(60, 100); tft.setTextSize(2); tft.setTextColor(ILI9341_WHITE);
  tft.println("SENDING...");

  FirebaseJson report;
  report.set("device_id", "SMART-BIN-01");
  report.set("issue", problemText);
  report.set("reporter_phone", reportUserPhone); 
  report.set("status", "Pending");

  if (Firebase.pushJSON(firebaseData, "/reports", report)) {
    tft.fillScreen(ILI9341_BLACK); 
    tft.drawRect(5, 5, 310, 230, ILI9341_GREEN);
    tft.setCursor(60, 100); tft.setTextColor(ILI9341_GREEN);
    tft.println("REPORT SENT!"); 
    beep(500); delay(2000);
  }
  resetSystem();
}

void addPointsToFirebase(String phone, int p, int m) {
  int curPts = 0, curP = 0, curM = 0;
  if (Firebase.getInt(firebaseData, "/users/" + phone + "/points")) curPts = firebaseData.intData();
  if (Firebase.getInt(firebaseData, "/users/" + phone + "/plastic_count")) curP = firebaseData.intData();
  if (Firebase.getInt(firebaseData, "/users/" + phone + "/metal_count")) curM = firebaseData.intData();
  FirebaseJson up;
  up.set("points", curPts + (p*5) + (m*10));
  up.set("plastic_count", curP + p);
  up.set("metal_count", curM + m);
  Firebase.updateNode(firebaseData, "/users/" + phone, up);
}

// --- Setup ---
void setup() {
  Serial.begin(115200); Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);
  pinMode(BUZZER_PIN, OUTPUT); digitalWrite(BUZZER_PIN, HIGH);
  tft.begin(); tft.setRotation(3); tft.fillScreen(ILI9341_BLACK);
  WiFi.begin(WIFI_NAME, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) { delay(500); }
  config.host = FIREBASE_HOST; config.signer.tokens.legacy_token = FIREBASE_AUTH;
  Firebase.begin(&config, &auth); Firebase.reconnectWiFi(true);
  beep(200); resetSystem();
}

// --- Loop ---
void loop() {
  char key = keypad.getKey();
  if (key == '*') { beep(100); resetSystem(); return; }
  
  if (key) {
    beep(50);
    switch (currentState) {
      case MAIN_MENU:
        if (key == 'A') { currentState = ENTER_CODE; inputCode = ""; drawCodeScreen("Enter Member Phone:"); }
        else if (key == 'B') { currentState = HELP_PAGE; drawHelpPage(); }
        else if (key == 'C') { currentState = ABOUT_PAGE; drawAboutPage(); }
        else if (key == 'D') { currentState = REPORT_PHONE; inputCode = ""; drawCodeScreen("Enter Your Phone:"); }
        break;

      case REPORT_PHONE:
        if (key >= '0' && key <= '9' && inputCode.length() < 10) { inputCode += key; updateInputDisplay(); }
        else if (key == 'D' && inputCode.length() > 0) { inputCode.remove(inputCode.length()-1); updateInputDisplay(); }
        else if (key == '#' && inputCode.length() == 10) {
            reportUserPhone = inputCode; 
            currentState = REPORT_MENU; 
            drawReportMenu();
        }
        break;

      case REPORT_MENU:
        if (key == 'A') sendReportToFirebase("Points error");
        else if (key == 'B') sendReportToFirebase("Bin is full");
        else if (key == 'C') sendReportToFirebase("Other issue");
        break;

      case ENTER_CODE:
        if (key >= '0' && key <= '9' && inputCode.length() < 10) { inputCode += key; updateInputDisplay(); }
        else if (key == 'D' && inputCode.length() > 0) { inputCode.remove(inputCode.length()-1); updateInputDisplay(); }
        else if (key == '#' && inputCode.length() == 10) {
            tft.setCursor(20, 120); tft.setTextColor(ILI9341_YELLOW); tft.setTextSize(2); tft.println("Checking...");
            if (Firebase.pathExisted(firebaseData, "/users/" + inputCode)) {
                currentState = COLLECTING; showSuccess();
            } else {
                showErrorMessage("USER NOT FOUND!");
            }
        }
        break;

      case COLLECTING:
        if (key == '#') { 
          currentState = RESULT_PAGE; 
          showFinalSuccess(); 
          addPointsToFirebase(inputCode, plasticCount, metalCount); 
        }
        break;

      case HELP_PAGE:
      case ABOUT_PAGE:
      case RESULT_PAGE:
        break;
    }
  }

  if (Serial2.available()) {
    String data = Serial2.readStringUntil('\n'); data.trim();
    if (currentState == COLLECTING) {
      if (data == "PLASTIC") { plasticCount++; updateCollectingDisplay(); beep(100); }
      else if (data == "METAL") { metalCount++; updateCollectingDisplay(); beep(150); }
    }
  }
}