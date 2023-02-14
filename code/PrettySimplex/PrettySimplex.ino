// Based on a file called Prettyfill 
// Morphed with other files 
// Im learning... 

#include <FastLED.h>

#define NUM_LEDS            144
#define LED_PIN             5
#define LED_PIN2            6
#define LED_TYPE            WS2812B
#define COLOR_ORDER         GRB


CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];


  struct rgb_t
   {
      unsigned char   red;
      unsigned char green;
      unsigned char  blue;
   };

CRGBPalette16 currentPalette;
TBlendType    currentBlending;


void setup() {
  delay( 3000); // 3 second delay for boot recovery, and a moment of silence
  FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
 FastLED.addLeds<LED_TYPE, LED_PIN2, COLOR_ORDER>(leds2, NUM_LEDS);
  FastLED.setBrightness(100);
  Serial.begin(9600);
SetupPurpleAndGreenPalette();
  //SetupTotallyRandomPalette();
  

}

void loop() {
 for (uint8_t octaves = 0; octaves < 6 ; octaves++)
  {
 
// right side 
 // uint8_t octaves = 4;
  uint16_t x = 0;  // shifts leds 
  int scale = 100;
  uint8_t hue_octaves = 1;
  uint16_t hue_x = 2;
  int hue_scale = 5; // segments more colors
  uint16_t ntime = millis() / 24;
  uint8_t hue_shift = 2;
  
  fill_noise16 (leds, NUM_LEDS, octaves, x, scale, hue_octaves, hue_x, hue_scale, ntime, hue_shift);
   
   // left side
  -- octaves = 4;
   x = 0;
   scale = 100;
   hue_octaves = 1;
   hue_x = 2;
   hue_scale = 5;
   ntime = millis() / 12;
   hue_shift = 2;
   hue_shift = octaves-2;

   fill_noise16 (leds2, NUM_LEDS, octaves, x, scale, hue_octaves, hue_x, hue_scale, ntime, hue_shift);
   FastLED.show();
  }
}
// This function fills the palette with totally random colors.
void SetupTotallyRandomPalette()
{
    for( int i = 0; i < 16; ++i) {
        currentPalette[i] = CHSV( random8(), 255, random8());
    }
}
// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    CRGB yellow  = CRGB::Yellow;
       CRGB orange  = CRGB::Orange;
           CRGB white  = CRGB::White;
    
    currentPalette = CRGBPalette16(
                                   green,  white,  black,  black,
                                   yellow, orange, black,  black,
                                   green,  green,  black,  black,
                                   yellow, orange, black,  black );
}

void convert_wave_length_nm_to_rgb(const double wave_length_nm)
{
   // Credits: Dan Bruton http://www.physics.sfasu.edu/astro/color.html
   double red   = 0.0;
   double green = 0.0;
   double blue  = 0.0;

   if ((380.0 <= wave_length_nm) && (wave_length_nm <= 439.0))
   {
      red   = -(wave_length_nm - 440.0) / (440.0 - 380.0);
      green = 0.0;
      blue  = 1.0;
   }
   else if ((440.0 <= wave_length_nm) && (wave_length_nm <= 489.0))
   {
      red   = 0.0;
      green = (wave_length_nm - 440.0) / (490.0 - 440.0);
      blue  = 1.0;
   }
   else if ((490.0 <= wave_length_nm) && (wave_length_nm <= 509.0))
   {
      red   = 0.0;
      green = 1.0;
      blue  = -(wave_length_nm - 510.0) / (510.0 - 490.0);
   }
   else if ((510.0 <= wave_length_nm) && (wave_length_nm <= 579.0))
   {
      red   = (wave_length_nm - 510.0) / (580.0 - 510.0);
      green = 1.0;
      blue  = 0.0;
   }
   else if ((580.0 <= wave_length_nm) && (wave_length_nm <= 644.0))
   {
      red   = 1.0;
      green = -(wave_length_nm - 645.0) / (645.0 - 580.0);
      blue  = 0.0;
   }
   else if ((645.0 <= wave_length_nm) && (wave_length_nm <= 780.0))
   {
      red   = 1.0;
      green = 0.0;
      blue  = 0.0;
   }

   double factor = 0.0;

   if ((380.0 <= wave_length_nm) && (wave_length_nm <= 419.0))
      factor = 0.3 + 0.7 * (wave_length_nm - 380.0) / (420.0 - 380.0);
   else if ((420.0 <= wave_length_nm) && (wave_length_nm <= 700.0))
      factor = 1.0;
   else if ((701.0 <= wave_length_nm) && (wave_length_nm <= 780.0))
      factor = 0.3 + 0.7 * (780.0 - wave_length_nm) / (780.0 - 700.0);
   else
      factor = 0.0;

   rgb_t result;

   const double gamma         =   0.8;
   const double intensity_max = 255.0;

 
   result.red   = static_cast<unsigned char>((red   == 0.0) ? red   : round(intensity_max * pow(red   * factor, gamma)));
   result.green = static_cast<unsigned char>((green == 0.0) ? green : round(intensity_max * pow(green * factor, gamma)));
   result.blue  = static_cast<unsigned char>((blue  == 0.0) ? blue  : round(intensity_max * pow(blue  * factor, gamma)));

   #undef round

   return result;
}

long clamp(long x1)
{
    long  minimum=0.0;
    long  maximum=1.0;
    return max(minimum, min(x1, maximum));
}


// This example shows how to set up a static color palette
// which is stored in PROGMEM (flash), which is almost always more
// plentiful than RAM.  A static PROGMEM palette like this
// takes up 64 bytes of flash.
const TProgmemPalette16 myRedWhiteBluePalette_p PROGMEM =
{
    CRGB::Red,
    CRGB::Gray, // 'white' is too bright compared to red and blue
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Black,
    
    CRGB::Red,
    CRGB::Red,
    CRGB::Gray,
    CRGB::Gray,
    CRGB::Blue,
    CRGB::Blue,
    CRGB::Black,
    CRGB::Black
};

