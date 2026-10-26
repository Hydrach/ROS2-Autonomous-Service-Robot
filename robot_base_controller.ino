// --- ENKODER VE MOTOR ---
volatile long sol_tika_sayac = 0;
int motor_yonu = 0; // 1: İleri, -1: Geri

const int enA = 5, enB = 6;
const int in1 = 9, in2 = 10, in3 = 11, in4 = 12;
const int sol_sensor_pin = 2; 

unsigned long sonYazdirma = 0;

void setup() {
  Serial.begin(115200);
  pinMode(enA, OUTPUT); pinMode(enB, OUTPUT);
  pinMode(in1, OUTPUT); pinMode(in2, OUTPUT);
  pinMode(in3, OUTPUT); pinMode(in4, OUTPUT);
  pinMode(sol_sensor_pin, INPUT);
  attachInterrupt(digitalPinToInterrupt(sol_sensor_pin), solTekerInterrupt, RISING);
}

void loop() {
  if (Serial.available() > 0) {
    String veri = Serial.readStringUntil('\n');
    int virgul = veri.indexOf(',');
    if (virgul > 0) {
      int sol_pwm = veri.substring(0, virgul).toInt();
      int sag_pwm = veri.substring(virgul + 1).toInt();
      motorKontrol(sol_pwm, sag_pwm);
    }
  }

  if (millis() - sonYazdirma > 50) {
    // Sağ enkoder yok, sol veriyi sağa da basıyoruz (Düz gidiş için)
    // Dönüşlerde ROS içindeki Bridge bunu kompanse edecek
    Serial.print(sol_tika_sayac);
    Serial.print(",");
    Serial.println(sol_tika_sayac); 
    sonYazdirma = millis();
  }
}

void solTekerInterrupt() {
  if (motor_yonu == -1) sol_tika_sayac--; else sol_tika_sayac++;
}

void motorKontrol(int sol, int sag) {
  sol = constrain(sol, -255, 255); sag = constrain(sag, -255, 255);
  // Basit motor yön mantığı
  digitalWrite(in3, sol > 0 ? HIGH : LOW); digitalWrite(in4, sol > 0 ? LOW : HIGH);
  analogWrite(enB, abs(sol));
  digitalWrite(in1, sag > 0 ? HIGH : LOW); digitalWrite(in2, sag > 0 ? LOW : HIGH);
  analogWrite(enA, abs(sag));
  motor_yonu = (sol + sag) > 0 ? 1 : -1;
}