#include "General.h"
#include "Tasks.h"
#include "Arduino.h"

static uint8_t getPGood()
{
    uint8_t pgood;
    uint32_t pgoodVal = 0;
    if (BT_SSID == BT_SSID_1)
    {
        pgoodVal = analogReadMilliVolts(ANALOG_PIN_PGOOD_N);
        pgoodVal += analogReadMilliVolts(ANALOG_PIN_PGOOD_N);
        pgoodVal += analogReadMilliVolts(ANALOG_PIN_PGOOD_N);
        pgoodVal += analogReadMilliVolts(ANALOG_PIN_PGOOD_N);
        pgoodVal += analogReadMilliVolts(ANALOG_PIN_PGOOD_N);
        pgoodVal = pgoodVal / 5;
        pgood = pgoodVal > PGOOD_THRESHOLD;
        Serial.printf("ssid1 pgood is %d\n", pgood);
        return pgood;
    }
    else
    {
        pgood = digitalRead(DIGITAL_PIN_CHARGER_SENSE) == HIGH ? LOW : HIGH;
        Serial.printf("ssid2 pgood is %d\n", pgood);
        return pgood;
    }
}
uint16_t batmV_GLOBAL;
void setup()
{
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    pinMode(BUZZER_PIN, OUTPUT); ///////////////////
    pinMode(BATTERY_SENSE_PIN, INPUT);
    pinMode(ANALOG_PIN_PGOOD_N, INPUT);
    pinMode(DIGITAL_PIN_CHARGING_N, INPUT);
    pinMode(DIGITAL_PIN_CHARGER_SENSE, INPUT);

    tone(BUZZER_PIN, 2000, 50);

    Serial.begin(115200);

    qTransmitBT = xQueueCreate(100, sizeof(MessageStruct));
    qBluetoothMac = xQueueCreate(50, sizeof(uint8_t));
    qLED = xQueueCreate(50, sizeof(ledStateStruct));
    qADC = xQueueCreate(QUEUE_ADC_SIZE, sizeof(float));
    delay(1000);

    uint8_t pgood = getPGood();

    uint8_t charging = digitalRead(DIGITAL_PIN_CHARGING_N);
    charging = digitalRead(DIGITAL_PIN_CHARGING_N);

    Serial.printf("pgood is %d\n", pgood);
    TaskHandle_t readBatteryWhileCharging_taskHandle;
    xTaskCreate(readBatteryWhileCharging_task, "charging task", LEDMANAGER_STACK_SIZE, NULL, 10, &readBatteryWhileCharging_taskHandle); /////////////////////
    int now = millis();
    while (pgood == LOW)
    {

        delay(1);
        static uint8_t led = 0;
        
        if (millis() - now > 1000)
        {
            now = millis();
            if (led == 0 && charging == LOW)
            {
                digitalWrite(LED_GREEN, HIGH);
                digitalWrite(LED_RED, LOW);
                digitalWrite(LED_YELLOW, LOW);
                led = 1;
            }
            else if (led == 1 && charging == LOW)
            {
                digitalWrite(LED_GREEN, LOW);
                digitalWrite(LED_RED, LOW);
                digitalWrite(LED_YELLOW, LOW);
                led = 0;
            }
            else if (charging == HIGH && batmV_GLOBAL == 0 && led == 1)
            {
                digitalWrite(LED_GREEN, LOW);
                digitalWrite(LED_RED, HIGH);
                digitalWrite(LED_YELLOW, LOW);
                led = 0;
            }
            else if (charging == HIGH && batmV_GLOBAL == 0 && led == 0)
            {
                digitalWrite(LED_GREEN, LOW);
                digitalWrite(LED_RED, LOW);
                digitalWrite(LED_YELLOW, LOW);
                led = 1;
            }
            else if (charging == HIGH)
            {
                digitalWrite(LED_GREEN, HIGH);
                digitalWrite(LED_RED, LOW);
                digitalWrite(LED_YELLOW, LOW);
            }
            pgood = getPGood();
            charging = digitalRead(DIGITAL_PIN_CHARGING_N);
            // batmV = 2 * analogReadMilliVolts(BATTERY_SENSE_PIN);
            if (pgood)
            {
                Serial.println("got to 6");
                ESP.restart();
            }
            // Serial.println("got to 7");
            Serial.printf("Charger Sense:%d - Battery Voltage: %d mFV - pgood: %d\n", pgood, batmV_GLOBAL, analogReadMilliVolts(ANALOG_PIN_PGOOD_N));
        }
    }
    vTaskDelete(readBatteryWhileCharging_taskHandle);
    xTaskCreate(taskManager_task, "Serial Commands Task", TASKMANAGER_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(alarm_task, "Alarm Task", ALARM_STACK_SIZE, NULL, 19, NULL);
    xTaskCreate(ledManager_task, "LED Task", LEDMANAGER_STACK_SIZE, NULL, 10, NULL); /////////////////////
    
}
void loop()
{
    if (1) ////////////////////////
    {
        static uint8_t i = 0;
        printf("Battery: %f - %d ok\n", Joystick.battery, i++);
    }
    vTaskDelay(5000);
    Serial.printf("connection: %d\n", Joystick.isConnected); //////////////////////
    
}
