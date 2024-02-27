#include <Wire.h>             // I2c
#include <Adafruit_GFX.h>     // OLED
#include <Adafruit_SSD1306.h> // OLED

#include <SPI.h>  // SPI
#include <LoRa.h> // LoRa

#include <ArduinoJson.h> // JSON

// define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
#define BAND 915E6

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
    display.println("LoRa sender"); // Display static text
    display.display();
    delay(1000);

    // SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DIO0); // setup LoRa transceiver module

    // initialize LoRa transceiver module
    if (!LoRa.begin(BAND))
    {
        display.println("LoRa init failed.");
        display.display();
        while (1)
            ;
    }
    display.println("LoRa init succeeded.");
    display.display();
    delay(1000);
}

void loop()
{
    byte i = 0;
    while (1)
    {
        // JSON
        StaticJsonDocument<200> packet; // 200 bytes, if you are sending bigger packets, increase this number, 1024 bytes is a good start

        packet["id"] = i;
        packet["value"] = random(0, 100);

        // Message
        display.clearDisplay();
        display.setCursor(0, 0);
        display.print("Sending: ");
        serializeJson(packet, display);
        display.display();

        // send packet
        LoRa.beginPacket();
        serializeJson(packet, LoRa);
        LoRa.endPacket();
        delay(400);
        i++;
    }
}