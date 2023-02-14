/* 

Code borrowed from different libraries 

   -- credit will be given once i get it all together
Xavier Bury 02/2023

*/

//#include <LiquidCrystal.h>
//char = "version 2.2"
#include <WiFi.h>
const char* ssid     = "*****";
const char* password = "*****";


#include <IRac.h>
#include <IRmacros.h>
#include <IRrecv.h>
#include <IRremoteESP8266.h>
#include <IRsend.h>
#include <IRtext.h>
#include <IRtimer.h>
#include <IRutils.h>
#include <i18n.h>


#define RECEIVER 13

#define KEY_POWER (0xFFA25D)
#define KEY_FUNC_STOP (0xFFE21D)
#define KEY_VOL_ADD (0xFF629D)
#define KEY_FAST_BACK (0xFF22DD)
#define KEY_PAUSE (0xFF02FD)
#define KEY_FAST_FORWARD (0xFFC23D)
#define KEY_DOWN (0xFFE01F)
#define KEY_VOL_DE (0xFFA857)
#define KEY_UP (0xFF906F)
#define KEY_EQ (0xFF9867)
#define KEY_ST_REPT (0xFFB04F)
#define KEY_0 (0xFF6897)
#define KEY_1 (0xFF30CF)
#define KEY_2 (0xFF18E7)
#define KEY_3 (0xFF7A85)
#define KEY_4 (0xFF10EF)
#define KEY_5 (0xFF38C7)
#define KEY_6 (0xFF5AA5)
#define KEY_7 (0xFF42BD)
#define KEY_8 (0xFF4AB5)
#define KEY_9 (0xFF52AD)
#define KEY_REPEAT (0xFFFFFFFF)
#define KEY_NUM 21
#define REPEAT 22

unsigned long keyValue[]={KEY_POWER,KEY_FUNC_STOP,KEY_VOL_ADD,KEY_FAST_BACK,KEY_PAUSE,KEY_FAST_FORWARD,
                KEY_DOWN,KEY_VOL_DE,KEY_UP,KEY_EQ,KEY_ST_REPT,KEY_0,KEY_1,KEY_2,KEY_3,KEY_4,KEY_5,
                KEY_6,KEY_7,KEY_8,KEY_9,KEY_REPEAT};

char keyBuf[][15]={"POWER","FUNC/STOP","VOL+","FAST BACK","PAUSE","FAST FORWARD","DOWN","VOL-",
                  "UP","EQ","ST/REPT","0","1","2","3","4","5","6","7","8","9"};

#include <FastLED.h>

#define LED_PIN     12
#define LED_PIN2    14
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB


#define BRIGHTNESS  50
#define FPS 120


// Param for different pixel layouts
const bool    kMatrixSerpentineLayout = true;


#define NUM_LEDS 288

// Params for width and height
const uint16_t kMatrixWidth  = NUM_LEDS;
const uint16_t kMatrixHeight = 1;


#define MAX_XDIMENSION NUM_LEDS
#define MAX_YDIMENSION 1



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


// The leds
CRGB leds[NUM_LEDS];
CRGB leds2[NUM_LEDS];

// The 16 bit version of our coordinates
static uint16_t x,x2;
static uint16_t y,y2;
static uint16_t z,z2;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
uint16_t speed = 10; // speed is set dynamically once we've started up

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.
uint16_t scale = 30; // scale is set dynamically once we've started up

// hue shifter - seems to cause flicker.
// might need to change the FPS factor..
uint8_t ihue=1;
uint8_t ihuedelta=1;  


// This is the array that we keep our computed noise values in
uint8_t noise[MAX_XDIMENSION][MAX_YDIMENSION];
uint8_t noise2[MAX_XDIMENSION][MAX_YDIMENSION];
uint8_t  colorLoop = 1;
uint8_t  brightness = 50;
CRGBPalette16 currentPalette( PartyColors_p );

IRrecv irrecv(RECEIVER);     // create instance of 'irrecv'
decode_results results;      // create instance of 'decode_results'

WiFiServer server(80);


 /* The circuit:
 * LCD RS pin to digital pin 7
 * LCD Enable pin to digital pin 8
 * LCD D4 pin to digital pin 9
 * LCD D5 pin to digital pin 10
 * LCD D6 pin to digital pin 11
 * LCD D7 pin to digital pin 12
 * LCD R/W pin to ground
 * LCD VSS pin to ground
 * LCD VCC pin to 5V
 * 10K resistor:
 * ends to +5V and ground
 * wiper to LCD VO pin (pin 3)
*/
//LiquidCrystal lcd(7, 8, 9, 10, 11, 12);

void setup() {

    // set up the LCD's number of columns and rows:
 // lcd.begin(16, 2);
  // Print a message to the LCD.
//  lcd.print("Hello, World!");

  Serial.begin(115200);
  delay(1000);
  Serial.println("IR Receiver Button Decode");
  irrecv.enableIRIn();

  delay(3000);
  FastLED.addLeds<LED_TYPE,LED_PIN,COLOR_ORDER>(leds,NUM_LEDS);
 // FastLED.addLeds<LED_TYPE,LED_PIN2,COLOR_ORDER>(leds2,NUM_LEDS);
  FastLED.addLeds<LED_TYPE,LED_PIN2,COLOR_ORDER>(leds,NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);

  // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();
  x2 = random16();
  y2 = random16();
  z2 = random16();

   Serial.begin(115200);
    pinMode(5, OUTPUT);      // set the LED pin mode

    delay(10);

    // We start by connecting to a WiFi network

    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);

    WiFi.begin(ssid, password);

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }

    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    
    server.begin();

}


// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  if( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }
  
  for(int i = 0; i < MAX_XDIMENSION; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < MAX_YDIMENSION; j++) {
      int joffset = scale * j;
      
      uint8_t data  = inoise8(x + ioffset,y + joffset,z);
      uint8_t data2 = inoise16(x2 + ioffset,y2 + joffset,z);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data,16);
      // data2 = qadd8(data,scale8(data,39));
      data2 = qadd8(data,scale16(data,200));

      if( dataSmoothing ) {
         uint8_t olddata = noise[i][j];
         // uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
         uint8_t newdata = scale16( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
         data = newdata;
         olddata = noise2[i][j];
         newdata = scale16( olddata, dataSmoothing) + scale16( data2, 256 - dataSmoothing);
         data2 = newdata;
     }
       noise[i][j] = data;
       noise2[i][j] = data2;
    }
  }
  
  z += speed;
  
  // apply slow drift to X and Y, just for visual variation.
  x += speed / 16;
  y -= speed / 8;
  
  z2 += speed;
  x2 += speed / 14;
  y2 -= speed / 9;
}

// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8bad() {
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  int dataSmoothing = 0;
  if( speed < 50) {
     dataSmoothing = 200 - (speed * 4);
  }
  
  for(uint16_t i = 0; i < MAX_XDIMENSION; i++) {
    uint8_t ioffset = scale * i;
    for(uint16_t j = 0; j < MAX_YDIMENSION; j++) {
      uint8_t joffset = scale * j;
      
      uint8_t data  = inoise8(x + ioffset,y + joffset,z);
      uint8_t data2 = inoise8(x2 + ioffset,y2 + joffset,z);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
   //  data = qsub8(data,16);
      // data2 = qadd8(data,scale8(data,39));
      //data = qadd8(data,scale16(data,200));
      //data2 = qadd8(data2,scale16(data2,200));

      if( dataSmoothing ) {
         uint8_t olddata = noise[i][j];
         // uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
         uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
         data = newdata;
         olddata = noise2[i][j];
         newdata = scale8( olddata, dataSmoothing) + scale8( data2, 256 - dataSmoothing);
         data2 = newdata;
     }
       noise[i][j] = data;
       noise2[i][j] = data2;
    }
  }
  
  z += speed;
  
  // apply slow drift to X and Y, just for visual variation.
  x += speed / 16;
  y -= speed / 8;
  
  z2 += speed;
  x2 += speed / 14;
  y2 -= speed / 9;
}



void mapNoiseToLEDsUsingPalette()
{

  for(uint16_t i = 0; i < kMatrixWidth; i++) {
    for(uint16_t j = 0; j < kMatrixHeight; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index  = noise[j][i];
      uint8_t index2 = noise2[j][i];
      uint8_t bri    = noise[i][j];
      uint8_t bri2    = noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) { 
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }
      if( bri2 > 127 ) {
        bri2 = 255;
      } else {
        bri2 = dim8_raw( bri2 * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
      CRGB color2 = ColorFromPalette( currentPalette, index2, bri2);
      // if (i+1 % 2 ==1)
      leds[XY(i,j)] = color;
      // else
      leds2[XY(i,j)] = color2;
      // leds2[XY(i,j)] = color2;
    }
  }
  
  if (ihue< 255-ihuedelta) ihue+=ihuedelta;
}


int ledactive = 0;

void loop() {
 

  irRemoteLoop();

  if ( ledactive == 1 )
  {
    FastLEDLOOP();
    }
  else
  {
    FastLEDOFF();
  }
  
  //FastLED.delay(1000 / FPS);

  wifiloop();
} // loop

void FastLEDLOOP(){
  // LED

    // Periodically choose a new palette, speed, and scale
    // ChangePaletteAndSettingsPeriodically();
    //currentPalette = LavaColors_p;            speed =  4; scale = 30; colorLoop = 1;
    // generate noise data
    fillnoise8();
    
    // convert the noise data to colors in the LED array
    // using the current palette
    mapNoiseToLEDsUsingPalette();

    FastLED.show();
    FastLED.delay(1000/FPS);
}

void FastLEDOFF(){
      FastLED.clear();
    FastLED.delay(1000 / FPS);

}

void irRemoteLoop(){
    unsigned long tmpValue;
 
  if (irrecv.decode(&results)) // have we received an IR signal?
  {
    //The implementation effect of the above program is 
    //the same as that of the following, but it is more concise
    //Serial.println(results.value);
    tmpValue = results.value;
    switch(results.value)
    { 
      // Serial.println("POWER");
      case 0xFFA25D: 
         togglePower();
      break;


      case 0xFFE21D: 
         // Serial.println("FUNC/STOP");
         // ledactive==1? 0:1;
      break;
  
      case 0xFF22DD:
         Serial.println("FAST BACK");
         scaleDownFast();
      break;
      
      case 0xFF02FD:
         Serial.println("PAUSE"); 
      break;

      case 0xFFC23D: 
         Serial.println("FAST FORWARD"); 
         scaleUpFast();
        
      break;
    
      // speed
      case 0xFF629D: 
         //Serial.println("VOL+"); 
         speedUp();
        
      break;

      case 0xFFA857: 
         //Serial.println("VOL-"); 
         speedDown();
         break;

      // scale
      case 0xFF906F:
         //  Serial.println("UP"); 
         scaleUp();  
         break;
     
       case 0xFFE01F: 
         scaleDown();
         break;

       case 0xFF9867: 
          //Serial.println("EQ");  
        	brightnessDown();
         // FastLED.setBrightness(84);
          break;
         
       case 0xFFB04F: 
          //Serial.println("ST/REPT");  
          brightnessUp();
          break;

          
        // palette

       case 0xFF6897: 
          //Serial.println("0");
          setPalette(0); // random
          break;

       case 0xFF30CF: 
          // Serial.println("1");
          setPalette(1); // forest
          break;
       case 0xFF18E7:
          // Serial.println("2");    
          setPalette(2); // Lava  
          break;
        case 0xFF7A85: 
           //Serial.println("3"); 
           setPalette(3); //Clouds
           break;
        case 0xFF10EF: 
           //Serial.println("4");   
           setPalette(4); //Ocean
           break;
        case 0xFF38C7: 
           //Serial.println("5");   
           setPalette(5);
           break;
        case 0xFF5AA5: 
          //Serial.println("6");  
          setPalette(6);
          break;
        case 0xFF42BD: 
          //Serial.println("7"); 
         setPalette(7);
        break;
        case 0xFF4AB5:
          // Serial.println("8"); 
            setPalette(8);
            break;
        case 0xFF52AD:
          // Serial.println("9");  
          setPalette(9);
        break;

        case 0xFFFFFFFF: 
          //Serial.println(" REPEAT");
        break;  
       // default: 
         /*
          Serial.print(" other button   ");
          Serial.println(results.value);
          Serial.print(" other button v  ");
          Serial.println(tmpValue,HEX);
         */

    }// End Case
    irrecv.resume(); // receive the next value
 } // if ir sig recvd
  
}


void togglePower(){
if (ledactive==1) ledactive = 0;
        else ledactive = 1;
        Serial.print("POWER: ");
        Serial.println(ledactive);
} //togglePower()

void scaleUp(){
        if(scale < 200) scale += 1;
        Serial.print("Scale up: ");
        Serial.println(scale);
}      

void scaleUpFast(){
        if(scale < 200) scale += 10;
        Serial.print("Scale up: ");
        Serial.println(scale);
}

void scaleDown(){
        if(scale < 200) scale -= 1;
        Serial.print("Scale up: ");
        Serial.println(scale);
}      

void scaleDownFast(){
        if(scale < 200) scale -= 10;
        Serial.print("Scale up: ");
        Serial.println(scale);
}

void speedUp(){
        if (speed<255) speed ++;
        Serial.print("Speed: ");
        Serial.println(speed);
}
void speedDown(){
        if (speed>0) speed --;
        Serial.print("Speed: ");
        Serial.println(speed);
}

void brightnessUp(){
        if (brightness<255) brightness ++;
        Serial.print("brightness: ");
        Serial.println(brightness);
        FastLED.setBrightness(brightness);
}

void brightnessDown(){
        if (brightness>0) brightness --;
        Serial.print("brightness: ");
        Serial.println(brightness);
        char bri = (char) brightness;
      //  showDisplay(strcat("brightness:",bri));  

        FastLED.setBrightness(brightness);
}

void wifiloop(){
 WiFiClient client = server.available();   // listen for incoming clients

  if (client) {                             // if you get a client,
    Serial.println("New Client.");           // print a message out the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected()) {            // loop while the client's connected
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        if (c == '\n') {                    // if the byte is a newline character

          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
       
            //    client.println("<HTML><meta HTTP/1.1 200 OKContent-type:text/html><head><style>.item1 { grid-column: 1; grid-auto-columns: 50px;}.item2 { grid-column: 2; }.item3 { grid-column: 3; }.item4 { grid-column: 1/span 3;}.grid-container {display: grid;grid-template-columns: auto auto auto;gap: 10px;background-color: rgba(255, 255, 255, 0.8);text-align: center;padding: 10px;}.grid-item {background-color: rgba(255, 255, 255, 0.8);text-align: center;padding: 20px;font-size: 30px;}.grid-container > div {padding: 20px 0;font-size: 30px;}.grid-container {display: grid;grid-template-columns: auto auto auto;gap: 10px;background-color: #2196F3;padding: 10px;}.grid-container > div { background-color: rgba(255, 255, 255, 0.8); text-align: center; padding: 20px 0; font-size: 20px;}.switch {position: relative;display: inline-block;width: 60px;height: 34px;}/* Hide default HTML checkbox */.switch input {opacity: 0;width: 0;height: 0;}/* The slider */.slider {position: absolute;cursor: pointer;top: 0;left: 0;right: 0;bottom: 0;background-color: #ccc;-webkit-transition: .4s;transition: .4s;}.slider:before {position: absolute;content: \"\";height: 26px;width: 26px;left: 4px;bottom: 4px;background-color: white;-webkit-transition: .4s;transition: .4s;}input:checked + .slider {background-color: #2196F3;}input:focus + .slider {box-shadow: 0 0 1px #2196F3;}input:checked + .slider:before {-webkit-transform: translateX(26px);-ms-transform: translateX(26px);transform: translateX(26px);}/* Rounded sliders */.slider.round {border-radius: 34px;}.slider.round:before {border-radius: 50%;}</style></head><body><H1><B style=\"color:red;\">L</B><b style=\"color:green;\">E</b><b style=\"color:blue;\">D</b><b> Machine</b></H1><div class=\"grid-container\"><div class=\"item1\"><b>Power</b></div><div class=\"item2\"><a href=\"/ON\">ON</a> </div> <div class=\"item3\"> <a href=\"/OFF\">OFF</a></div><div><!-- Rounded switch --><a href=\"/ON\"><label class=\"switch\"><input type=\"checkbox\"><span class=\"slider round\"></span></label></a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Speed</b></div><div class=\"item2\"><a href=\"/FASTER\">Faster &#128640;</a></div><div class=\"item3\"><a href=\"/SLOWER\">Slower &#128012;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Scale</b></div><div class=\"item2\"><a href=\"/SCALEUP\">Up &#128200;</a></div> <div class=\"item3\"><a href=\"/SCALEDOWN\">Down &#128201;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Brightness</b></div><div class=\"item2\"><a href=\"/DARKER\">Darker &#128526;</a></div><div class=\"item3\"><a href=\"/BRIGHTER\">Brighter &#128161;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Color Loop</b></div><div class=\"item2\"><a href=\"/COLORLOOPON\">Loop ON &#128274;</a></div><div class=\"item3\"><a href=\"/COLORLOOPOF\">Loop OFF &#128275;</a></div></div><div class=\"grid-container\"><div class=\"item4\"><B>Palette</b></div><div><a href=\"/PALFOREST\">Forest &#127796;</a></div><div><a href=\"/PALLAVA\">Lava &#127755;</a></div><div><a href=\"/PALCLOUDS\">Clouds</a></div><div><a href=\"/PALOCEAN\">Ocean &#127754;</a></div><div><a href=\"/PALPARTY\">PARTY &#127881;</a></div><div><a href=\"/PALBLUEGREEN\">Blue and Green &#127757;</a></div><div><a href=\"/PALRAINBOW\">Rainbow &#127752;</a></div><div><a href=\"/PALRAINBOWSTRIPE\">Rainbow Stripes &#128136;</a></div><div><a href=\"/PALRLUCPIZZA\">Luc Pizza &#127829;</a></div><div><a href=\"/PALRANDOM\">Random &#127920;</a></div></div></div></body></html>");
            //client.println("<HTML>HTTP/1.1 200 OKContent-type:text/html<head><style>.item1 { grid-column: 1; grid-auto-columns: 50px;}.item2 { grid-column: 2; }.item3 { grid-column: 3; }.item4 { grid-column: 1/span 3;}.grid-container {display: grid;grid-template-columns: auto auto auto;gap: 10px;background-color: rgba(255, 255, 255, 0.8);text-align: center;padding: 10px;}.grid-item {background-color: rgba(255, 255, 255, 0.8);text-align: center;padding: 20px;font-size: 30px;}.grid-container > div {padding: 20px 0;font-size: 30px;}.grid-container {display: grid;grid-template-columns: auto auto auto;gap: 10px;background-color: #2196F3;padding: 10px;}.grid-container > div { background-color: rgba(255, 255, 255, 0.8); text-align: center; padding: 20px 0; font-size: 20px;}.switch {position: relative;display: inline-block;width: 60px;height: 34px;}/* Hide default HTML checkbox */.switch input {opacity: 0;width: 0;height: 0;}/* The slider */.slider {position: absolute;cursor: pointer;top: 0;left: 0;right: 0;bottom: 0;background-color: #ccc;-webkit-transition: .4s;transition: .4s;}.slider:before {position: absolute;content: \"\";height: 26px;width: 26px;left: 4px;bottom: 4px;background-color: white;-webkit-transition: .4s;transition: .4s;}input:checked + .slider {background-color: #2196F3;}input:focus + .slider {box-shadow: 0 0 1px #2196F3;}input:checked + .slider:before {-webkit-transform: translateX(26px);-ms-transform: translateX(26px);transform: translateX(26px);}/* Rounded sliders */.slider.round {border-radius: 34px;}.slider.round:before {border-radius: 50%;}</style></head><body><H1><B style=\"color:red;\">L</B><b style=\"color:green;\">E</b><b style=\"color:blue;\">D</b><b> Machine</b></H1><div class=\"grid-container\"><div class=\"item1\"><b>Power</b></div><div class=\"item2\"><a href=\"/ON\">ON</a> </div> <div class=\"item3\"> <a href=\"/OFF\">OFF</a></div><div><!-- Rounded switch --><a href=\"/ON\"><label class=\"switch\"><input type=\"checkbox\"><span class=\"slider round\"></span></label></a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Speed</b></div><div class=\"item2\"><a href=\"/FASTER\">Faster &#128640;</a></div><div class=\"item3\"><a href=\"/SLOWER\">Slower &#128012;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Scale</b></div><div class=\"item2\"><a href=\"/SCALEUP\">Up &#128200;</a></div> <div class=\"item3\"><a href=\"/SCALEDOWN\">Down &#128201;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Brightness</b></div><div class=\"item2\"><a href=\"/DARKER\">Darker &#128526;</a></div><div class=\"item3\"><a href=\"/BRIGHTER\">Brighter &#128161;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Color Loop</b></div><div class=\"item2\"><a href=\"/COLORLOOPON\">Loop ON &#128274;</a></div><div class=\"item3\"><a href=\"/COLORLOOPOF\">Loop OFF &#128275;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Hue Delta</b></div><div class=\"item2\"><a href=\"/IHUEUP\">Up &#128274;</a></div><div class=\"item3\"><a href=\"/IHUEDOWN\">Down &#128275;</a></div></div><div class=\"grid-container\"><div class=\"item4\"><B>Palette</b></div><div><a href=\"/PALFOREST\">Forest &#127796;</a></div><div><a href=\"/PALLAVA\">Lava &#127755;</a></div><div><a href=\"/PALCLOUDS\">Clouds</a></div><div><a href=\"/PALOCEAN\">Ocean &#127754;</a></div><div><a href=\"/PALPARTY\">PARTY &#127881;</a></div><div><a href=\"/PALBLUEGREEN\">Blue and Green &#127757;</a></div><div><a href=\"/PALRAINBOW\">Rainbow &#127752;</a></div><div><a href=\"/PALRAINBOWSTRIPE\">Rainbow Stripes &#128136;</a></div><div><a href=\"/PALREDBLUEWHT\">RedBlueWhite &#128136;</a></div><div><a href=\"/PALRANDOM\">Random &#127920;</a></div></div></div></body></html>");
            client.println("<HTML><meta HTTP/1.1 200 OKContent-type:text/html><head><style>.item1 { grid-column: 1; grid-auto-columns: 50px;}.item2 { grid-column: 2; }.item3 { grid-column: 3; }.item4 { grid-column: 1/span 3;}.grid-container {display: grid;grid-template-columns: auto auto auto;gap: 10px;background-color: rgba(255, 255, 255, 0.8);text-align: center;padding: 10px;}.grid-item {background-color: rgba(255, 255, 255, 0.8);text-align: center;padding: 20px;font-size: 30px;}.grid-container > div {padding: 20px 0;font-size: 30px;}.grid-container {display: grid;grid-template-columns: auto auto auto;gap: 10px;background-color: #2196F3;padding: 10px;}.grid-container > div { background-color: rgba(255, 255, 255, 0.8); text-align: center; padding: 20px 0; font-size: 20px;}.switch {position: relative;display: inline-block;width: 60px;height: 34px;}/* Hide default HTML checkbox */.switch input {opacity: 0;width: 0;height: 0;}/* The slider */.slider {position: absolute;cursor: pointer;top: 0;left: 0;right: 0;bottom: 0;background-color: #ccc;-webkit-transition: .4s;transition: .4s;}.slider:before {position: absolute;content: \"\";height: 26px;width: 26px;left: 4px;bottom: 4px;background-color: white;-webkit-transition: .4s;transition: .4s;}input:checked + .slider {background-color: #2196F3;}input:focus + .slider {box-shadow: 0 0 1px #2196F3;}input:checked + .slider:before {-webkit-transform: translateX(26px);-ms-transform: translateX(26px);transform: translateX(26px);}/* Rounded sliders */.slider.round {border-radius: 34px;}.slider.round:before {border-radius: 50%;}</style></head><body><H1><B style=\"color:red;\">L</B><b style=\"color:green;\">E</b><b style=\"color:blue;\">D</b><b> Machine</b></H1><div class=\"grid-container\"><div class=\"item1\"><b>Power</b></div><div class=\"item2\"><a href=\"/ON\">ON</a> </div> <div class=\"item3\"> <a href=\"/OFF\">OFF</a></div><div><!-- Rounded switch --><a href=\"/ON\"><label class=\"switch\"><input type=\"checkbox\"><span class=\"slider round\"></span></label></a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Speed</b></div><div class=\"item2\"><a href=\"/FASTER\">Faster &#128640;</a></div><div class=\"item3\"><a href=\"/SLOWER\">Slower &#128012;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Scale</b></div><div class=\"item2\"><a href=\"/SCALEUP\">Up &#128200;</a></div> <div class=\"item3\"><a href=\"/SCALEDOWN\">Down &#128201;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Brightness</b></div><div class=\"item2\"><a href=\"/DARKER\">Darker &#128526;</a></div><div class=\"item3\"><a href=\"/BRIGHTER\">Brighter &#128161;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Color Loop</b></div><div class=\"item2\"><a href=\"/COLORLOOPON\">Loop ON &#128274;</a></div><div class=\"item3\"><a href=\"/COLORLOOPOF\">Loop OFF &#128275;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Hue Delta</b></div><div class=\"item2\"><a href=\"/IHUEUP\">Up &#128274;</a></div><div class=\"item3\"><a href=\"/IHUEDOWN\">Down &#128275;</a></div></div><div class=\"grid-container\"><div class=\"item1\"><b>Hue </b></div><div class=\"item2\"><a href=\"/IHUEDELTAUP\">Up &#128274;</a></div><div class=\"item3\"><a href=\"/IHUEDELTADOWN\">Down &#128275;</a></div></div><div class=\"grid-container\"><div class=\"item4\"><B>Palette</b></div><div><a href=\"/PALFOREST\">Forest &#127796;</a></div><div><a href=\"/PALLAVA\">Lava &#127755;</a></div><div><a href=\"/PALCLOUDS\">Clouds</a></div><div><a href=\"/PALOCEAN\">Ocean &#127754;</a></div><div><a href=\"/PALPARTY\">PARTY &#127881;</a></div><div><a href=\"/PALBLUEGREEN\">Blue and Green &#127757;</a></div><div><a href=\"/PALRAINBOW\">Rainbow &#127752;</a></div><div><a href=\"/PALRAINBOWSTRIPE\">Rainbow Stripes &#128136;</a></div><div><a href=\"/PALREDBLUEWHT\">RedBlueWhite &#128136;</a></div><div><a href=\"/PALRLUCPIZZA\">Luc Pizza &#127829;</a></div><div><a href=\"/PALRANDOM\">Random &#127920;</a></div></div></div></body></html>");

            // break out of the while loop:
            break;
          } else {    // if you got a newline, then clear currentLine:
             currentLine = "";
          }
        } else if (c != '\r') {  // if you got anything else but a carriage return character,
           currentLine += c;      // add it to the end of the currentLine
        }

   
      //  if (currentLine.endsWith("GET /ON"))               if(ledactive == 1) ledactive = 0; else ledactive = 1;
       // if (currentLine.endsWith("GET /OFF"))              ledactive = 0;
        if (currentLine.endsWith("GET /ON"))               togglePower();
        if (currentLine.endsWith("GET /OFF"))              togglePower();

        if (currentLine.endsWith("GET /FASTER"))           speedUp();
        if (currentLine.endsWith("GET /SLOWER"))           speedDown();
        
        if (currentLine.endsWith("GET /SCALEUP"))          scaleUp();
        if (currentLine.endsWith("GET /SCALEDOWN"))        scaleDown();

        if (currentLine.endsWith("GET /BRIGHTER"))         brightnessUp();
        if (currentLine.endsWith("GET /DARKER"))           brightnessDown();

        if (currentLine.endsWith("GET /COLORLOOPON"))      colorLoopOn();
        if (currentLine.endsWith("GET /COLORLOOPOF"))      colorLoopOff();
        if (currentLine.endsWith("GET /IHUEUP"))           iHueUp();
        if (currentLine.endsWith("GET /IHUEDOWN"))         iHueDown();
      
        if (currentLine.endsWith("GET /PALFOREST"))        setPalette(1);
        if (currentLine.endsWith("GET /PALLAVA"))          setPalette(2);
        if (currentLine.endsWith("GET /PALCLOUDS"))        setPalette(3);
        if (currentLine.endsWith("GET /PALOCEAN"))         setPalette(4);
        if (currentLine.endsWith("GET /PALPARTY"))         setPalette(5);
        if (currentLine.endsWith("GET /PALBLUEGREEN"))     setPalette(6);
        if (currentLine.endsWith("GET /PALRAINBOW"))       setPalette(7);
        if (currentLine.endsWith("GET /PALRAINBOWSTRIPE")) setPalette(8);
        if (currentLine.endsWith("GET /PALLUCPIZZA"))      setPalette(9);
        if (currentLine.endsWith("GET /PALRANDOM"))        setPalette(0);
/*        
        if (currentLine.endsWith("GET /PALFOREST"))        setPalette("forest");
        if (currentLine.endsWith("GET /PALLAVA"))          setPalette("lava");
        if (currentLine.endsWith("GET /PALCLOUDS"))        setPalette("clouds");
        if (currentLine.endsWith("GET /PALOCEAN"))         setPalette("ocean");
        if (currentLine.endsWith("GET /PALPARTY"))         setPalette("party");
        if (currentLine.endsWith("GET /PALBLUEGREEN"))     setPalette("bluegreen");
        if (currentLine.endsWith("GET /PALRAINBOW"))       setPalette("rainbow");
        if (currentLine.endsWith("GET /PALRAINBOWSTRIPE")) setPalette("rainbowstripe");
        if (currentLine.endsWith("GET /PALRANDOM"))        setPalette("random");

     */
      }
    }
    // close the connection:
    client.stop();
    Serial.println("Client Disconnected.");
  }
} //wifiloop


// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.

// 1 = 5 sec per palette
// 2 = 10 sec per palette
// etc
#define HOLD_PALETTES_X_TIMES_AS_LONG 2

void ChangePaletteAndSettingsPeriodically()
{
  uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
  //  if( secondHand ==  0)  { currentPalette = RainbowColors_p;         speed = 20; scale = 30; colorLoop = 1; }
   // if( secondHand ==  5)  { SetupPurpleAndGreenPalette();             speed = 10; scale = 50; colorLoop = 1; }
  //  if( secondHand == 10)  { SetupBlackAndWhiteStripedPalette();       speed = 20; scale = 30; colorLoop = 1; }
   // if( secondHand == 15)  { currentPalette = ForestColors_p;          speed =  8; scale =120; colorLoop = 0; }
    // if( secondHand == 20)  { currentPalette = CloudColors_p;           speed =  4; scale = 30; colorLoop = 0; }
     if( secondHand == 25)  { currentPalette = LavaColors_p;            speed =  4; scale = 30; colorLoop = 1; }
    // if( secondHand == 30)  { currentPalette = OceanColors_p;           speed = 20; scale = 90; colorLoop = 0; }
    // if( secondHand == 35)  { currentPalette = PartyColors_p;           speed = 20; scale = 30; colorLoop = 1; }
     if( secondHand == 40)  { currentPalette = LavaColors_p;            speed =  8; scale = 100; colorLoop = 1; }
     if( secondHand == 45)  {  currentPalette = LavaColors_p;            speed =  16; scale = 50; colorLoop = 1; }
 //   if( secondHand == 50)  { SetupRandomPalette();                     speed = 90; scale = 90; colorLoop = 1; }
 //   if( secondHand == 55)  { currentPalette = RainbowStripeColors_p;   speed = 30; scale = 20; colorLoop = 1; }
  }
}


//
// Mark's xy coordinate mapping code.  See the XYMatrix for more information on it.
//
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }
  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint16_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
    CRGB purple = CHSV( HUE_PURPLE, 255, 255);
    CRGB green  = CHSV( HUE_GREEN, 255, 255);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   green,  green,  black,  black,
                                   purple, purple, black,  black,
                                   green,  green,  black,  black,
                                   purple, purple, black,  black );
}
void SetupLucPizzaPalette()
{
    CRGB red = CHSV( 5, 92, 94);
    CRGB orange  = CHSV( 16, 85, 94);
    CRGB yellow  = CHSV( 56, 100, 100);
    CRGB brown  = CHSV( 47, 86, 74);
    CRGB black  = CRGB::Black;
    
    currentPalette = CRGBPalette16(
                                   red,  red,  orange,  orange,
                                   yellow, orange, brown,  black,
                                   red,  red,  orange,  brown,
                                   yellow, yellow, orange,  brown );
}

void SetupRandomPalette()
{
  currentPalette = CRGBPalette16( 
                      CHSV( random8(), 255, 32), 
                      CHSV( random8(), 255, 255), 
                      CHSV( random8(), 128, 255), 
                      CHSV( random8(), 255, 255)); 
}

void colorLoopOn(){
  colorLoop = 1;
}
void colorLoopOff(){
  colorLoop = 0;
}
void setPalette(uint8_t dpalette)
{
    switch (dpalette)
   {
     case 1:
       currentPalette = ForestColors_p;
       showDisplay("Palette: ForestColors_p");  
       break;
     case 2:
       currentPalette = LavaColors_p;
       showDisplay("Palette: LavaColors_p");  
       break;
     case 3:
       currentPalette = CloudColors_p;
       showDisplay("Palette: CloudColors_p");  
       break;
     case 4:
       currentPalette = OceanColors_p;
       showDisplay("Palette: OceanColors_p");
       break;
     case 5:
       currentPalette = CloudColors_p;
       showDisplay("Palette: PartyColors_p");  
       break;
     case 6:
       SetupPurpleAndGreenPalette();
       showDisplay("Palette: PurpleAndGreenPalette");  
       break;
     case 7:
       currentPalette = RainbowColors_p;  
       showDisplay("Palette: RainbowColors_p");  
       break;
     case 8:
       currentPalette = RainbowStripeColors_p;  
       showDisplay("Palette: RainbowStripe");  
       break;
     case 9:
       SetupLucPizzaPalette();
      // currentPalette = myRedWhiteBluePalette_p;
       showDisplay("Palette: LucPizzaPalette");  
      displaystatus();
       break;
     case 0:
        SetupRandomPalette();
        Serial.println("random"); 
        break;
   }
   /*  switch (dpalette)
   {
     case "forest":
        currentPalette = ForestColors_p;
        break;
     case "lava":
        currentPalette = LavaColors_p;
        break;
     case "clouds"
        currentPalette = CloudColors_p;
        break;
     case "ocean"
        currentPalette = OceanColors_p;
        break;
     case "party"
        currentPalette = ForestColors_p;
        break;
     case "rainbow"
        currentPalette = RainbowColors_p;
        break;
     case "rainbowstripe"
        currentPalette = ForestColors_p;
        break;
     case "bluegreen"
        SetupPurpleAndGreenPalette();
        break;
     case "random"
        SetupRandomPalette();
        break;
   }
 */
}        

void  iHueUp(){
  if (ihuedelta < 255)
   ihuedelta++;      
  //else ihuedelta = 0;   
int temp = ihuedelta;    
  Serial.print("Hue delta up:"); Serial.println(temp);
}
void  iHueDown(){
    if (ihuedelta > 0)
   ihuedelta--;      
  //else ihuedelta = 255;    
  int temp = ihuedelta;        
  Serial.print("Hue delta Down:"); Serial.print(temp);Serial.print(" - Hue :");Serial.print(ihue);
}
       
void showDisplay(char *txt)
{
  //  lcd.print(txt);
    Serial.println(txt);

  
}


void displaystatus(){
 Serial.print("Version: "); Serial.println("v 2.2");
 Serial.print("ledactive: "); Serial.println(ledactive);

 Serial.print("colorloop: "); Serial.println(colorLoop );
 Serial.print("Speed: "); Serial.println(speed);
 Serial.print("Scale: "); Serial.println(scale);
 Serial.print("Hue: ");Serial.println(ihue);
 Serial.print("Hue delta: "); Serial.println(ihuedelta);

Serial.print("X: ");Serial.println(x);
Serial.print("Y: ");Serial.println(y);
Serial.print("Z: ");Serial.println(z);
Serial.print("X2: ");Serial.println(x2);
Serial.print("Y2: ");Serial.println(y2);
Serial.print("Z2: ");Serial.println(z2);
}