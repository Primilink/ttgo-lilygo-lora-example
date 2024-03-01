
#include <WiFi.h>       // Wifi library
#include <HTTPClient.h> // HTTP client library for post requests

// Your Domain name with URL path or IP address with path
String serverName = ""; // server name

const char *ssid = "";
const char *password = "";

// Libraries import
#include <SPI.h>
#include <LoRa.h>

#include <Wire.h> // I2c

#include <Adafruit_GFX.h>     // OLED
#include <Adafruit_SSD1306.h> // OLED

#include <ArduinoJson.h> // JSON management

// OLED configuration
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// LoRa configuration
// define the pins used by the LoRa transceiver module
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

// LoRa band frequency configuration
// 433E6 for Asia
// 866E6 for Europe
// 915E6 for North America
#define BAND 915E6

// Global packet
DynamicJsonDocument packet_out(1024);

// Enviar datos al servidor
void postDataToServer()
{
    // if connected to wifi post data to server
    if (WiFi.status() == WL_CONNECTED)
    {
        WiFiClient client;
        HTTPClient http;

        // Your Domain name with URL path or IP address with path
        http.begin(client, serverName);

        String message_out = "";
        serializeJson(packet_out, message_out);

        debugPrint(message_out, 1);

        // If you need an HTTP request with a content type: application/json, use the following:
        http.addHeader("Content-Type", "application/json");
        int httpResponseCode = http.POST(message_out);

        debugPrint("HTTP: " + String(httpResponseCode), 1);
        debugPrint(http.getString(), 0);

        // Free resources
        http.end();
    }
}

void debugPrint(String text, bool lineBreak)
{
    if (lineBreak == 1)
    {
        display.clearDisplay();
        display.setCursor(0, 0);
    }
    display.println(text);
    Serial.println(text);
}

void setup()
{
    // reprogram time
    delay(1000);

    // Initialize Serial Monitor
    Serial.begin(115200);

    // Start OLED display
    if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C))
    {
        Serial.println(F("SSD1306 allocation failed"));
        for (;;)
            ;
    }

    // Print some stuff
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 10);
    display.display();

    // SPI LoRa pins
    SPI.begin(SCK, MISO, MOSI, SS);
    // setup LoRa transceiver module
    LoRa.setPins(SS, RST, DIO0);

    // LoRa band configuration
    if (!LoRa.begin(BAND))
    {
        debugPrint("LoRa error!", 0);
        while (1)
            ;
    }

    // Print success message
    debugPrint("LoRa OK!", 0);

    // delete old config
    WiFi.disconnect(true);

    delay(1000);

    WiFi.begin(ssid, password);

    unsigned long start = millis();
    while (WiFi.status() != WL_CONNECTED)
    {
        debugPrint("Esperando conexión WiFi con: " + String(ssid), 1);
        delay(1000);
        if (millis() - start > 10000)
        {
            // restart ESP
            ESP.restart();
        }
    }

    // Print initial message
    debugPrint("All done", 1);
}

void loop()
{
    // check serial
    if (Serial.available())
    {
        String serial_message = Serial.readString();

        DynamicJsonDocument sending_packet(1024);

        // remove trailing \n or \r
        serial_message.trim();

        packet_out["temperature"] = 12.5; // fill with your data
        packet_out["humidity"] = 40.5;    // fill with your data
        packet_out["message"] = serial_message;

        postDataToServer();
    }
}