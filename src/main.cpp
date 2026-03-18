#include <Arduino.h>

// Task handles for debugging/control
TaskHandle_t TaskHeartbeat;
TaskHandle_t TaskLogic;

// Function prototypes
void heartbeatTask(void *pvParameters);
void logicTask(void *pvParameters);

void setup() {
    Serial.begin(115200);
    while(!Serial); // Wait for USB Serial to initialize (typical for S3)

    Serial.println("\n--- ESP32-S3 Dual Core Booting ---");
    
    // Display Hardware Info
    Serial.printf("Internal Heap: %d bytes\n", ESP.getFreeHeap());
    Serial.printf("PSRAM Size: %d bytes\n", ESP.getPsramSize());

    // Create Heartbeat Task on Core 0 (System Core)
    xTaskCreatePinnedToCore(
        heartbeatTask,   // Task function
        "Heartbeat",     // Name
        4096,            // Stack size
        NULL,            // Parameters
        1,               // Priority
        &TaskHeartbeat,  // Task handle
        0                // Core ID
    );

    // Create Logic Task on Core 1 (App Core)
    xTaskCreatePinnedToCore(
        logicTask,
        "Logic",
        4096,
        NULL,
        2,               // Higher priority
        &TaskLogic,
        1                // Core ID
    );
}

void loop() {
    // In FreeRTOS, we leave loop() empty or use it for low-priority cleanup.
    vTaskDelete(NULL); 
}

// --- Task Implementations ---

void heartbeatTask(void *pvParameters) {
    pinMode(LED_BUILTIN, OUTPUT); // Note: verify your specific S3 board LED pin
    for(;;) {
        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
        Serial.printf("[Core %d] Heartbeat - Uptime: %lu ms\n", xPortGetCoreID(), millis());
        vTaskDelay(pdMS_TO_TICKS(1000)); // Non-blocking delay
    }
}

void logicTask(void *pvParameters) {
    for(;;) {
        // This is where you'd poll your SPI/I2C sensors
        Serial.printf("[Core %d] Processing Application Logic...\n", xPortGetCoreID());
        
        // Use a shorter delay for high-frequency tasks
        vTaskDelay(pdMS_TO_TICKS(500)); 
    }
}