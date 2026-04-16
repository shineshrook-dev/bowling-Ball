#include <Wire.h>
#include <MPU6050.h>

MPU6050 mpu;

int16_t ax, ay, az, gx, gy, gz;


float pitch = 0;
float lasttime;
float gyroBiasY = 0; 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  
  Serial.println("Initializing MPU6050...");
  mpu.initialize();

  if (mpu.testConnection()) {
    Serial.println("MPU6050 connection successful");
  } else {
    Serial.println("MPU6050 connection failed");
    while (1); // Stop if sensor not found
  }

  
  Serial.println("calibrating");
  float sumY = 0;
  for (int i = 0; i < 200; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sumY += gy;
    delay(5);
  }
  gyroBiasY = sumY / 200.0;
  Serial.println("Calibration Done!");
  
  lasttime = millis();
}

void loop() {
  
  float currentTime = millis();
  float dt = (currentTime - lasttime) / 1000.0;
  lasttime = currentTime;


  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float pitch_gyrorate = (gy - gyroBiasY) / 131.0;

  float pitch_acc = atan2(-(float)ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / M_PI;

  pitch = 0.98 * (pitch + pitch_gyrorate * dt) + 0.02 * pitch_acc;

  Serial.print("angle: ");
  Serial.println(pitch);

  delay(10); 
}