/*─────────────────────────────────────────────────────────────────────────────
    LoRa Sender Program using ESP32
    → Sends text typed in Serial Monitor to another LoRa receiver.
──────────────────────────────────────────────────────────────────────────────

  ■ Components Used
    - ESP32
    - LoRa SX1278 Module (433 MHz)

  ■ Pin Connections (LoRa → ESP32)
    --------------------------------
    | LoRa Pin |   ESP32 Pin       |
    |----------|-------------------|
    | NSS / SS | GPIO 5            |
    | RST      | GPIO 14           |
    | DIO0     | GPIO 2            |
    | MOSI     | GPIO 23           |
    | MISO     | GPIO 19           |
    | SCK      | GPIO 18           |
    | 3.3V     | 3.3V              |
    | GND      | GND               |
    --------------------------------

  ■ What This Program Does
    - Waits for a message typed in Serial Monitor.
    - Sends the message wirelessly over LoRa.
    - Configures LoRa for:
         • Maximum range
         • Best sensitivity
         • CRC error-checking
    - Rejects corrupted packets automatically.

  ■ Code Flow Summary

    ● setup()
      - Start Serial @ 9600
      - Show instructions
      - Configure LoRa pins (SS, RST, DIO0)
      - Initialize LoRa at 433 MHz
      - Apply long-range settings:
           • Spreading Factor: SF12
           • Bandwidth: 125 kHz
           • Coding Rate: 4/8
           • Preamble Length: 8
           • CRC: Enabled
           • TX Power: 20 dBm

    ● loop()
      - Check Serial input
      - Read full line of text
      - If >250 chars → show error
      - Begin LoRa packet
      - Send the message
      - End packet and transmit
      - Print "Message sent"

──────────────────────────────────────────────────────────────────────────────*/


#include <SPI.h>
#include <LoRa.h>

// LoRa Pins for ESP32
#define LORA_SS    5
#define LORA_RST   14
#define LORA_DIO0  2

String outgoingMessage = "";

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("ESP32 LoRa Sender Ready");
  Serial.println("Type a message below and press ENTER to send via LoRa.");
  Serial.println("Max length: 250 characters\n");

  // LoRa init
  LoRa.setPins(LORA_SS, LORA_RST, LORA_DIO0);

  Serial.println("Initializing LoRa...");
  if (!LoRa.begin(433E6)) {
    Serial.println("LoRa init failed! Check wiring.");
    while (1);
  }
    LoRa.enableCrc();
LoRa.setSpreadingFactor(12);        // SF12 = max range
LoRa.setSignalBandwidth(125E3);     // 125 kHz = best sensitivity
LoRa.setCodingRate4(8);             // 4/8 = highest error correction
LoRa.setPreambleLength(8);          // default, stable
LoRa.enableCrc();                   // reject corrupted packets
LoRa.setTxPower(20);
}

void loop() {
  // Check for input from serial monitor
  if (Serial.available()) {
    outgoingMessage = Serial.readStringUntil('\n');  // Read full line

    // Safety check
    if (outgoingMessage.length() > 250) {
      Serial.println("Error: Message too long! Max = 250 chars.");
      return;
    }

    // Send message via LoRa
    Serial.print("Sending LoRa message: ");
    Serial.println(outgoingMessage);

    LoRa.beginPacket();
    LoRa.print(outgoingMessage);
    LoRa.endPacket();

    Serial.println("Message sent!\n");
  }
}
