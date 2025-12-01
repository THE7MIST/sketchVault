/*─────────────────────────────────────────────────────────────────────────────
    ESP8266 LoRa Receiver with OLED Display
    → Receives LoRa messages and shows them on the OLED screen.
──────────────────────────────────────────────────────────────────────────────
  ■ Components Used
    - ESP8266 (NodeMCU)
    - LoRa SX1278 Module (433 MHz)
    - OLED Display (SSD1306, I2C, 128×64)

  ■ Pin Connections
        LoRa Module → ESP8266
        -----------------------
        |  LoRa Pin  |  ESP8266 |
        |------------|----------|
        | NSS / SS   |   D8     |
        | RST        |   D0     |
        | DIO0       |   D3     |
        | MOSI       |   D7     |
        | MISO       |   D6     |
        | SCK        |   D5     |
        | 3.3V       |  3.3V    |
        | GND        |   GND    |
        -----------------------

        OLED Display (SSD1306, I2C)
        ----------------------------
        SDA → D2
        SCL → D1

  ■ What This Program Does
    - Listens for LoRa packets continuously.
    - Reads the received message.
    - Shows the message on the OLED screen.
    - Shows signal strength (RSSI).
    - Uses long-range LoRa settings:
         • SF12
         • 125 kHz bandwidth
         • Coding rate 4/8
         • CRC enabled
         • Stable preamble
         • TX Power set for SX1278 limits

  ■ Code Flow Summary

    ● setup()
      - Start Serial at 9600
      - Initialize OLED (0x3C)
      - Configure LoRa pins
      - Initialize LoRa at 433 MHz
      - Apply long-range settings
      - Display “LoRa Ready”

    ● loop()
      - Check for LoRa packet
      - If packet received:
          • Read full message
          • Print to Serial
          • Display message on OLED
          • Show RSSI (dBm)

──────────────────────────────────────────────────────────────────────────────*/


#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>

// -------------------- OLED SETUP --------------------
Adafruit_SSD1306 display(128, 64, &Wire);

// -------------------- LoRa Pins ----------------------
#define LORA_SS   D8      // Chip Select
#define LORA_RST  D0      // Reset
#define LORA_DIO0 D3      // DIO0 (IRQ)

// -------------------- Setup --------------------------
void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("ESP8266 LoRa Receiver Starting...");
  
  // -------- Initialize OLED --------
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED init failed!");
    while (1);
  }
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("LoRa Receiver Booting...");
  display.display();

  // -------- Initialize LoRa --------
  Serial.println("Initializing LoRa...");
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed. Check wiring!");
    display.clearDisplay();
    display.setCursor(0, 0);
    display.println("LoRa FAILED!");
    display.display();
    while (1);
  }

  Serial.println("LoRa init OK!");
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("LoRa Ready...");
  display.display();
LoRa.setSpreadingFactor(12);        // SF12 = max range
LoRa.setSignalBandwidth(125E3);     // 125 kHz = best sensitivity
LoRa.setCodingRate4(8);             // 4/8 = highest error correction
LoRa.setPreambleLength(8);          // default, stable
LoRa.enableCrc();                   // reject corrupted packets
LoRa.setTxPower(20); 
 // max for SX1278

}

// -------------------- Loop ---------------------------
void loop() {
  // Check for incoming packet
  int packetSize = LoRa.parsePacket();

  if (packetSize) {
    Serial.print("Received packet: ");

    String message = "";

    // Read the incoming LoRa message
    while (LoRa.available()) {
      char c = (char)LoRa.read();
      message += c;
    }

    // Print on Serial Monitor
    Serial.println(message);

    String displayMsg = message.substring(0, 21 * 8);  // max visible chars = 168

    display.clearDisplay();
    display.setCursor(0, 0);
    display.setTextSize(1);
    display.println(displayMsg);


    // Debugging: RSSI
    int rssi = LoRa.packetRssi();
    display.print("Strength:"); display.print(rssi);display.print("dBm");


    display.display();
  }
}
