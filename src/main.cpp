#include <Arduino.h>
#include <ESP.h>        // Include the ESP class header

// --- Modern C++: Namespaces & Constexpr ---
// We use a namespace to group related constants. This prevents "LED_PIN" from 
// accidentally conflicting with other libraries.
namespace Config {
    // 'constexpr' tells the compiler this value is known at compile-time.
    // It is more efficient than 'const' and safer than '#define'.
    constexpr int LedPin    = 2;
}

// Global Objects
// Task handles for debugging/control
TaskHandle_t TaskHeartbeat;
TaskHandle_t TaskLogic;

// Prototypes
void heartbeatTask(void *pvParameters);
void logicTask(void *pvParameters);

void setup() {
// Standard delay for UART stability
    delay(1000); 
    Serial.begin(115200);
    
    // On hardware UART (COM?), Serial is always "true" 
    // but we wait a moment for the user to open the monitor
    delay(5000);

    Serial.println("\n--- Connected via CH343 UART (COM?) ---");
    Serial.println(  "--- ESP32-S3 Dual Core Booting --------");
    Serial.println("\n--- ESP Hardware Info------------------");
    
    // Display ESP Information

    Serial.printf("Chip ID: %u\n", ESP.getChipModel()); // Get the 24-bit chip ID
    Serial.printf("CPU Frequency: %u MHz\n", ESP.getCpuFreqMHz()); // Get CPU frequency
    Serial.printf("SDK Version: %s\n", ESP.getSdkVersion()); // Get SDK version

    // Get and print flash memory information
    Serial.printf("Flash Chip Size: %u bytes\n", ESP.getFlashChipSize()); // Get total flash size
   
    // Get and print SRAM memory information
    Serial.printf("Internal free Heap at setup: %d bytes\n", ESP.getFreeHeap());
    if(psramFound()){
        Serial.printf("Total PSRAM Size: %d bytes", ESP.getPsramSize());
    } else {
         Serial.print("No PSRAM found");
    }


    Serial.println("\n---------------------------------------");
    Serial.println("\n");

       // Configure Hardware using our Namespace
    pinMode(Config::LedPin,    OUTPUT);

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
    for(;;) {
        // Toggle the LED state
        digitalWrite(Config::LedPin, !digitalRead(Config::LedPin));
        
        // Log the status to Serial (Core 0)
        Serial.printf("[Core %d] Heartbeat - Uptime: %lu ms | LED: %s\n", 
                      xPortGetCoreID(), 
                      millis(),
                      digitalRead(Config::LedPin) ? "ON" : "OFF");
        
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