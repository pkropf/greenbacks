// Copyright (c) 2012 Peter Kropf. All rights reserved.
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.


// Determine how much money was read by a Pyramid Technologies APEX-5400 series 
// bill acceptor.

// The bill acceptor should be configured to communicate by sending 10 pulses
// for every $1. The pulses should be set to fast - 50ms on, 50ms off.


int quantum = 62;      // milliseconds per step, 1/4 second

int s1steps = 5;        // $1 - steps 4, 1 second
char s1s1[] = "00010";
char s1s2[] = "10000";
char s1s3[] = "00010";

int s5steps = 12;       // $5 - 3 seconds
char s5s1[] = "100100000000";
char s5s2[] = "000010010000";
char s5s3[] = "000000001001";

int s10steps = 36;       // $10 - 12 seconds
char s10s1[] = "100000001000000010000000000000001000";
char s10s2[] = "000010000000100000001000000010000000";
char s10s3[] = "100000001000000000000000100000000000";

int s20steps = 92;       // $20 - 23 seconds
char s20s1[] = "11110000111100000000000000000000111100001111000011110000000000000000000000000000111100001111";
char s20s2[] = "11110000000000001111000011110000000000001111000000000000111100000000000011110000000000001111";
char s20s3[] = "11110000111100000000000000000000111100001111000000000000000000001111000000000000000000001111";

char *steps1;
char *steps2;
char *steps3;
int steps = 0;                         // number of steps to work through
int current_step = 0;                  // number of steps left
unsigned long last_step = millis();    // when was the last time a step was executed

int sw1_pin = 3;                       // input switch 1

int rly1_pin = 9;                      // relay 1
int rly2_pin = 10;                     // relay 2
int rly3_pin = 11;                     // relay 3

unsigned long last_change = millis();  // when was the last time a pulse was received?

int checked = true;


void setup()
{
  //Serial.begin(57600);                    // setup the //Serial port for communications with the host computer
  //Serial.println("setup");

  randomSeed(analogRead(0));

  pinMode(sw1_pin, INPUT);

  pinMode(rly1_pin, OUTPUT);
  pinMode(rly2_pin, OUTPUT);
  pinMode(rly3_pin, OUTPUT);
}


void setup_steps()
{
  switch (random(0, 4)) {
  case 0:
    //Serial.println("random 0");
    steps  = s1steps;
    steps1 = s1s1;
    steps2 = s1s2;
    steps3 = s1s3;
    break;
  
  case 1:
    //Serial.println("random 1");
    steps  = s5steps;
    steps1 = s5s1;
    steps2 = s5s2;
    steps3 = s5s3;
    break;
  
  case 2:
    //Serial.println("random 2");
    steps  = s10steps;
    steps1 = s10s1;
    steps2 = s10s2;
    steps3 = s10s3;
    break;
  
  case 3:
    //Serial.println("random 3");
    steps  = s20steps;
    steps1 = s20s1;
    steps2 = s20s2;
    steps3 = s20s3;
    break;

  }
}


void check_the_switches()
{
  if (digitalRead(sw1_pin) == LOW) {

    //Serial.println("switch lo");
    setup_steps();
    current_step = 0;

  }
}


void trip(int solenoid, char note)
{
  if (note == '0') {
    digitalWrite(solenoid, LOW);
  }
  else {
    if (note == '1') {
      digitalWrite(solenoid, HIGH);
    }
  }
}


void trip_the_relays()
{
  unsigned long int now = millis();

  if (current_step < steps) {
    if ((now - last_step) > quantum) {

      //Serial.println("tripping");
      //Serial.println(steps1[current_step]);
      //Serial.println(steps2[current_step]);
      //Serial.println(steps3[current_step]);

      trip(rly1_pin, steps1[current_step]);
      trip(rly2_pin, steps2[current_step]);
      trip(rly3_pin, steps3[current_step]);

      current_step += 1;
      last_step = now;

    }
  } else {
  
    trip(rly1_pin, '0');
    trip(rly2_pin, '0');
    trip(rly3_pin, '0');

  }
}


void loop() 
{
  check_the_switches();
  trip_the_relays();
}

