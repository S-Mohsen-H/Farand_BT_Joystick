// #include "main.h"
#include "General.h"
// #include "Logging.h"
#include "Tasks.h"
#include "Arduino.h"
// #include "BluetoothSerial.h"
// BluetoothSerial SerialBT;
// hw_timer_t *hwAlarmTimer = NULL;

// void IRAM_ATTR func();
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
    // pinMode(ADC_PIN_3, INPUT);
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
    // qBatteryCharging = xQueueCreate(100, sizeof(uint8_t));
    delay(1000);

    uint8_t pgood = getPGood();

    uint8_t charging = digitalRead(DIGITAL_PIN_CHARGING_N);
    charging = digitalRead(DIGITAL_PIN_CHARGING_N);
    // Serial.println("got to 1");

    // uint16_t batmV = 2 * analogReadMilliVolts(BATTERY_SENSE_PIN);
    // batmV = 0;
    Serial.printf("pgood is %d\n", pgood);
    TaskHandle_t readBatteryWhileCharging_taskHandle;
    xTaskCreate(readBatteryWhileCharging_task, "charging task", LEDMANAGER_STACK_SIZE, NULL, 10, &readBatteryWhileCharging_taskHandle); /////////////////////
    int now = millis();
    while (pgood == LOW)
    {

        delay(1);
        static uint8_t led = 0;
        // static uint64_t timer = 0;
        // timer++;
        // batmV = 2 * analogReadMilliVolts(BATTERY_SENSE_PIN);
        // if (batmV < 2000)
        // {
        //     timer = 0;
        //     batmV = 0;
        // }
        // if (timer > 900)
        // {
        //     batmV = 2 * analogReadMilliVolts(BATTERY_SENSE_PIN);
        // }
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
    // vQueueDelete(qBatteryCharging);
    xTaskCreate(taskManager_task, "Serial Commands Task", TASKMANAGER_STACK_SIZE, NULL, 2, NULL);
    xTaskCreate(alarm_task, "Alarm Task", ALARM_STACK_SIZE, NULL, 19, NULL);
    xTaskCreate(ledManager_task, "LED Task", LEDMANAGER_STACK_SIZE, NULL, 10, NULL); /////////////////////
    // esp_task_wdt_init(CONFIG_ESP_TASK_WDT_TIMEOUT_S - 3, CONFIG_ESP_TASK_WDT_PANIC);
    // xTimerCreate("Alarm timer", 31.25, true, NULL, Farand_Update_Alarm);

    // hwAlarmTimer = timerBegin(0, getCpuFrequencyMhz(), true);

    // timerAttachInterrupt(hwAlarmTimer, func, true);

    // timerAlarmWrite(hwAlarmTimer, 1000000 / 32, true);

    // timerAlarmEnable(hwAlarmTimer);
    // Farand_Reset_Alarm();
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
    // if (doIt)
    //     tone(BUZZER_PIN, 1);
    // else
    //     digitalWrite(BUZZER_PIN, 0);
}
// bool initQT()
// {

// }
// IRAM_ATTR void func()
// {
//     Farand_Update_Alarm();
// }

// // // // //This example code is in the Public Domain (or CC0 licensed, at your option.)
// // // // //By Evandro Copercini - 2018
// // // // //
// // // // //This example creates a bridge between Serial and Classical Bluetooth (SPP)
// // // // //and also demonstrate that SerialBT have the same functionalities of a normal Serial
// // // // #include "Arduino.h"
// // // // #include "BluetoothSerial.h"

// // // // #if !defined(CONFIG_BT_ENABLED_PIN) || !defined(CONFIG_BLUEDROID_ENABLED_PIN)
// // // // #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// // // // #endif

// // // // BluetoothSerial SerialBT;

// // // // void setup() {
// // // //   Serial.begin(115200);
// // // //   SerialBT.begin("ESP32test"); //Bluetooth device name
// // // //   Serial.println("The device started, now you can pair it with bluetooth!");
// // // // }

// // // // void loop() {
// // // //   if (Serial.available()) {
// // // //     SerialBT.write(Serial.read());
// // // //   }
// // // //   if (SerialBT.available()) {
// // // //     Serial.write(SerialBT.read());
// // // //   }
// // // //   delay(20);
// // // // }

// // // // #include "Arduino.h"
// // // // #include "BluetoothSerial.h"
// // // // #define UART_DEBUG_MODE 1

// // // // #ifndef int8
// // // // #define int8 uint8_t
// // // // #endif
// // // // #ifndef int16
// // // // #define int16 uint16_t
// // // // #endif

// // // // #define ADC_PIN_1 36
// // // // #define ADC_PIN_2 39
// // // // #define ADC_PIN_3 34

// // // // #define ADC_COUNT 3
// // // // #define MAX_ADC_COUNT 3

// // // // #define DIGITAL_PIN_1 25
// // // // #define DIGITAL_PIN_2 26
// // // // #define DIGITAL_PIN_3 27

// // // // #define BUTTON_COUNT 1
// // // // #define MAX_BUTTON_COUNT 3

// // // // // #define BYTE_ARRAY_SIZE (ADC_COUNT * 1.5 + 1 + 2)
// // // // #define BYTE_ARRAY_SIZE 8

// // // // #define ACTION_MODE 0
// // // // #define COMMAND_MODE_SERIAL 1
// // // // #define COMMAND_MODE_PACKET 2
// // // // #define COMMAND_PACKET_SIZE 2

// // // // #define BT_INITIAL_TRANSMIT_RATE_MS 100

// // // // #define USING_MULTI_SAMPLING
// // // // #define SAMPLE_COUNT 50
// // // // #define INITIAL_SAMPLING_COEFFICIENT 0.8
// // // // #define LED_PIN 23
// // // // int8 adcPins[MAX_ADC_COUNT] = {ADC_PIN_1, ADC_PIN_2, ADC_PIN_3};
// // // // int8 digPins[MAX_BUTTON_COUNT] = {DIGITAL_PIN_1, DIGITAL_PIN_2, DIGITAL_PIN_3};
// // // // float alpha;
// // // // bool b;
// // // // BluetoothSerial SerialBT;

// // // // void setup()
// // // // {
// // // //     SerialBT.begin("ESP32_Joystick");
// // // //     Serial.begin(115200);

// // // //     for (int i = 0; i < BUTTON_COUNT; i++)
// // // //     {
// // // //         printf("1:%d\n", i);
// // // //         // gpio_set_direction(digPins[i],inputonly);
// // // //         pinMode(digPins[i], INPUT);
// // // //         printf("2:%d\n", i);
// // // //     }
// // // //     pinMode(LED_PIN, OUTPUT);
// // // //     digitalWrite(LED_PIN, 1);
// // // //     vTaskDelay(100);
// // // //     digitalWrite(LED_PIN, 0);
// // // // }
// // // // void loop()
// // // // {
// // // //     int adc[ADC_COUNT];
// // // //     int temp = 0;
// // // //     int foop = 0;
// // // //     int dig[BUTTON_COUNT];
// // // //     static int prev[ADC_COUNT];
// // // //     // foop = Serial.available();
// // // //     if (Serial.available())
// // // //     {
// // // //         printf("enter alpha\n");

// // // //         foop = Serial.read();
// // // //         switch (foop)
// // // //         {
// // // //         case '1':
// // // //             for (int i = 0; i < 20; i++)
// // // //                 Serial.println((SerialBT.connected()) ? " BLUETOOTH CONNECTED" : "NOT CONNECTED");
// // // //             break;
// // // //         case '2':
// // // //             alpha = 0.95;
// // // //             break;
// // // //         case '3':
// // // //             alpha = 0.9;
// // // //             break;
// // // //         case '4':
// // // //             alpha = 0.7;
// // // //             break;
// // // //         case '5':
// // // //             alpha = 0.5;
// // // //             break;
// // // //         case '6':
// // // //             alpha = 0.3;
// // // //             break;
// // // //         case '7':
// // // //             alpha = 0.2;
// // // //             break;
// // // //         case '8':
// // // //             alpha = 0.1;
// // // //         default:
// // // //             break;
// // // //         }
// // // //     }

// // // //     for (int i = 0; i < BUTTON_COUNT; i++)
// // // //     {
// // // //         dig[i] = digitalRead(digPins[i]);
// // // //         printf("%d  -", dig[i]);
// // // //         SerialBT.printf("%d  -", dig[i]);
// // // //     }
// // // //     printf("\n");
// // // //     digitalWrite(26, b);
// // // //     b = b ^ 0;
// // // //     vTaskDelay(100);
// // // // }

// // // // #include "Arduino.h"
// // // // void setup()
// // // // {
// // // //     Serial.begin(115200);
// // // // }
// // // // void loop()
// // // // {
// // // //     for (int i = 0; i < 100; i++)
// // // //     {
// // // //         Serial.printf("%d: ", i);
// // // //         Serial.println(i % 2);
// // // //         vTaskDelay(100);
// // // //     }
// // // // }

// #include "Arduino.h"
// #include "Alarm_FreeRTOS.h"
// #include "BluetoothSerial.h"
// BluetoothSerial serialBT;
// byte commBT[16];
// #define BUZZER_PIN 19

// void setup()
// {
//     pinMode(BUZZER_PIN, OUTPUT);
//     pinMode(21, OUTPUT);
//     pinMode(23, OUTPUT);

//     pinMode(14, INPUT);

//     tone(BUZZER_PIN, 2000);
//     serialBT.begin("ESP32_Joystick", false);
// }
// void loop()
// {
//     if (serialBT.available())
//     {
//         serialBT.readBytes(commBT, 16);
//         if (commBT[8] == 0xC2)
//         {
//             alarmMessage_typeDef alarmMessage;
//             alarmMessage.BeepOnOff = 1;
//             alarmMessage.AlarmCount = commBT[8 + 5];
//             alarmMessage.Pattern = commBT[8 + 4];
//             alarmMessage.Pattern |= commBT[8 + 3] << 8;
//             alarmMessage.Pattern |= commBT[8 + 2] << 16;
//             alarmMessage.Pattern |= commBT[8 + 1] << 24;
//             Farand_Alarm(alarmMessage.Pattern, alarmMessage.AlarmCount, 32, 1);
//             digitalWrite(21, 1);
//         }
//         else
//             digitalWrite(21, 0);
//     }
//     Farand_Update_Alarm(BUZZER_PIN);
//     vTaskDelay(32.25);
// }

// // #include "Arduino.h"
// // #define LED 23
// // #define LED2 22
// // #define LED3 21

// // hw_timer_t *My_timer = NULL;
// // hw_timer_t *My_timer2 = NULL;
// // hw_timer_t *My_timer3 = NULL;
// // void IRAM_ATTR onTimer()
// // {
// //     digitalWrite(LED, !digitalRead(LED));
// // }
// // void IRAM_ATTR onTimer2()
// // {
// //     digitalWrite(LED2, !digitalRead(LED2));
// // }
// // void IRAM_ATTR onTimer3()
// // {
// //     digitalWrite(LED3, !digitalRead(LED3));
// // }

// // void setup()
// // {
// //     pinMode(LED, OUTPUT);
// //     pinMode(LED2, OUTPUT);
// //     pinMode(LED3, OUTPUT);

// //     My_timer = timerBegin(0, getCpuFrequencyMhz(), true);
// //     My_timer2 = timerBegin(1, 80, true);
// //     My_timer3 = timerBegin(2, getCpuFrequencyMhz(), true);

// //     timerAttachInterrupt(My_timer, &onTimer, true);
// //     timerAttachInterrupt(My_timer2, &onTimer2, true);
// //     timerAttachInterrupt(My_timer3, &onTimer3, true);

// //     timerAlarmWrite(My_timer, 1000000, true);
// //     timerAlarmWrite(My_timer2, 1000000, true);
// //     timerAlarmWrite(My_timer3, 1000000 / 32, true);

// //     timerAlarmEnable(My_timer);  // Just Enable
// //     timerAlarmEnable(My_timer2); // Just Enable
// //     timerAlarmEnable(My_timer3); // Just Enable
// // }
// // void loop()
// // {
// //     vTaskDelay(1000000);
// // }
// #include "Arduino.h"
// #define LED1_PIN 23
// #define LED2_PIN 18
// #define LED3_PIN 12

// void setup()
// {
//     Serial.begin(115200);
//     Serial.print("Hello\n");
//     pinMode(LED1_PIN, OUTPUT);
//     pinMode(LED2_PIN, OUTPUT);
//     pinMode(LED3_PIN, OUTPUT);
//     digitalWrite(LED1_PIN, 1);
//     digitalWrite(LED2_PIN, 1);
//     digitalWrite(LED3_PIN, 1);
// }
// void loop()
// {
//     digitalWrite(LED1_PIN, 1);
//     digitalWrite(LED2_PIN, 1);
//     digitalWrite(LED3_PIN, 1);
//     Serial.print("Hello1\n");
//     delay(1000);
//     digitalWrite(LED1_PIN, 0);
//     digitalWrite(LED2_PIN, 0);
//     digitalWrite(LED3_PIN, 0);
//     Serial.print("Hello2\n");
//     delay(500);
// }
