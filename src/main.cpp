// #include "main.h"
#include "General.h"
// #include "Logging.h"
#include "Tasks.h"
#include "Arduino.h"
// #include "BluetoothSerial.h"
// BluetoothSerial SerialBT;
// hw_timer_t *hwAlarmTimer = NULL;

// void IRAM_ATTR func();

void setup()
{
    pinMode(LED_CONNECTION_STATE_PIN, OUTPUT);
    pinMode(LED_BATTERY_STATE_PIN, OUTPUT);
    pinMode(LED_ALARM_PIN, OUTPUT);

    Serial.begin(115200);

    qTransmitBT = xQueueCreate(20, sizeof(MessageStruct));
    qBluetoothMac = xQueueCreate(50, sizeof(uint8_t));
    qLED = xQueueCreate(50, sizeof(uint8_t));
    qADC = xQueueCreate(QUEUE_ADC_SIZE, sizeof(float));

    xTaskCreate(taskManager_task, "Serial Commands Task", TASKMANAGER_STACK_SIZE, NULL, 1, NULL);
    xTaskCreate(alarm_task, "Alarm Task", ALARM_STACK_SIZE, NULL, 19, NULL);
    xTaskCreate(ledManager_task, "LED Task", 0x400, NULL, 10, NULL);
    // xTaskCreate(buzzManager_task, "LED Task", 0x200, NULL, 10, NULL);
    // xTimerCreate("Alarm timer", 31.25, true, NULL, Farand_Update_Alarm);

    // hwAlarmTimer = timerBegin(0, getCpuFrequencyMhz(), true);

    // timerAttachInterrupt(hwAlarmTimer, func, true);

    // timerAlarmWrite(hwAlarmTimer, 1000000 / 32, true);

    // timerAlarmEnable(hwAlarmTimer);
    // Farand_Reset_Alarm();
}
void loop()
{
    if (1)
    {
        static uint8_t i = 0;
        printf("%d ok\n", i++);
    }
    vTaskDelay(5000);
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

// // // // #define DIG_PIN_1 25
// // // // #define DIG_PIN_2 26
// // // // #define DIG_PIN_3 27

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
// // // // int8 digPins[MAX_BUTTON_COUNT] = {DIG_PIN_1, DIG_PIN_2, DIG_PIN_3};
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
// #define LED3_PIN 25
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
//     delay(1000);
//     digitalWrite(LED1_PIN, 0);
//     digitalWrite(LED2_PIN, 0);
//     digitalWrite(LED3_PIN, 0);
//     delay(500);
// }
