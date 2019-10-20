#include <SPI.h>
#include <RF24.h>

RF24 radio(7, 8);

byte address[][7] {"1Dugum", "2Dugum"};

void setup() {
  Serial.begin(9600);
  radio.begin();

  radio.openReadingPipe(1, address[0]);
  radio.openWritingPipe(address[1]);
  
}

void loop() {
  char data;
  radio.startListening();
  if (radio.available()) {
    radio.read(&data, sizeof(data));
    

    char r[] = "onay";
    radio.stopListening();

    radio.write(&r, sizeof(r));
    
  }
  
Serial.println(data);
  


}
