#include <afstandssensor.h>

                                        // AfstandsSensor(triggerPin, echoPin);
AfstandsSensor afstandssensor(13, 12);  // Starter afstandssensoren på ben 13 og 12.

void setup () {

    pinMode(4 , OUTPUT); // A
    pinMode(5 , OUTPUT); // B
    pinMode(6 , OUTPUT); // C
    pinMode(7 , OUTPUT); // D

    digitalWrite(4, LOW);
    digitalWrite(5, HIGH);

    digitalWrite(6, LOW);
    digitalWrite(7, HIGH);

    Serial.begin(9600);  // Opsætter serial kommunikation tilbage til computeren
}

float afst;

void loop () {
    // Måler afstanden for hver 500ms

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

    delay(20);
}
