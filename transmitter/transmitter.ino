/* Transmitter Code with Serial Debugging
 * Board: Arduino UNO/NANO/Pro Mini (5V, 16MHz)
 * NRF24L01 Wiring:
 *   GND  -> GND
 *   VCC  -> 3.3V (use regulator)
 *   CE   -> D9
 *   CSN  -> D10
 *   SCK  -> D13
 *   MOSI -> D11
 *   MISO -> D12
 */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// RF24 setup
RF24 radio(9, 10); // CE, CSN
const byte address[6] = "00001";

// Joystick pins
const int xPin = A1;
const int yPin = A0;

void setup() {
  Serial.begin(9600);
  Serial.println("Transmitter Starting...");

  radio.begin();
  radio.openWritingPipe(address);
  radio.setPALevel(RF24_PA_MIN);
  radio.stopListening(); // Set as transmitter

  Serial.println("NRF24 Initialized. Sending data...");
}

void loop() {
  int xAxis = analogRead(xPin); // 0–1023
  int yAxis = analogRead(yPin); // 0–1023

  String message = String(xAxis) + "," + String(yAxis);
  bool report = radio.write(message.c_str(), message.length() + 1);

  // Debug
  Serial.print("Sent: ");
  Serial.print(message);
  Serial.print(" | Status: ");
  Serial.println(report ? "Success" : "Failed");

  delay(200); // Short delay for stability
}
