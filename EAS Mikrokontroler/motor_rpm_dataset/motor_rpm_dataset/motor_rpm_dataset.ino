/* ======================================
   DATA COLLECTION MOTOR DC - IMPROVED
   Tujuan: Mendapatkan data stabil untuk AI
====================================== */

const int IN1 = 27;
const int IN2 = 26;
const int EN  = 12;
const int rpmPin = 13;

volatile unsigned long pulseCount = 0;
float rpm = 0;
int pwmOutput = 0;

unsigned long lastSampleTime = 0;
unsigned long lastPwmChangeTime = 0;

// Pengaturan Data Collection
int pwmStep = 10;           // Naik sedikit demi sedikit
int stepInterval = 3000;    // Tunggu 3 detik agar motor stabil sebelum ambil data
int pwmMax = 255;

void IRAM_ATTR rpmISR() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(rpmPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rpmPin), rpmISR, RISING);

  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  
  Serial.println("PWM,RPM"); // Header untuk CSV
}

void loop() {
  unsigned long currentMillis = millis();

  // 1. Logika Kenaikan PWM (Setiap 3 Detik)
  if (currentMillis - lastPwmChangeTime >= stepInterval) {
    if (pwmOutput <= pwmMax) {
      // Tampilkan data ke Serial SEBELUM PWM naik lagi (saat sudah stabil)
      Serial.print(pwmOutput);
      Serial.print(",");
      Serial.println(rpm);
      
      pwmOutput += pwmStep;
      if (pwmOutput > pwmMax) {
        pwmOutput = 0; // Reset atau stop jika sudah selesai
        Serial.println("DATA_COLLECTION_DONE");
      }
      
      analogWrite(EN, pwmOutput);
    }
    lastPwmChangeTime = currentMillis;
  }

  // 2. Logika Update RPM (Setiap 100ms untuk monitoring)
  if (currentMillis - lastSampleTime >= 100) {
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    // Hitung RPM (Sesuaikan kalibrasi lubang piringanmu)
    // Jika 2 lubang, maka: (pulses / 2) * (60000 / 100)
    rpm = pulses * 300; 

    lastSampleTime = currentMillis;
  }
}