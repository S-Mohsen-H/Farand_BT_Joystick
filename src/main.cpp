// #include "main.h"
#include "General.h"
// #include "Logging.h"
#include "Tasks.h"
#include "Arduino.h"
#include "BluetoothSerial.h"
// BluetoothSerial SerialBT;
void setup()
{

    Serial.begin(115200);
    qUART = xQueueCreate(10, sizeof(MessageStruct));
    qSerialCommands = xQueueCreate(50, sizeof(uint8_t));
    qADC = xQueueCreate(5, sizeof(float));
    // qTaskManager = xQueueCreate(10, sizeof(uint8_t));

    xTaskCreate(serialCommandsTask, "Serial Commands Task", 0x2000, NULL, 1, NULL);

    // BluetoothSerial SerialBT;
    // vTaskDelay(2000);
    // printf("1 ok\n");
    // SerialBT.begin("ESP32_Joystick", false);
    // printf("2 ok\n");
    // // SerialBT.end
    // uint8_t mac[6];
    // SerialBT.getBtAddress(mac);
    // // for (int8 i = 0; i < 6; i++)
    // // {
    // //     xQueueReceive(qSerialCommands, mac + i, 1000);
    // // }
    // Serial.printf("BT MAC: %x:%x:%x:%x:%x:%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
    // vTaskDelay(1000);
    // SerialBT.end();
}
void loop()

{
    if (1)
    {
        static uint8_t i = 0;
        printf("%d ok\n", i++);
    }

    // int a = 0;
    // printf("connecting");
    // while (SerialBT.connected())
    // {
    //     vTaskDelay(100);
    //     printf(".");
    // }
    // printf("\n");
    // SerialBT.printf("%d\n", a++);
    // vTaskDelay(1000);
    // if (Serial.available())
    // {
    //     SerialBT.write(Serial.read());
    // }
    // if (SerialBT.available())
    // {
    //     Serial.write(SerialBT.read());
    // }
    vTaskDelay(1000);
}
// bool initQT()
// {

// }

// //This example code is in the Public Domain (or CC0 licensed, at your option.)
// //By Evandro Copercini - 2018
// //
// //This example creates a bridge between Serial and Classical Bluetooth (SPP)
// //and also demonstrate that SerialBT have the same functionalities of a normal Serial
// #include "Arduino.h"
// #include "BluetoothSerial.h"

// #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// #endif

// BluetoothSerial SerialBT;

// void setup() {
//   Serial.begin(115200);
//   SerialBT.begin("ESP32test"); //Bluetooth device name
//   Serial.println("The device started, now you can pair it with bluetooth!");
// }

// void loop() {
//   if (Serial.available()) {
//     SerialBT.write(Serial.read());
//   }
//   if (SerialBT.available()) {
//     Serial.write(SerialBT.read());
//   }
//   delay(20);
// }

// #include "Arduino.h"
// #include "BluetoothSerial.h"
// #define uart_debug_mode 1

// #ifndef int8
// #define int8 uint8_t
// #endif
// #ifndef int16
// #define int16 uint16_t
// #endif

// #define ADC_PIN_1 36
// #define ADC_PIN_2 39
// #define ADC_PIN_3 34

// #define ADC_COUNT 3
// #define MAX_ADC_COUNT 3

// #define DIG_PIN_1 25
// #define DIG_PIN_2 26
// #define DIG_PIN_3 27

// #define BUTTON_COUNT 1
// #define MAX_BUTTON_COUNT 3

// // #define BYTE_ARRAY_SIZE (ADC_COUNT * 1.5 + 1 + 2)
// #define BYTE_ARRAY_SIZE 8

// #define ACTION_MODE 0
// #define COMMAND_MODE_SERIAL 1
// #define COMMAND_MODE_PACKET 2
// #define COMMAND_PACKET_SIZE 2

// #define BT_TRANSMIT_RATE_MS 100

// #define USING_MULTI_SAMPLING
// #define SAMPLE_COUNT 50
// #define INITIAL_SAMPLING_COEFFICIENT 0.8
// #define LED 23
// int8 adcPins[MAX_ADC_COUNT] = {ADC_PIN_1, ADC_PIN_2, ADC_PIN_3};
// int8 digPins[MAX_BUTTON_COUNT] = {DIG_PIN_1, DIG_PIN_2, DIG_PIN_3};
// float alpha;
// bool b;
// BluetoothSerial SerialBT;

// void setup()
// {
//     SerialBT.begin("ESP32_Joystick");
//     Serial.begin(115200);

//     for (int i = 0; i < BUTTON_COUNT; i++)
//     {
//         printf("1:%d\n", i);
//         // gpio_set_direction(digPins[i],inputonly);
//         pinMode(digPins[i], INPUT);
//         printf("2:%d\n", i);
//     }
//     pinMode(LED, OUTPUT);
//     digitalWrite(LED, 1);
//     vTaskDelay(100);
//     digitalWrite(LED, 0);
// }
// void loop()
// {
//     int adc[ADC_COUNT];
//     int temp = 0;
//     int foop = 0;
//     int dig[BUTTON_COUNT];
//     static int prev[ADC_COUNT];
//     // foop = Serial.available();
//     if (Serial.available())
//     {
//         printf("enter alpha\n");

//         foop = Serial.read();
//         switch (foop)
//         {
//         case '1':
//             for (int i = 0; i < 20; i++)
//                 Serial.println((SerialBT.connected()) ? " BLUETOOTH CONNECTED" : "NOT CONNECTED");
//             break;
//         case '2':
//             alpha = 0.95;
//             break;
//         case '3':
//             alpha = 0.9;
//             break;
//         case '4':
//             alpha = 0.7;
//             break;
//         case '5':
//             alpha = 0.5;
//             break;
//         case '6':
//             alpha = 0.3;
//             break;
//         case '7':
//             alpha = 0.2;
//             break;
//         case '8':
//             alpha = 0.1;
//         default:
//             break;
//         }
//     }

//     for (int i = 0; i < BUTTON_COUNT; i++)
//     {
//         dig[i] = digitalRead(digPins[i]);
//         printf("%d  -", dig[i]);
//         SerialBT.printf("%d  -", dig[i]);
//     }
//     printf("\n");
//     digitalWrite(26, b);
//     b = b ^ 0;
//     vTaskDelay(100);
// }

// #include "Arduino.h"
// void setup()
// {
//     Serial.begin(115200);
// }
// void loop()
// {
//     for (int i = 0; i < 100; i++)
//     {
//         Serial.printf("%d: ", i);
//         Serial.println(i % 2);
//         vTaskDelay(100);
//     }
// }