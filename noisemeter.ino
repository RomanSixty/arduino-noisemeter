/**
 * LX' noisemeter
 * written by LX in 2018
 * for Arduino/Genuino Nano
 * 
 * a noise indicator using 10 colored LEDs from an Adafruit LED strip
 * self calibrating by using running averages
 * (Library by Rob Tillaart)
 * 
 * BSD license, all text above must be included in any redistribution.
 */

#include <Adafruit_NeoPixel.h>
#include "RunningAverage.h"

#define NUMPIXELS 10
#define LEDPIN     8
#define MIC       A6

// estimation of largest amplitude
// based on measurements

#define max_amplitude 20

Adafruit_NeoPixel pixels = Adafruit_NeoPixel ( NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800 );

RunningAverage readings(100);

void setup()
{
  pinMode ( LEDPIN, OUTPUT );
  pixels.begin();
  //Serial.begin(115200);
}

void loop()
{
  int sensorReading = analogRead ( MIC );

  // reading a long term average to get the current calibration
  // difference between this average and the current reading
  // is the amplitude we're trying to visualize

  readings.addValue ( sensorReading );
  int median = readings.getAverage();

  int amplitude = abs ( sensorReading - median );
  
/*
  Serial.print(sensorReading);
  Serial.print(" ");
  Serial.print(amplitude);
  Serial.print(" ");
  Serial.println(median);
*/

  byte shine      = map ( amplitude, 0, max_amplitude, 0, NUMPIXELS       ); // how many LEDs are turned on at all
  int  brightness = map ( amplitude, 0, max_amplitude, 0, NUMPIXELS * 255 ); // combined brightness values of all LEDs

  for ( byte i = 0; i < NUMPIXELS; i++ )
  {
    byte curr_brightness = min ( 255, brightness );
    brightness         -= curr_brightness;

    // the first LED should always be on
    // otherwise its constant flicker will drive me nuts

    if ( i == 0 ) curr_brightness = 80;

    // three color channels
    // the first LED is pure green, the last one pure red
    // intensity for the green channel is reduced because that's a very bright color
    
    byte color_r = map ( i, 0, NUMPIXELS,                   0, curr_brightness );
    byte color_g = map ( i, 0, NUMPIXELS, curr_brightness / 3,               0 );
    byte color_b = 0;
    
    if ( i <= shine )
      pixels.setPixelColor ( i, pixels.Color ( color_r, color_g, color_b ) );
    else
      pixels.setPixelColor ( i, pixels.Color ( 0, 0, 0 ) );
  }

  pixels.show();
}
