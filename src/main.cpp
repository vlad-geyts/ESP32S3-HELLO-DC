#include <Arduino.h>

// Task handles for debugging/control
TaskHandle_t TaskHeartbeat;
TaskHandle_t TaskLogic;

// Function prototypes
void heartbeatTask(void *pvParameters);
void logicTask(void *pvParameters);

void setup() {
// Standard delay for UART stability
    delay(1000); 
    
    Serial.begin(115200);
    
    // On hardware UART (COM17), Serial is always "true" 
    // but we wait a moment for the user to open the monitor
    delay(500);

    Serial.println("\n--- Connected via CH343 UART (COM17) ---");
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
    const int LED_PIN = 2; // Freenove standard onboard LED
    pinMode(LED_PIN, OUTPUT);
    
    for(;;) {
        // Toggle the LED state
        digitalWrite(LED_PIN, !digitalRead(LED_PIN));
        
        // Log the status to Serial (Core 0)
        Serial.printf("[Core %d] Heartbeat - Uptime: %lu ms | LED: %s\n", 
                      xPortGetCoreID(), 
                      millis(), 
                      digitalRead(LED_PIN) ? "ON" : "OFF");
        
        vTaskDelay(pdMS_TO_TICKS(1000)); // 1 second interval
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