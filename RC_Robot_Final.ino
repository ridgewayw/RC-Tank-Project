#include <SoftwareSerial.h>

SoftwareSerial BTserial(2, 3); // RX | TX
// Connect the HM-10 TX to the Arduino RX on pin 2. 
// Connect the HM-10 RX to the Arduino TX on pin 3. 

String receivedData;
int xValue = 0;  // Place to store x-value received from Phone
float yValue = 0.00;  // Place to store y-value received from Phone

// Define pins
const int statePin = 13; // STATE pin for connection status
const int rEnPin = 8; // Right Enable pin for Right Motor connected to BTS7960
const int lEnPin = 12; // Left Enable pin for Right Motor connected to BTS7960
const int Right_Forward = 9; // Right PWM pin for Right Motor connected to BTS7960
const int Right_Reverse = 10; // Left PWM pin for Right Motor connected to BTS7960
const int r1EnPin = 4; // Right Enable Pin for Left Motor connected to BTS7960
const int l1EnPin = 7; // Left Enable Pin for Left Motor connected to BTS7960
const int Left_Reverse = 5; // Right PWM Pin for Left Motor connected to BTS7960
const int Left_Forward = 6; // Left PWM Pin for Left Motor connected to BTS7960

// Variables for tracking STATE pin changes
int previousState = LOW; // Tracks the previous state of the STATE pin
int currentState = LOW;  // Tracks the current state of the STATE pin

// Function declaration
void forward(int speed);
void forward_right(int speed);
void forward_left(int speed);
void reverse(int speed);
void reverse_left(int speed);
void reverse_right(int speed);
void stopcar();
void right(int speed);
void left(int speed);
void controlMotors();

void setup() {
  Serial.begin(9600); // Initialize serial communication for debugging
  BTserial.begin(9600); // Initialize serial communication with the HM-10 module

  // Set pin modes
  pinMode(statePin, INPUT); // Set STATE pin as input
  pinMode(rEnPin, OUTPUT);
  pinMode(lEnPin, OUTPUT);
  pinMode(Right_Forward, OUTPUT);
  pinMode(Right_Reverse, OUTPUT);
  pinMode(r1EnPin, OUTPUT);
  pinMode(l1EnPin, OUTPUT);
  pinMode(Left_Reverse, OUTPUT);
  pinMode(Left_Forward, OUTPUT);

  // Enable the motor driver
  digitalWrite(rEnPin, HIGH);
  digitalWrite(lEnPin, HIGH);
  digitalWrite(r1EnPin, HIGH);
  digitalWrite(l1EnPin, HIGH);

  Serial.println("Arduino is ready.");
  BTserial.println("Arduino is ready.");
}

void loop() {
  // Read the current state of the STATE pin
  currentState = digitalRead(statePin);

  // Check if the state has changed
  if (currentState != previousState) {
    if (currentState == HIGH) {
      Serial.println("STATE pin switched to HIGH: Connected.");
      BTserial.println("STATE pin switched to HIGH: Connected.");
    } else if (currentState == LOW) {
      Serial.println("STATE pin switched to LOW: Disconnected.");
      BTserial.println("STATE pin switched to LOW: Disconnected.");
      stopcar(); // Stop the car when disconnected
    }

    // Update the previous state
    previousState = currentState;
  }

  // If the STATE pin is HIGH, proceed with controlling the motors
  if (currentState == HIGH) {
    if (BTserial.available()) {
      receivedData = BTserial.readStringUntil('\n');

      // Parse the joystick data
      int xIndex = receivedData.indexOf(':') + 1;
      int commaIndex = receivedData.indexOf(',');
      int endIndex = receivedData.indexOf(',', commaIndex + 1);

      if (xIndex != -1 && commaIndex != -1) {
        xValue = receivedData.substring(xIndex, commaIndex).toInt();
        yValue = receivedData.substring(commaIndex + 1, endIndex).toFloat();

        controlMotors();
      } else if (receivedData.equals("J0:0,0.0")) {
        stopcar();
        Serial.println("Stop command received.");
        BTserial.println("Stop command received.");
      }
    }
  }

  delay(30);
}

void controlMotors() {
  int speed = yValue * 255; // Convert the Y value to a range of 0-255 for speed

    if (receivedData.equals("J0:0,0.0")) {
      stopcar();
      Serial.println("Stop command received.");
      BTserial.println("Stop command received.");

  } else if (xValue <= 30 || xValue >= 330) {

    right(speed);
  } else if (xValue > 60 && xValue < 120) {

    forward(speed);
  } else if (xValue > 30 && xValue <= 60) {

    forward_right(speed);
  } else if (xValue >= 120 && xValue < 150) {

    forward_left(speed);
  } else if (xValue >= 150 && xValue < 210) {

    left(speed);
  } else if (xValue >= 240 && xValue < 300) {

    reverse(speed);
  } else if (xValue >= 210 && xValue < 240) {

    reverse_left(speed);
  } else if (xValue >= 300 && xValue < 330) {

    reverse_right(speed);
  } else {
    Serial.println("No condition met. Stopping car.");
    stopcar();
  }
}

void forward(int speed) {
  Serial.print("Forward with speed: ");
  BTserial.println("Forward with speed: ");
  Serial.println(speed);
  BTserial.println(speed);
  analogWrite(Right_Forward, speed);
  analogWrite(Right_Reverse, LOW);
  analogWrite(Left_Reverse, LOW);
  analogWrite(Left_Forward, speed);
}

void forward_left(int speed) {
  if (speed > 255) {
    speed = 170; // Clamp the speed to a maximum of 170
  }
  Serial.print("Forward left with speed: ");
  BTserial.println("Forward left with speed: ");
  Serial.println(speed);
  BTserial.println(speed);
  analogWrite(Right_Forward, speed/1.5);
  analogWrite(Right_Reverse, LOW);
  analogWrite(Left_Reverse, LOW);
  analogWrite(Left_Forward, speed/3);

}

void forward_right(int speed) {
  if (speed > 255) {
    speed = 170; // Clamp the speed to a maximum of 170
  }
  Serial.print("Forward right with speed: ");
  BTserial.println("Forward right with speed: ");
  Serial.println(speed);
  BTserial.println(speed);
  analogWrite(Right_Forward, speed/3);
  analogWrite(Right_Reverse, LOW);
  analogWrite(Left_Reverse, LOW);
  analogWrite(Left_Forward, speed/1.5);

}

void reverse(int speed) {
  if (speed > 170) {
    speed = 170; // Clamp the speed to a maximum of 170
  }
  Serial.print("Reverse with speed: ");
  BTserial.println("Reverse with speed: ");
  Serial.println(speed);
  BTserial.println(speed);
  analogWrite(Right_Forward, LOW);
  analogWrite(Right_Reverse, speed);
  analogWrite(Left_Reverse, speed);
  analogWrite(Left_Forward, LOW);
}

void reverse_left(int speed) {
  if (speed > 255) {
    speed = 255; // Clamp the speed to a maximum of 255
  }
  Serial.print("Reverse left with speed: ");
  BTserial.println("Reverse left with speed: ");
  Serial.println(speed);
  BTserial.println(speed);
  analogWrite(Right_Forward, LOW);
  analogWrite(Right_Reverse, speed/1.5);
  analogWrite(Left_Reverse, speed/3);
  analogWrite(Left_Forward, LOW);
}

void reverse_right(int speed) {
  if (speed > 170) {
    speed = 170; // Clamp the speed to a maximum of 170
  }
  Serial.print("Reverse right with speed: ");
  BTserial.println("Reverse right with speed: ");
  Serial.println(speed);
  BTserial.println(speed);
  analogWrite(Right_Forward, LOW);
  analogWrite(Right_Reverse, speed/3);
  analogWrite(Left_Reverse, speed/1.5);
  analogWrite(Left_Forward, LOW);
}

void stopcar() {
  Serial.println("Stop car");
  BTserial.println("Car is stopped");
  analogWrite(Right_Forward, LOW);
  analogWrite(Right_Reverse, LOW);
  analogWrite(Left_Reverse, LOW);
  analogWrite(Left_Forward, LOW);
}

void right(int speed) {
  if (speed > 170) {
    speed = 170; // Clamp the speed to a maximum of 170
  }
  Serial.print("Right with speed: ");
  BTserial.println("Right with speed: ");
  Serial.println(speed);
  BTserial.println(speed);
  analogWrite(Right_Forward, LOW);
  analogWrite(Right_Reverse, speed);
  analogWrite(Left_Reverse, LOW);
  analogWrite(Left_Forward, speed);
}

void left(int speed) {
  if (speed > 170) {
    speed = 170; // Clamp the speed to a maximum of 170
  }
  Serial.print("Left with speed: ");
  BTserial.println("Left with speed: ");
  Serial.println(speed);
  BTserial.println(speed);
  analogWrite(Right_Forward, speed);
  analogWrite(Right_Reverse, LOW);
  analogWrite(Left_Reverse, speed);
  analogWrite(Left_Forward, LOW);
}
