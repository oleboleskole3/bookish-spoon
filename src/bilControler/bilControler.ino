#include <Wire.h>
#include <afstandssensor.h>
#include <Servo.h>

                                        // AfstandsSensor(triggerPin, echoPin);
AfstandsSensor afstandssensor(13, 12);  // Starter afstandssensoren på ben 13 og 12.

Servo myservo;

void setup () {

    myservo.attach(9);

    pinMode(4 , OUTPUT); // A
    pinMode(5 , OUTPUT); // B
    pinMode(6 , OUTPUT); // C
    pinMode(7 , OUTPUT); // D

    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);

    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);

    Serial.begin(9600);  // Opsætter serial kommunikation tilbage til computeren

    Serial.println("iyuib4");

    Wire.begin();
}

float afst;

struct TransitStruct {
  uint8_t throttle;
  uint8_t steer;
};

TransitStruct data;

void loop () {

    Wire.requestFrom(12,sizeof(data));
    delay(10);

    int i = 0;

    while(Wire.available()){

        byte b = Wire.read();
        ((byte*)&data)[i] = b;

        Serial.println(b);

        i++;


    }

    int angle = int( float(data.steer) * (90./255.) );

    myservo.write( angle );

    // Måler afstanden for hver 20ms

    afst = afstandssensor.afstandCM();

    if(afst > 0){
        Serial.println(afst);

        if(afst < 50){
            if(afst > 10){
                analogWrite(4, -afst*(255./25.));
                analogWrite(6, -afst*(255./25.));
            }else{
                digitalWrite(4, HIGH);
                digitalWrite(6, HIGH);
            }
        }else{
            digitalWrite(4, LOW);
            digitalWrite(6, LOW);
        }
    }

    Serial.println();
    Serial.print("Steer " + data.steer);
    Serial.println();
    Serial.print("Throttle " + data.throttle);

    delay(20);
}
