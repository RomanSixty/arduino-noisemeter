#include <Adafruit_NeoPixel.h>
#include "RunningAverage.h"

#define NUMPIXELS 10
#define LEDPIN     8
#define MIC       A6

Adafruit_NeoPixel pixels = Adafruit_NeoPixel ( NUMPIXELS, LEDPIN, NEO_GRB + NEO_KHZ800 );

RunningAverage readings(100);

// größte Amplitude in etwa
// aus Messwerten geschätzt
int max_amplitude   = 20;

void setup()
{
  pinMode ( LEDPIN, OUTPUT );
  pixels.begin();
  //Serial.begin(115200);
}

void loop()
{
  int sensorReading = analogRead ( MIC );

  // wir berechnen den Durchschnitt, um aus der Differenz zum
  // aktuellen Messwert die Amplitude zu ermitteln

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

  int shine      = map ( amplitude, 0, max_amplitude, 0, NUMPIXELS       ); // "LED an" vs. "LED aus"
  int brightness = map ( amplitude, 0, max_amplitude, 0, NUMPIXELS * 255 ); // Helligkeit jeder LED

  for ( int i = 0; i < NUMPIXELS; i++ )
  {
    int curr_brightness = min ( 255, brightness );
    brightness         -= curr_brightness;

    // das erste Licht soll immer an sein, das Flackern nervt sonst
    if ( i == 0 ) curr_brightness = 80;

    // die drei Farbkanäle
    // die erste LED soll Grün, die letzte Rot sein
    // Grün machen wir um ein Drittel weniger intensiv, weil das sehr hell ist
    // Blauanteile haben wir keine
    
    int color_r = map ( i, 0, NUMPIXELS,                   0, curr_brightness );
    int color_g = map ( i, 0, NUMPIXELS, curr_brightness / 3,               0 );
    int color_b = 0;
    
    if ( i <= shine )
      pixels.setPixelColor ( i, pixels.Color ( color_r, color_g, color_b ) );
    else
      pixels.setPixelColor ( i, pixels.Color ( 0, 0, 0 ) );
  }

  pixels.show();
}
