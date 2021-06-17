#include <Wire.h>
#include <LSM303.h>
#include <Servo.h>

Servo myservo;  // create servo object to control a servo  

LSM303 compass;

  float targetHeading = 0;
  float headingError = 0;
  int absoluteVal = 0;
  float correctedError = 0;
  float smoothedHeading = 0;
  float centrePoint = 86;
  float servoOutput = 0;
  int servoPin = 9; grey
  float gain = 15;
  int smoothing = 50;
  int setHeading = 1;
  int on = 1;
  long nextL = 0;
  long nextR = 0;
  long now = 0;

void setup() {
  
  myservo.attach(9);
  Serial.begin(9600);
  Wire.begin();
  compass.init();
  compass.enableDefault();
  pinMode(8,INPUT_PULLUP); //start working -Orange (red)
  pinMode(7,INPUT_PULLUP); //tweak left -Purple (white)
  pinMode(6,INPUT_PULLUP); //tweak right -Blue (green)
    
  /*
  Calibration values; the default values of +/-32767 for each axis
  lead to an assumed magnetometer bias of 0. Use the Calibrate example
  program to determine appropriate values for your particular unit.
  */
  compass.m_min = (LSM303::vector<int16_t>){-32767, -32767, -32767};
  compass.m_max = (LSM303::vector<int16_t>){+32767, +32767, +32767};
}

void loop() {
  //Serial.println(on);
  compass.read();
  
  int on = digitalRead(8); //start working
  int tweakL = digitalRead(6); //tweak left
  int tweakR = digitalRead(7); //tweak right

  float heading = compass.heading();
  smoothedHeading = (smoothedHeading * smoothing + heading)/(smoothing + 1);
  Serial.println(smoothedHeading);

if (on == 0){// if its on then run the compass

  if (setHeading == 1){          //If this is the first time through since the switch was turned on
    targetHeading = smoothedHeading;
    setHeading = 0;
  }
  
  /*
  When given no arguments, the heading() function returns the angular
  difference in the horizontal plane between a default vector and
  north, in degrees.
  
  The default vector is chosen by the library to point along the
  surface of the PCB, in the direction of the top of the text on the
  silkscreen. This is the +X axis on the Pololu LSM303D carrier and
  the -Y axis on the Pololu LSM303DLHC, LSM303DLM, and LSM303DLH
  carriers.
  
  To use a different vector as a reference, use the version of heading()
  that takes a vector argument; for example, use
  
    compass.heading((LSM303::vector<int>){0, 0, 1});
  
  to use the +Z axis as a reference.
  */
  now = millis();
  
  if (tweakL == 0){
    if (now > nextL){
      --targetHeading;
      if (targetHeading < 0){
        targetHeading = 359;
      }
    nextL = now + 500;  
    }
  }

  if (tweakR == 0){
    if (now > nextR){
      ++targetHeading;
      if (targetHeading > 359){
        targetHeading = 0;
      }
    nextR = now + 500;  
    }
  }
  
  headingError = 0 - targetHeading + smoothedHeading;
  
  if (headingError < -180){
    correctedError = headingError + 360;
  }
  if (headingError > 180){
    correctedError = headingError - 360;
  }
  absoluteVal = abs(headingError);
  if (absoluteVal <= 180){
    correctedError = headingError;
  }

    
  servoOutput = centrePoint + correctedError*gain;
  
    if (servoOutput >= 140){
      servoOutput = 140;
    }
    if (servoOutput <= 40){
    servoOutput = 40;
    }
    
}

if (on == 1){// if its off then centre servo
  servoOutput = centrePoint;
  setHeading = 1;
}

//Serial.println(servoOutput);
myservo.write(servoOutput);

}
