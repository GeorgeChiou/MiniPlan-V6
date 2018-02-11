/*********************************************************************
This is an example for our Monochrome OLEDs based on SSD1306 drivers

  Pick one up today in the adafruit shop!
  ------> http://www.adafruit.com/category/63_98

This example is for a 128x64 size display using I2C to communicate
3 pins are required to interface (2 I2C and one reset)

Adafruit invests time and resources providing this open source code, 
please support Adafruit and open-source hardware by purchasing 
products from Adafruit!

Written by Limor Fried/Ladyada  for Adafruit Industries.  
BSD license, check license.txt for more information
All text above, and the splash screen must be included in any redistribution

Code edited by Andrew R. from HobbyTransform for this example.
*********************************************************************/

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define XPOS 0
#define YPOS 1

#define ANALOG_X A0
#define ANALOG_Y A1
#define ANALOG_Z A2

#define LOGO16_GLCD_HEIGHT 16 
#define LOGO16_GLCD_WIDTH  16 
static const unsigned char PROGMEM logo16_glcd_bmp[] =
{ B11000011, B00000000,
  B11000011, B00000000,
  B11000011, B00000000,
  B11111111, B00000000,
  B11111111, B00000000,
  B11000011, B00000000,
  B11000011, B00000000,
  B11000011, B00000000,
  B00000000, B11111111,
  B00000000, B11111111,
  B00000000, B00011000,
  B00000000, B00011000,
  B00000000, B00011000,
  B00000000, B00011000,
  B00000000, B00011000,
  B00000000, B00011000};

#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

char incomingByte;
int lines = 0;
int chars = 0;

void setup()   {                
  Serial.begin(9600);

  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x64)
  // init done
  
  // Show image buffer on the display hardware.
  // Since the buffer is intialized with an Adafruit splashscreen
  // internally, this will display the splashscreen.
  display.display();
  delay(2000);

  // Clear the buffer.
  display.clearDisplay();

  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0,0);

  // draw a bitmap icon and 'animate' movement
  //movingbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
}


void loop() {
  // Read text from the serial window
  incomingByte = Serial.read();
  
  // Print incoming characters, if any, and keep track of the line and chars number
  if(incomingByte != -1){
    chars++;
    if(incomingByte == '\n' || chars == 21){
      lines++;
      chars = 0;
    }
    Serial.print(incomingByte);
    display.print(incomingByte);
    display.display();
    delay(10);

    // Clear screen to print new message if we reach the end
    if(lines == 8){
      delay(500);
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(WHITE);
      display.setCursor(0,0);
      lines = 0;
      chars = 0;
    }
  }
  
  // Change modes if joystick button pressed
  if(analogRead(ANALOG_Z) == LOW){
    lines = 0;
    chars = 0;
    display.clearDisplay();
    movingbitmap(logo16_glcd_bmp, LOGO16_GLCD_HEIGHT, LOGO16_GLCD_WIDTH);
  }
}


void movingbitmap(const uint8_t *bitmap, uint8_t w, uint8_t h) {
  uint8_t icons[2];
  uint16_t width = 172;
  uint16_t height = 73;
  uint16_t joystick_x;
  uint16_t joystick_y;
  uint16_t joystick_z;
 
  // initialize
  icons[XPOS] = width/2;
  icons[YPOS] = height/2;

  while (1) {
    // draw icon
    display.drawBitmap(icons[XPOS], icons[YPOS], bitmap, w, h, WHITE);
    display.display();
    delay(10);
    
    // then erase it + move it
    display.drawBitmap(icons[XPOS], icons[YPOS], bitmap, w, h, BLACK);
    
    // move it
    joystick_x = analogRead(ANALOG_X);
    joystick_y = analogRead(ANALOG_Y);
    joystick_z = analogRead(ANALOG_Z);
    icons[XPOS] = (joystick_x/1023.0)*width;
    icons[YPOS] = (joystick_y/1023.0)*height;
    
    // if it exceeds screen dimensions, max it
    if(icons[XPOS] > width) icons[XPOS] = width;
    if(icons[YPOS] > height) icons[YPOS] = height;
    
    // button press to change modes
    if(joystick_z == LOW){
      display.clearDisplay();
      break;
    }
  }
}
