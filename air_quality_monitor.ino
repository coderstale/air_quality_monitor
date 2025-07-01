#include <Wire.h>
#include <hd44780.h>
#include <hd44780ioClass/hd44780_I2Cexp.h>

hd44780_I2Cexp lcd;

const int PM25_SENSOR_PIN = A1;
const int AIR_QUALITY_SENSOR_PIN = A2;
const int LED_PIN = 10;

bool PM25_OK = false;
bool AIR_QUALITY_OK = false;

void setup() {
    Serial.begin(115200);
    pinMode(LED_PIN, OUTPUT);

    // ✅ LCD Initialization
    if (lcd.begin(16, 2) == 0) {
        lcd.clear();
        lcd.print("Initializing...");
    } else {
        Serial.println("[ERROR] LCD Init Failed!");
    }

    delay(2000);
    lcd.clear();
    checkSensors();
}

void checkSensors() {
    int raw_PM25 = analogRead(PM25_SENSOR_PIN);
    int raw_AirQuality = analogRead(AIR_QUALITY_SENSOR_PIN);

    Serial.print("[DEBUG] PM2.5 Sensor Raw: ");
    Serial.println(raw_PM25);
    Serial.print("[DEBUG] Air Quality Sensor Raw: ");
    Serial.println(raw_AirQuality);

    PM25_OK = (raw_PM25 > 10 && raw_PM25 < 1023);
    AIR_QUALITY_OK = (raw_AirQuality > 10 && raw_AirQuality < 1023);

    lcd.clear();
    if (PM25_OK && AIR_QUALITY_OK) {
        Serial.println("[INFO] All Sensors Detected!");
        lcd.print("Sensors: OK");
    } else {
        lcd.print("Sensor Error!");
        Serial.println("[ERROR] One or more sensors not detected!");
    }
    delay(3000);
    lcd.clear();
}

bool readSensors(float &PM25, float &AQI) {
    int raw_PM25 = analogRead(PM25_SENSOR_PIN);
    int raw_AirQuality = analogRead(AIR_QUALITY_SENSOR_PIN);

    float voltage_PM25 = raw_PM25 * (5.0 / 1023.0);
    float voltage_AirQuality = 2 * raw_AirQuality * (5.0 / 1023.0);

    PM25 = max(0.0, voltage_PM25 * 100);
    AQI = max(0.0, voltage_AirQuality * 100);

    Serial.print("[DEBUG] PM2.5: ");
    Serial.print(PM25, 2);
    Serial.print(" AQI: ");
    Serial.println(AQI, 2);

    return PM25_OK && AIR_QUALITY_OK;
}

void sendData(float PM25, float AQI) {
    Serial.print("DATA:");
    Serial.print(PM25, 2);
    Serial.print(",");
    Serial.println(AQI, 2);
    Serial.flush();  // ✅ Ensure all data is sent before proceeding
}

void displayPrediction(String bucket) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("AQI Category:");
    lcd.setCursor(0, 1);
    lcd.print(bucket);
    Serial.print("[INFO] AQI Bucket: ");
    Serial.println(bucket);
    delay(5000);
}

void loop() {
    Serial.println("\n[DEBUG] Starting loop iteration...");
    float PM25 = 0.0, AQI = 0.0;

    if (!readSensors(PM25, AQI)) {
        Serial.println("[ERROR] Sensor reading failed!");
        lcd.clear();
        lcd.print("Sensor Read Err");
        delay(2000);
        return;
    }

    sendData(PM25, AQI);

    // ✅ Wait for response from Python
    long timeoutStart = millis();
    while (!Serial.available()) {
        if (millis() - timeoutStart > 5000) {  // Timeout after 5 seconds
            Serial.println("[ERROR] No response from Python!");
            lcd.clear();
            lcd.print("Python Timeout");
            return;
        }
        delay(100);
    }

    // ✅ Read response from Python
    String receivedData = Serial.readStringUntil('\n');
    receivedData.trim();

    if (receivedData.startsWith("BUCKET:")) {
        String bucket = receivedData.substring(7);
        displayPrediction(bucket);
    } else {
        Serial.println("[ERROR] Invalid response from Python!");
    }

    Serial.println("[DEBUG] Loop iteration complete.");
    delay(10000);
}