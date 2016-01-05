/*
 * -----------------------------------------------------------------
 * Beagle Food Regulator
 * -----------------------------------------------------------------
 * @author Scott Lerner
 * @license Released into the public domain, please refer to <http://unlicense.org/>
 * 
 * Motivation:
 * My girlfriend's dog, Flint, likes to beg for food... a lot. This
 * can be detrimental for his weight if a family member feeds him
 * extra food. Flint is supposed to be fed twice a day; once in the 
 * morning and once in the evening. A perfect application for the
 * Arduino.
 * 
 * Hardware:
 * 1x Arduino Uno
 * 2x Buttons
 * 2x RGB LEDs
 * 2x 1k Resistors
 * 
 * Operation:
 * This code operates as a state machine that uses the Arduino's
 * internal millisecond counter to keep track of Flint's feeding
 * times. The LEDs represent whether Flint should be fed. Once the
 * food has been given, the corresponding button is pressed to 
 * advance the state machine.
 */

 /*
 * Flow chart
 * -Power Up (state 0):
 *    -> LEDs both GREEN
 *    -> Wait for Button 1
 * -Breakfast is served (state 1):
 *    -> Button 1 is pressed (assume 6am)
 *    -> LED1 from GREEN to RED
 *    -> Start keeping track of 10 hour timer until dinner
 * -Dinner is served (state 2):
 *    -> Button 2 is pressed (assume 6pm)
 *    -> LED2 from GREEN to RED
 *    -> Start a timer to turn off the LEDs for the night
 * -Night time:
 *    -> Wait until day resets
 */

//LED for Breakfast
const int redPin1 = 9;
const int bluePin1 = 11;
const int greenPin1 = 10;
//LED for Dinner
const int redPin2 = 3;
const int bluePin2 = 5;
const int greenPin2 = 6;
//Buttons for Breakfast, Dinner
const int button1 = 2;
const int button2 = 4;
int button1state = 0;
int button2state = 0;

//Time Unit: Seconds
long unsigned const int HOUR = 3600;

//State machine state
int state = 0;
//State machine for flashing LED
int ledState;
//Keeping track of an interval of time
long unsigned int start = 0;
long unsigned int nowTime = 0;

long unsigned int waitUntilDinner = 12*HOUR; //6am to 6pm
long unsigned int waitLightsOff = 4*HOUR; //6pm to 10pm
long unsigned int waitUntilMorning = waitLightsOff+(5*HOUR); //10pm to 3am

void setup(){
  Serial.begin(9600); //set baud rate
  pinMode(button1, INPUT);
  pinMode(button2, INPUT);
}

void loop(){
  if (state == 0) { //Power-Up
    setColor1(0,255,0); //LED1 Green
    setColor2(0,255,0); //LED2 Green
    button1state = digitalRead(button1);
    if (button1state == HIGH) {
      setColor1(255,0,0); //LED1 Red
      start = millis()/1000;
      state = 1;
    }
  } else if (state == 1) { //Breakfast
    nowTime = millis()/1000;
    //Serial.print("Breakfast Elapsed: ");
    //Serial.println(nowTime - start);
    if (nowTime - start > waitUntilDinner) {
      //Serial.print("Wait time up.");
      ledState = HIGH;
      while (1) {
        flashColor2(0,255,0,ledState);
        ledState = !ledState;
        button2state = digitalRead(button2);
        if (button2state == HIGH) {
          setColor2(255,0,0);
          break;
        }
        delay(500);
      }
    }
    button2state = digitalRead(button2);
    if (button2state == HIGH) {
      setColor2(255,0,0);
      state = 2;
      start = millis()/1000;
    }
  } else if (state == 2) { //Dinner
    nowTime = millis()/1000;
    //Serial.print("Dinner Elapsed: ");
    //Serial.println(nowTime-start);
    if (nowTime - start < waitLightsOff) {
      setColor1(255,0,0);
      setColor2(255,0,0);
    } else if (nowTime - start < waitUntilMorning) {
      setColor1(0,0,0);
      setColor2(0,0,0);
    } else {
      Serial.println("Reset State");
      state = 0;
    }
  }
  delay(100);
}

void setColor1(int red, int green, int blue) {
  analogWrite(redPin1, red); analogWrite(greenPin1, green); analogWrite(bluePin1, blue);
}
void setColor2(int red, int green, int blue) {
  analogWrite(redPin2, red); analogWrite(greenPin2, green); analogWrite(bluePin2, blue);
}
void flashColor1(int red, int green, int blue, int mystate) {
  if (mystate) {
    analogWrite(redPin1, red); analogWrite(greenPin1, green); analogWrite(bluePin1, blue);
  } else {
    analogWrite(redPin1, 0); analogWrite(greenPin1, 0); analogWrite(bluePin1, 0);
  }
}
void flashColor2(int red, int green, int blue, int mystate) {
  if (mystate) {
    analogWrite(redPin2, red); analogWrite(greenPin2, green); analogWrite(bluePin2, blue);
  } else {
    analogWrite(redPin2, 0); analogWrite(greenPin2, 0); analogWrite(bluePin2, 0);
  }
}
