// remote controlled servo - receiver program

#include <esp_now.h>
#include <WiFi.h>
#include <ESP32Servo.h>

// Declare a servo object
Servo servo1;
int servo1_pin = 4;

Servo servo2;
int servo2_pin = 10;

// Structure to receive data
typedef struct struct_message {
  uint8_t message;
} struct_message;

// Create a struct_message called myData
struct_message myData;

esp_now_peer_info_t peerInfo;
// Flag to indicate new data
volatile bool newDataReceived = false;

// Servo control variables
int servo1_angle = 0;   // Stores the last angle of servo1
int servo2_angle = 0;   // Stores the last angle of servo2
bool toggleServo = false; // Tracks which servo moves next

// Callback when data is received
void onDataRecv(const esp_now_recv_info_t *info, const uint8_t *data, int len) {
  memcpy(&myData, data, sizeof(myData));  // Copy received data to myData
  Serial.print("Received Message: ");
  Serial.println( (myData.message == 1) ? "Button pressed, activate servo." : "Button not pressed.");
  newDataReceived = true;
}

void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a WiFi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the callback to handle incoming messages
  esp_now_register_recv_cb(onDataRecv);

  // Initialize the Servo motor
  servo1.attach(servo1_pin);
  servo2.attach(servo2_pin);

  Serial.println("Receiver Ready");

}

void loop() {
  if (newDataReceived && myData.message == 1) {
    // Reset flag after processing
    newDataReceived = false; 
    if (toggleServo) {
      // Move Servo 1 // Toggle between 0 and 35 degrees
      servo1_angle = 35;  // 35 degrees is angle needed to hit light switch

      servo1.write(servo1_angle);
      Serial.print("Moving Servo 1 to: "); Serial.println(servo1_angle);
      // Delay to allow light switch to flip
      delay(250); 
      servo1.write(0);
    } else {
      // Move Servo 2 // Toggle between 0 and 90 degrees
      servo2_angle = (servo2_angle == 0) ? 45 : 0; 
      servo2_angle = 35;

      servo2.write(servo2_angle);
      Serial.print("Moving Servo 2 to: "); Serial.println(servo2_angle);
      // Delay to allow light switch to flip
      delay(250);
      servo2.write(0);
    }
    // Toggle the bool state to flip the other side of the switch
    toggleServo = !toggleServo;
  }
  // Small delay for loop stability
  delay(50);
}
