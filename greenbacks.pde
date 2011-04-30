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


int dollar = 0;
unsigned long last_change = millis();
int pulses = 0;


void setup()
{
  Serial.begin(57600);                    // setup the serial port for communications with the host computer
  attachInterrupt(0, count_pulses, CHANGE);
}


void count_pulses()
{
  int val = digitalRead(2);

  if (val == HIGH) {
    last_change = millis();
    pulses += 1;
  }
}


void loop() 
{
  unsigned long int now = millis();

  if (now - last_change > 500) { // no pulses for more than 1/2 second

    if (pulses > 1 && pulses < 20) {            // $1
      dollar = 1;
    } else {

      if (pulses > 20 && pulses < 65) {         // $5
        dollar = 5;
      } else {

        if (pulses > 65 && pulses < 150) {      // $10
          dollar = 10;
        } else {

          if (pulses > 150 && pulses < 250) {   // $20
            dollar = 20;
          }
        }
      }
    }

    pulses = 0;
  }

  Serial.print("apex - dollar: ");
  Serial.print(dollar);
  Serial.print(" pulses: ");
  Serial.print(pulses);
  Serial.print(" last pulse: ");
  Serial.println(last_change);

  delay(500);
}

