#include <WiFi.h>
#include <WebSocketsClient.h>
#include <ArduinoJson.h>

#define USE_SERIAL Serial

const char* ssid = "TIM_2.4G"; 
const char* password = "pass"; 
const char* websocket_server = "192.168.1.5";
const uint16_t websocket_port = 3000; 

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
    // Inicialize o pino 2 como entrada
    pinMode(2, INPUT);

    // Inicialize os pinos 18 e 19 como saída
    pinMode(18, OUTPUT);
    pinMode(19, OUTPUT);
    
    USE_SERIAL.begin(9600); // Initialize serial communication
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

unsigned long lastSensorReadTime = 0; // Store the last sensor read time
const unsigned long sensorReadInterval = 500; // Interval between sensor reads in milliseconds


void loop() {
    countPing++;
    if (countPing == 180000) { // Adjust this value as needed
        countPing = 0;

        USE_SERIAL.println("[LOOP] Sending ping message to server...");
        webSocket.sendTXT("ping");
    }

    // Check if it's time to read the sensor
    unsigned long currentMillis = millis();
    if (currentMillis - lastSensorReadTime >= sensorReadInterval) {
        lastSensorReadTime = currentMillis;
        int value32 = analogRead(32);
        USE_SERIAL.println("[LOOP] Sending sensors [32] value:"+String(value32));
        webSocket.sendTXT("reading,32,"+String(value32));

        int value35 = analogRead(35);
        USE_SERIAL.println("[LOOP] Sending sensors [35] value:"+String(value35));
        webSocket.sendTXT("reading,35,"+String(value35));
    }

    if (currentMillis - lastSensorReadTime >= sensorReadInterval*4) {
      USE_SERIAL.println("[LOOP] Sending sensors readings to server...");
    }

    webSocket.loop();
}