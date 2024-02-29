#include <Wire.h>             // I2c
#include <Adafruit_GFX.h>     // OLED
#include <Adafruit_SSD1306.h> // OLED

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

void setup()
{
    // initialize serial
    Serial.begin(115200);

    // Check if the OLED is connected on Address 0x3C
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        while (1)
            ; // Don't proceed, loop forever
    }

    display.clearDisplay();      // Clear the screen
    display.setTextSize(1);      // Set text size
    display.setTextColor(WHITE); // Set text color
    display.setCursor(0, 0);
    display.println("Hello world"); // Display static text
    display.display();
    delay(2000);
}

void loop()
{
    if (Serial.available())
    {
        display.clearDisplay(); // Clear the screen
        display.setCursor(0, 0);
        display.print("Serial: ");
        display.println(Serial.readString());
        display.display();
    }
}
