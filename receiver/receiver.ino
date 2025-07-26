#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

// RF24 setup
RF24 radio(9, 10);  // CE, CSN
const byte address[6] = "00001";

char receivedData[32] = "";
int xAxis = 590, yAxis = 590;

unsigned long lastSignalTime = 0;
const unsigned long SIGNAL_TIMEOUT = 500;

void setup() {
  pinMode(8, OUTPUT);  // in1
  pinMode(7, OUTPUT);  // in2
  pinMode(4, OUTPUT);  // in3
  pinMode(3, OUTPUT);  // in4

  Serial.begin(9600);
  Serial.println("Receiver Initialized - Full Speed Control with Direction Only");

  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
}

void loop() {
  if (radio.available()) {
    radio.read(&receivedData, sizeof(receivedData));
    Serial.print("Received: ");
    Serial.println(receivedData);

    char *xStr = strtok(receivedData, ",");
    char *yStr = strtok(NULL, ",");

    if (xStr && yStr) {
      xAxis = atoi(xStr);
      yAxis = atoi(yStr);
      lastSignalTime = millis();
      Serial.print("Joystick X: "); Serial.print(xAxis);
      Serial.print(" | Y: "); Serial.println(yAxis);
    }
  }

  if (millis() - lastSignalTime > SIGNAL_TIMEOUT) {
    stopMotors();
    Serial.println("⚠️ Signal Lost - Motors Stopped");
    return;
  }

  handleJoystick(xAxis, yAxis);
}

void handleJoystick(int x, int y) {
  bool forward = false;
  bool backward = false;
  bool turning = false;

  if (y > 600) forward = true;
  else if (y < 500) backward = true;

  if (x < 500 || x > 600) turning = true;

  if (forward && !turning) {
    setMotorDir(true, true);
    Serial.println("➡️ Forward");
  } else if (backward && !turning) {
    setMotorDir(false, false);
    Serial.println("⬅️ Backward");
  } else if (turning && !forward && !backward) {
    if (x < 500) {
      setMotorDir(true, false);  // Pivot Right
      Serial.println("↪️ Pivot Right");
    } else {
      setMotorDir(false, true);  // Pivot Left
      Serial.println("↩️ Pivot Left");
    }
  } else if (turning && (forward || backward)) {
    if (forward) {
      setMotorDir(true, true);
      Serial.println("🔄 Forward + Turn");
    } else {
      setMotorDir(false, false);
      Serial.println("🔁 Backward + Turn");
    }
  } else {
    stopMotors();
  }
}

void setMotorDir(bool leftFwd, bool rightFwd) {
  digitalWrite(8, leftFwd ? LOW : HIGH);   // in1
  digitalWrite(7, leftFwd ? HIGH : LOW);   // in2
  digitalWrite(4, rightFwd ? LOW : HIGH);  // in3
  digitalWrite(3, rightFwd ? HIGH : LOW);  // in4
  Serial.println("Function calling");
}

void stopMotors() {
  digitalWrite(8, LOW);  // in1
  digitalWrite(7, LOW);  // in2
  digitalWrite(4, LOW);  // in3
  digitalWrite(3, LOW);  // in4
}
