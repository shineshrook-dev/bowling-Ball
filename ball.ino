#include <RF24.h>                                
#include <nRF24L01.h>
#include <SPI.h>
#include <Servo.h>
#include<MPU6050.h>
#include <PID_v1.h>

#define ServoPin 6
#define ce 9
#define csn 10
Servo serv1;
RF24 radio(ce,csn);

MPU6050 mpu2;
int16_t ax,ay,az,gx,gy,gz;
float last_time;
float input;                               //the angel of the ball NOW after using the servo
float pi = 3.14159 ;
float set_point;                          //the angel that must the ball move ( that sent to the servo ) (the actual angel)
float output;                             //the angel that the servo will take
float error = 0 ;                         //the difference between the set point (the actual angel) and input (the angel of the ball NOW) 
float preverror = 0;                      //previous error  
float kp=2 , ki=0.5 , kd=0.1 ;            //this is random numbers //variables of pid
float proportional = 0 , integral = 0 , derivative = 0;     //concepts of pid


const uint64_t address = 0xE8E8F0E1LL;
float angle;

void setup() {

Serial.begin(9600);
Wire.begin();
serv1.attach(ServoPin);
mpu2.initialize();

if (!radio.begin()) {
Serial.println("Problem occured");
while(1);
}

radio.setPALevel(RF24_PA_LOW);
radio.openReadingPipe(0, address);
radio.startListening();
radio.printDetails();
Serial.println("Receiver Ready");

if(mpu2.testConnection()){
Serial.println("MPU is ready");
}
else{
Serial.println("MPU is not connected !");
}

last_time = millis();
}


void loop() {
if (radio.available()) {

radio.read(&angle, sizeof(angle));
angle = constrain(angle,0,180);
Serial.println(angle);
}

float dt = (millis() - last_time)/1000 ;     //convert it to seconds
last_time = millis();                        //to update the last time

mpu2.getMotion6(&ax,&ay,&az,&gx,&gy,&gz);     //measure acclration and angular velocity

set_point = angle;
float roll_from_acc = atan2(ay,az) *180.0/pi ;          //the angel in x-axis from acclration
float roll_gx = gx/131.0;                    //to convert gx from 16-bits to angel (make it same scale )
input = ( input + (roll_gx*dt) ) *0.98 + ( roll_from_acc) *0.02;        //input is combine between the angel from acclration and the angle from gyroscope
error = set_point - input;
proportional = error;
integral = integral + error*dt;              //this equation is the integration of the error to dt
derivative = ( error - preverror )/dt;
preverror = error;
output = (kp*proportional) + (ki*integral) + (kd*derivative);         //the corrected angel that the servo will take
output = constrain(output,0,180);                                //mapping it between 0 and 180 for the servo

serv1.write(output);

}