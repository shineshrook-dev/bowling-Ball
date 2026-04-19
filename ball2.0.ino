#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

RF24 radio(7, 8); 
const uint64_t address = 0xF0F0F0F0E1LL; 

void setup() {
  Serial.begin(9600);
  pinMode(10, OUTPUT); 

  if (!radio.begin()) {
    Serial.println("errror");
    while (1);
  }
  
  radio.setDataRate(RF24_250KBPS);
  radio.setPALevel(RF24_PA_MAX);
  radio.setAutoAck(false); 
  radio.openReadingPipe(1, address);
  radio.startListening();
  
  Serial.println("Ready :");
}

void loop() {
  if (radio.available()) {
    float receivedPitch = 0.0;
    while (radio.available()) {
      radio.read(&receivedPitch, sizeof(receivedPitch));
    }
    Serial.print("Pitch: ");
    Serial.println(receivedPitch, 2); 
  }
}