#include <Arduino.h>

const int ITEMCOUNT = 3; // total message that could be sent
const int SENDBUFFERSIZE = ITEMCOUNT * 3; // Buffer array size.
const int RECEIVEBUFFERSIZE = 3; // Buffer array size.

byte sendBuffer[SENDBUFFERSIZE]; // Buffer for sending data to Raspberry Pi
byte receiveBuffer[RECEIVEBUFFERSIZE]; // Buffer for receiving data from Raspberry Pi

uint16_t valueUi16;
float valueFloat32;
bool valueStatus;

void sendArduinoInfo();
void getMovementData();
void listenRaspberry();
void sendSerial();
bool readSerial();
void setInductionMotorPWM(uint16_t);
void setBrakes();
void setLevitation();
bool messageEncoder();


void setup() {
  Serial.begin(115200);
  while (receiveBuffer[0] == 0xf) {readSerial();} // waits till raspberry sends start command
  sendArduinoInfo();
  getMovementData();
  sendSerial();
  delay(100);
  listenRaspberry();
  listenRaspberry();
  listenRaspberry();
  sendSerial();
}

void loop() {
}

void getMovementData() {
  sendBuffer[0 + 0 * 3] = 0x01;
  sendBuffer[1 + 0 * 3] = 0x03;
  sendBuffer[2 + 0 * 3] = 0x01;

  sendBuffer[0 + 1 * 3] = 0x0A;
  sendBuffer[1 + 1 * 3] = 0x00;
  sendBuffer[2 + 1 * 3] = 0x00; // currently not breaking!

  sendBuffer[0 + 2 * 3] = 0x0B;
  sendBuffer[1 + 2 * 3] = 0x00;
  sendBuffer[2 + 2 * 3] = 0x01; // Levitating (feat. DaBaby) - Dua Lipa
}

void sendArduinoInfo() {
    byte sensorduinoInfoBuffer[3] = {0x0D, 0x0, 0x0};
    Serial.write(sensorduinoInfoBuffer, 3);
}


// sends the sendBuffer to Raspberry Pi
void sendSerial() {
  Serial.write(sendBuffer, SENDBUFFERSIZE);
}

// returns true if the receiveBuffer is changed after call,
// returns false if the receiveBuffer is not changed.
bool readSerial() {
  if (Serial.available() >= RECEIVEBUFFERSIZE) {
      Serial.readBytes(receiveBuffer, RECEIVEBUFFERSIZE);
      return true;
  }
  return false;
}

// if there is something coming from raspberry, executes it
// else nothing happens.
void listenRaspberry() {
  if (readSerial()) {
    messageEncoder(); // Runs the command that raspberry sent.
  }
}

// message 		byte1(op)   byte2   byte3
//
// Sets the PWM of the induction motor.
// Byte2 and byte3 combined is the uint16 value.
// induction    0x01         uint16
//
// Sets the current status depending on the last bit.
// if the last bit is 0 sets brake/levitation to off
// if the last bit is 1 sets brake/levitation to on
// brake 	  	  0x0A      0x00 	  uint8
// levitation   0x0B      0x00 	  uint8
bool messageEncoder() {
  if (receiveBuffer[0] == 0x01) {
    uint16_t newPWM = (receiveBuffer[1] << 8) | receiveBuffer[2];
    setInductionMotorPWM(newPWM);
  } else if (receiveBuffer[0] == 0x0A) {
    setBrakes();
  } else if (receiveBuffer[0] == 0x0B) {
    setLevitation();
  } else {
    return false;
  }

  return true;
}

void setInductionMotorPWM(uint16_t newPWM) {
  sendBuffer[0 + 0 * 3] = 0x01;
  sendBuffer[1 + 0 * 3] = receiveBuffer[1];
  sendBuffer[2 + 0 * 3] = receiveBuffer[2];
}

void setBrakes() {
  sendBuffer[0 + 1 * 3] = 0x0A;
  sendBuffer[1 + 1 * 3] = 0x00;
  sendBuffer[2 + 1 * 3] = receiveBuffer[2];
}

void setLevitation() {
  sendBuffer[0 + 2 * 3] = 0x0B;
  sendBuffer[1 + 2 * 3] = 0x00;
  sendBuffer[2 + 2 * 3] = receiveBuffer[2];
}
