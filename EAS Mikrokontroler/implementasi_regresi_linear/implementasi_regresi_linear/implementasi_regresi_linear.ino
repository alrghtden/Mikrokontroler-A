/* * SMART MOTOR CONTROLLER - LINEAR REGRESSION
 * Menggunakan model: PWM = (m * RPM) + c
 */

// Konfigurasi Pin
const int IN1 = 27;
const int IN2 = 26;
const int EN  = 12;
const int rpmPin = 13;

// Variabel Sensor RPM
volatile unsigned long pulseCount = 0;
float currentRPM = 0;
unsigned long lastMillis = 0;

// Parameter Model Regresi Linear
const float m = 0.0095;  // Slope
const float c = 65.7;    // Intercept (Dead zone offset)

// Target Kecepatan
float targetRPM = 5000;  

// Fungsi Interrupt untuk hitung pulsa
void IRAM_ATTR rpmISR() {
  pulseCount++;
}

void setup() {
  Serial.begin(115200);

  // Setup Hardware
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(EN, OUTPUT);
  pinMode(rpmPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(rpmPin), rpmISR, RISING);

  // Set Arah Motor (Maju)
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);

  Serial.println("--- Sistem AI Regresi Linear Motor DC ---");
  Serial.print("Target yang diminta: "); Serial.print(targetRPM); Serial.println(" RPM");
  
  // STEP AI: Memprediksi PWM berdasarkan target menggunakan model linear
  float predictedPWM = (m * targetRPM) + c;
  
  // Batasi hasil prediksi agar tetap dalam range 0-255
  int finalPWM = (int)constrain(predictedPWM, 0, 255);

  // Jalankan motor dengan hasil tebakan AI
  analogWrite(EN, finalPWM);
  
  Serial.print("AI Memprediksi PWM: "); Serial.println(finalPWM);
  Serial.println("Menghitung akurasi dalam 3 detik...");
  delay(3000); // Tunggu motor stabil sebelum mulai monitoring
}

void loop() {
  // Update RPM setiap 1 detik untuk melihat seberapa akurat tebakan AI
  if (millis() - lastMillis >= 1000) {
    noInterrupts();
    unsigned long pulses = pulseCount;
    pulseCount = 0;
    interrupts();

    // Hitung RPM (Sampling 1 detik, 2 lubang piringan)
    // Rumus: (pulsa / 2 lubang) * 60 detik
    currentRPM = (pulses / 2.0) * 60.0;

    // Hitung Selisih (Error) antara Prediksi AI vs Kenyataan
    float accuracy = 100.0 - (abs(targetRPM - currentRPM) / targetRPM * 100.0);

    // Tampilkan di Serial Monitor
    Serial.print("Target: "); Serial.print(targetRPM);
    Serial.print(" | Realitas: "); Serial.print(currentRPM);
    Serial.print(" | Akurasi AI: "); Serial.print(accuracy); Serial.println("%");

    lastMillis = millis();
  }
}