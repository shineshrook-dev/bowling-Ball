#include<Wire.h>
#include<MPU6050.h>
#include<SPI.h>
#include<nRF24l01.h>
#include<RF24.h>
#define ce 9
#define csn 10

RF24 radio(ce,csn);
const uint64_t address = 0xE8E8F0E1LL;

float angle;

MPU6050 mpu ;
int16_t ax , ay , az , gx , gy , gz ;

float lasttime , pitch ;

void setup() {

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_LOW);
  radio.stopListening(); //send data

  Serial.begin(9600) ;  
  Wire.begin() ;
  mpu.initialize() ;

  if (mpu.testConnection()) 
     Serial.println("ready") ;
  else 
    Serial.println("notready") ;  

  lasttime = millis() ;

}

void loop() {

  float dt = (millis()-lasttime) ;
  lasttime = millis() ;
  
  mpu.getMotion6(&ax,&ay,&az, &gx,&gy,&gz) ;
  float pitch_gyrorate = gy/131.0 ;

  float pitch_acc = atan2(-ax,sqrt(ay*ay+az*az)) ;

  pitch = 0.98 *(pitch+pitch_gyrorate*dt) + 0.02*pitch_acc  ;
  Serial.println(pitch) ;  

  angle=pitch ; //هنجرب السيرفو 
 //angle= getAngleFromMPU();//هنقرأ الزاوية

 radio.write(&angle, sizeof(angle));//نبعت الزاوية
 delay(50);
}
