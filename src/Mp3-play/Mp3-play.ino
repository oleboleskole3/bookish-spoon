#include <DFPlayerMini_Fast.h>

#if !defined(UBRR1H)
#include <SoftwareSerial.h>
SoftwareSerial mySerial(10, 11); // RX, TX
#endif

DFPlayerMini_Fast myMP3;

void setup()
{
  pinMode(13, INPUT_PULLUP);
  Serial.begin(115200);

  Serial1.begin(9600);
  myMP3.begin(Serial1, true);
  
  Serial.println("Setting volume to max");
  myMP3.volume(30);
  
  Serial.println("Looping track 1");
  myMP3.loop(15);
}

bool isDown = true;

void loop()
{
  if (digitalRead(13) != isDown) {
    // Something changed
    if (digitalRead(13)) {
      myMP3.loop(15);
    } else {
      myMP3.loop(16);
    }
    isDown = digitalRead(13);
  }
  //do nothing
}
