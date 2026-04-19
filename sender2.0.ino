#include <Wire.h>
#include <MPU6050.h>
#include <SPI.h> 
#include <nRF24L01.h>
#include <RF24.h> 

RF24 radio(7, 8); 
const uint64_t address = 0xF0F0F0F0E1LL; 

MPU6050 mpu;
int16_t ax, ay, az, gx, gy, gz;
float pitch = 0, lasttime, gyroBiasY = 0; 

void setup() {
  Serial.begin(9600);
  Wire.begin();
  pinMode(10, OUTPUT); 

  if (!radio.begin()) {
    Serial.println("NRF Error");
    while (1);
  }
  
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setAutoAck(false); 
  radio.openWritingPipe(address);
  radio.stopListening();

  mpu.initialize();
  
  float sumY = 0;
  for (int i = 0; i < 200; i++) {
    mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);
    sumY += gy;
    delay(5);
  }


  gyroBiasY = sumY / 200.0;


  lasttime = millis();
}

void loop() {

  float dt = (millis() - lasttime) / 1000.0;
  lasttime = millis();
  mpu.getMotion6(&ax, &ay, &az, &gx, &gy, &gz);

  float pitch_gyrorate = (gy - gyroBiasY) / 131.0;
  float pitch_acc = atan2(-(float)ax, sqrt((float)ay * ay + (float)az * az)) * 180.0 / M_PI;
  pitch = 0.98 * (pitch + pitch_gyrorate * dt) + 0.02 * pitch_acc;

  
  radio.write(&pitch, sizeof(pitch)); 

  Serial.print("Sending: "); 
  Serial.println(pitch);
  delay(30); 
}