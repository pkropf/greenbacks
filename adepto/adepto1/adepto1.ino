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


int quantum = 250;      // milliseconds between steps, 1/4 second

int s1steps = 4;        // $1 - steps 4, 1 second
char s1s1[] = "0000";
char s1s2[] = "1111";
char s1s3[] = "0000";

int s5steps = 12;       // $5 - 3 seconds
char s5s1[] = "111100000000";
char s5s2[] = "000011110000";
char s5s3[] = "000000001111";

int s10steps = 36;       // $10 - 12 seconds
char s10s1[] = "111100001111000011110000000000001111";
char s10s2[] = "000011110000111100001111000011110000";
char s10s3[] = "111100001111000000000000111100000000";

int s20steps = 92;       // $20 - 23 seconds
char s20s1[] = "11110000111100000000000000000000111100001111000011110000000000000000000000000000111100001111";
char s20s2[] = "11110000000000001111000011110000000000001111000000000000111100000000000011110000000000001111";
char s20s3[] = "11110000111100000000000000000000111100001111000000000000000000001111000000000000000000001111";

int steps = 0;                         // number of steps to work through
int current_step = 0;                  // number of steps left
char *s1 = 0;                          // sequence string 1
char *s2 = 0;                          // sequence string 2
char *s3 = 0;                          // sequence string 3
unsigned long last_step = millis();    // when was the last time a step was executed

int sw1_pin = 3;                       // input switch 1
int sw2_pin = 4;                       // input switch 2
int sw3_pin = 5;                       // input switch 3

int sw1_active = false;                // check if switch 1 is pushed
int sw2_active = false;                // check if switch 2 is pushed
int sw3_active = false;                // check if switch 3 is pushed

int lt1_pin = 6;                       // switch light 1
int lt2_pin = 7;                       // switch light 2
int lt3_pin = 8;                       // switch light 3

int rly1_pin = 9;                      // relay 1
int rly2_pin = 10;                     // relay 2
int rly3_pin = 11;                     // relay 3

int apex_pin = 2;                      // what pin is the apex bill reader connect to?
int apex_interrupt = 0;                // interrupt to use when sensing a pulse

unsigned long last_change = millis();  // when was the last time a pulse was received?

int dollar = 0;                        // what is the dollar amount that was read?
int pulses = 0;                        // counting the pulses sent
int pulses_per_dollar = 1;             // how many pulses are sent per dollar
int done_pulsing = 20 * pulses_per_dollar * 100;
                                       // how many milliseconds after the last pulse to 
                                       // consider the bill reading done

int led_pin = 13;
int checked = true;


void setup()
{
  Serial.begin(57600);                    // setup the serial port for communications with the host computer
  attachInterrupt(apex_interrupt, count_pulses, CHANGE);
  pinMode(led_pin, OUTPUT);

  pinMode(sw1_pin, INPUT);
  pinMode(sw2_pin, INPUT);
  pinMode(sw3_pin, INPUT);

  pinMode(lt1_pin, OUTPUT);
  pinMode(lt2_pin, OUTPUT);
  pinMode(lt3_pin, OUTPUT);

  pinMode(rly1_pin, OUTPUT);
  pinMode(rly2_pin, OUTPUT);
  pinMode(rly3_pin, OUTPUT);
}


void count_pulses()
{
  int val = digitalRead(apex_pin);
  checked = false;

  if (val == HIGH) {
    last_change = millis();
    pulses += 1;
  }
}


void count_the_money()
{
  unsigned long int now = millis();

  if (((now - last_change) > done_pulsing) && ! checked) { // no pulses for more than 1/10 second

    if (pulses == 1 * pulses_per_dollar) {           // $1
      dollar = 1;
      sw2_active = true;

    } else {

      if (pulses == 5 * pulses_per_dollar) {         // $5
        dollar = 5;
        sw2_active = true;

      } else {

        if (pulses == 10 * pulses_per_dollar) {      // $10
          dollar = 10;
          sw2_active = true;

        } else {

          if (pulses == 20 * pulses_per_dollar) {    // $20
            dollar = 20;
            sw2_active = true;
          }
        }
      }
    }

    pulses = 0;
    checked = true;
  }
}


void lights()
{
  digitalWrite(lt1_pin, sw1_active);
  digitalWrite(lt2_pin, sw2_active);
  digitalWrite(lt3_pin, sw3_active);
}


void check_the_switches()
{
  if (sw2_active) {
    if (digitalRead(sw2_pin)) {
      if (dollar == 1) {

        steps = s1steps;
        current_step = 0;
        s1 = s1s1;
        s2 = s1s2;
        s3 = s1s3;

      } else {
        if (dollar == 5) {
          
          steps = s5steps;
          current_step = 0;
          s1 = s5s1;
          s2 = s5s2;
          s3 = s5s3;

        } else {
          if (dollar == 10) {
            
            steps = s10steps;
            current_step = 0;
            s1 = s10s1;
            s2 = s10s2;
            s3 = s10s3;

          } else {
            if (dollar == 20) {
              
              steps = s20steps;
              current_step = 0;
              s1 = s20s1;
              s2 = s20s2;
              s3 = s20s3;

            }
          }
        }
      }
      dollar = 0;
    }
  }
}


void trip_the_relays()
{
  unsigned long int now = millis();

  if (current_step < steps) {
    if ((now - last_step) > quantum) { // no pulses for more than 1/10 second

      digitalWrite(rly1_pin, s1[current_step]);
      digitalWrite(rly2_pin, s2[current_step]);
      digitalWrite(rly3_pin, s3[current_step]);

      current_step += 1;
    }
  } else {

    sw2_active = false;
  }
}


void loop() 
{
  count_the_money();
  lights();
  check_the_switches();
  trip_the_relays();
}

