// Copyright (c) 2011 Peter Kropf. All rights reserved.
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


// Interface with a Pyramid Technologies APEX-5400 series bill acceptor.
//
// The bill acceptor should be configured to communicate by sending 10 pulses
// for every $1. The pulses should be set to fast - 50ms on, 50ms off.


int apex_pin = 2;                      // what pin is the apex bill reader connect to?
int apex_interrupt = 0;                // interrupt to use when sensing a pulse
int dollar = 0;                        // what is the dollar amount that was read?
unsigned long last_change = millis();  // when was the last time a pulse was received?
int pulses = 0;                        // counting the pulses sent
int pulses_per_dollar = 1;            // how many pulses are sent per dollar
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


void loop() 
{
  unsigned long int now = millis();

  if (((now - last_change) > done_pulsing) && ! checked) { // no pulses for more than 1/10 second

    if (pulses == 1) {            // $1
      dollar = 1;
    } else {

      if (pulses == 5) {         // $5
        dollar = 5;
      } else {

        if (pulses == 10) {      // $10
          dollar = 10;
        } else {

          if (pulses == 20) {   // $20
            dollar = 20;
          }
        }
      }
    }

    Serial.print("dollar: ");
    Serial.print(dollar);
    Serial.print(" pulses: ");
    Serial.print(pulses);
    Serial.print(" last pulse: ");
    Serial.println(now - last_change);

    pulses = 0;
    dollar = 0;
    checked = true;

  }
}

