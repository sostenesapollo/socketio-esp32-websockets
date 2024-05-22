#include <WiFi.h>
#include <WebSocketsClient.h>

#define USE_SERIAL Serial

const char* ssid = "TIM_2.4G"; // Replace with your WiFi SSID
const char* password = "elionmano"; // Replace with your WiFi Password
const char* websocket_server = "192.168.1.4"; // Replace with your server's IP address
const uint16_t websocket_port = 3000; // Replace with your server's port

WebSocketsClient webSocket;

void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            USE_SERIAL.printf("[WSc] Disconnected!\n");
            break;
        case WStype_CONNECTED:
            USE_SERIAL.printf("[WSc] Connected to url: %s\n", payload);
            webSocket.sendTXT("ping");
            break;
        case WStype_TEXT:
            USE_SERIAL.printf("[WSc] Received text: %s\n", payload);

            // Split the received text into action and pin
            char *action = strtok((char *)payload, ",");
            char *pin = strtok(NULL, ",");

            if (action != NULL && pin != NULL) {
                USE_SERIAL.printf("Action: [%s], Pin: [%s]\n", action, pin);

                // Perform the action based on the received values
                int pinNumber = atoi(pin);

                // Validate pin number (GPIO 0 to GPIO 39 are valid on most ESP32 boards)
                if (pinNumber >= 0 && pinNumber <= 39) {
                    pinMode(pinNumber, OUTPUT);

                    if (strcmp(action, "on") == 0) {
                        digitalWrite(pinNumber, HIGH);
                    } else if (strcmp(action, "off") == 0) {
                        digitalWrite(pinNumber, LOW);
                    } else {
                        USE_SERIAL.println("Invalid action");
                    }
                } else {
                    USE_SERIAL.println("Invalid pin number");
                }
            } else {
                USE_SERIAL.println("Invalid message format");
            }
            break;
    }
}

void setup() {
    USE_SERIAL.begin(115200); // Initialize serial communication
    USE_SERIAL.println("[SETUP] Booting...");

    // Initialize WebSocket client
    webSocket.begin(websocket_server, websocket_port, "/");
    webSocket.onEvent(webSocketEvent);

    USE_SERIAL.println("[SETUP] WebSocket connecting...");

    // Connect to WiFi
    WiFi.begin(ssid, password);
    USE_SERIAL.print("[SETUP] Connecting to WiFi");

    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        USE_SERIAL.print(".");
    }

    USE_SERIAL.println();
    USE_SERIAL.println("[SETUP] WiFi connected");
    USE_SERIAL.print("[SETUP] IP Address: ");
    USE_SERIAL.println(WiFi.localIP());
}

int countPing = 0;
int countSensor1 = 0;

void loop() {
    webSocket.loop();

    countPing++;
    if (countPing == 180000) { // Adjust this value as needed
      countPing = 0;

      USE_SERIAL.println("[LOOP] Sending ping message to server...");
      webSocket.sendTXT("ping");
    }

    countSensor1++;
    if (countSensor1 % 4000 == 0) { // Adjust this value as needed
      countSensor1 = 0;

      // Read sensor value from pin 25
      int sensorValue = analogRead(25);

      // Send sensor reading and ping message to the server
      USE_SERIAL.println("[LOOP] Sending sensor reading to server...");

      // Construct the message with sensor reading and ping
      String message = "reading,25,";
      message += sensorValue;

      // Send the message via WebSocket
      webSocket.sendTXT(message);
    }
}