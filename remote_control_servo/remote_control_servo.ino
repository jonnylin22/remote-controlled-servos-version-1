// Remote Control Servo 

#include <WiFi.h>
#include <esp_now.h>

// Receiver MAC Address
uint8_t receiverAddress[] = {0x34, 0xB7, 0xDA, 0xF6, 0x38, 0x84};
int buttonPin = 4;
volatile bool button_pressed = false;

// Parameters for filtering noise from button press
volatile unsigned long lastPressTime = 0;
const unsigned long debounceDelay = 200;

// Structure to send data
typedef struct struct_message {
    uint8_t flag; // 1 when button is pressed, 0 otherwise
} struct_message; 

// Create a struct_message called myData
struct_message myData;

// Declare a ESP peer object
esp_now_peer_info_t peerInfo;

// Callback when data is sent
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
  Serial.print("\r\nLast Packet Send Status:\t");
  Serial.println(status == ESP_NOW_SEND_SUCCESS ? "Delivery Success" : "Delivery Fail");
}

void ISR() {
  unsigned long currentTime = millis();
  if (currentTime - lastPressTime > debounceDelay) {
    button_pressed = true;
    lastPressTime = currentTime;
  }
}

void setup() {
  pinMode(buttonPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(buttonPin), ISR, FALLING);

  // Initialize Serial Monitor
  Serial.begin(115200);

  // Set device as a WiFi Station
  WiFi.mode(WIFI_STA);

  // Initialize ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Register the callback to monitor send status
  esp_now_register_send_cb(OnDataSent);

  // Register peer (Receiver device)
  memcpy(peerInfo.peer_addr, receiverAddress, 6);
  peerInfo.channel = 0;
  peerInfo.encrypt = false;

  // Add peer
  if (esp_now_add_peer(&peerInfo) != ESP_OK) {
    Serial.println("Failed to add peer");
    return;
  }

  Serial.println("Sender Ready");
  // Initialize button flag to unpressed 
  myData.flag = 0;
}


void loop() {
  
  if (button_pressed) {
    Serial.println("Button Pressed.");
    
    // Set flag to 1 when button is pressed
    myData.flag = true;

    // Send data
    esp_err_t result = esp_now_send(receiverAddress, (uint8_t *) &myData, sizeof(myData));

    // Verify successful sending
    if (result == ESP_OK) {
      Serial.println("Flag send successfully");
    } else {
      Serial.println("Error sending flag");
    }

    delay(50);
    button_pressed = false; // Reset flag after sending
  }
  
}
