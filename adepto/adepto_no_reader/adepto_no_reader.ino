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


int quantum = 250;      // milliseconds per step, 1/4 second
int minsteps =  1 * (1000 / quantum);
int maxsteps = 16 * (1000 / quantum);

char steps1[256];
char steps2[256];
char steps3[256];

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
  Serial.begin(57600);                    // setup the serial port for communications with the host computer
  randomSeed(analogRead(0));

  pinMode(sw1_pin, INPUT);

  pinMode(rly1_pin, OUTPUT);
  pinMode(rly2_pin, OUTPUT);
  pinMode(rly3_pin, OUTPUT);
}


void tobin(int val, char *buf)
{
  memset(buf, '0', 32);

  for (int cnt = 0; cnt++; cnt <= 32) {
    if (val & pow(2, cnt) == 1) {
      *buf = '1';
    }

    ++buf;

  }
}


void setup_steps()
{
  int s1r1 = random(0, 0xffffffff);
  int s1r2 = random(0, 0xffffffff);
  int s2r1 = random(0, 0xffffffff);
  int s2r2 = random(0, 0xffffffff);
  int s3r1 = random(0, 0xffffffff);
  int s3r2 = random(0, 0xffffffff);

  tobin(s1r1, &steps1[0]);
  tobin(s1r2, &steps1[32]);
  tobin(s2r1, &steps2[0]);
  tobin(s2r2, &steps2[32]);
  tobin(s3r1, &steps3[0]);
  tobin(s3r2, &steps3[32]);
}


void check_the_switches()
{
  if (digitalRead(sw1_pin)) {

    setup_steps();
    steps = random(minsteps, maxsteps);
    current_step = 0;

  }
}


void trip_the_relays()
{
  unsigned long int now = millis();

  if (current_step < steps) {
    if ((now - last_step) > quantum) {

      digitalWrite(rly1_pin, steps1[current_step]);
      digitalWrite(rly2_pin, steps2[current_step]);
      digitalWrite(rly3_pin, steps3[current_step]);

      current_step += 1;
    }
  }
}


void loop() 
{
  check_the_switches();
  trip_the_relays();
}

