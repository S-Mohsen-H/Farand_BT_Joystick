// // #include "main.h"
// #include "General.h"
// // #include "Logging.h"
// #include "Tasks.h"
// #include "Arduino.h"
// #include "BluetoothSerial.h"
// // BluetoothSerial SerialBT;
// void setup()
// {

//     Serial.begin(115200);
//     qUART = xQueueCreate(10, sizeof(MessageStruct));
//     qSerialCommands = xQueueCreate(50, sizeof(uint8_t));
//     // qTaskManager = xQueueCreate(10, sizeof(uint8_t));

//     xTaskCreate(serialCommandsTask, "Serial Commands Task", 0x2000, NULL, 1, NULL);

//     // BluetoothSerial SerialBT;
//     // vTaskDelay(2000);
//     // printf("1 ok\n");
//     // SerialBT.begin("ESP32_Joystick", false);
//     // printf("2 ok\n");
//     // // SerialBT.end
//     // uint8_t mac[6];
//     // SerialBT.getBtAddress(mac);
//     // // for (int8 i = 0; i < 6; i++)
//     // // {
//     // //     xQueueReceive(qSerialCommands, mac + i, 1000);
//     // // }
//     // Serial.printf("BT MAC: %x:%x:%x:%x:%x:%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
//     // vTaskDelay(1000);
//     // SerialBT.end();
// }
// void loop()

// {
//     if (uart_debug_mode)
//     {
//         static uint8_t i = 0;
//         printf("%d ok\n", i++);
//     }

//     // int a = 0;
//     // printf("connecting");
//     // while (SerialBT.connected())
//     // {
//     //     vTaskDelay(100);
//     //     printf(".");
//     // }
//     // printf("\n");
//     // SerialBT.printf("%d\n", a++);
//     // vTaskDelay(1000);
//     // if (Serial.available())
//     // {
//     //     SerialBT.write(Serial.read());
//     // }
//     // if (SerialBT.available())
//     // {
//     //     Serial.write(SerialBT.read());
//     // }
//     vTaskDelay(1000);
// }
// // bool initQT()
// // {

// // }

// // //This example code is in the Public Domain (or CC0 licensed, at your option.)
// // //By Evandro Copercini - 2018
// // //
// // //This example creates a bridge between Serial and Classical Bluetooth (SPP)
// // //and also demonstrate that SerialBT have the same functionalities of a normal Serial
// // #include "Arduino.h"
// // #include "BluetoothSerial.h"

// // #if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
// // #error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
// // #endif

// // BluetoothSerial SerialBT;

// // void setup() {
// //   Serial.begin(115200);
// //   SerialBT.begin("ESP32test"); //Bluetooth device name
// //   Serial.println("The device started, now you can pair it with bluetooth!");
// // }

// // void loop() {
// //   if (Serial.available()) {
// //     SerialBT.write(Serial.read());
// //   }
// //   if (SerialBT.available()) {
// //     Serial.write(SerialBT.read());
// //   }
// //   delay(20);
// // }

#include "Arduino.h"

#ifndef int8
#define int8 uint8_t
#endif
#ifndef int16
#define int16 uint16_t
#endif

#define ADC_PIN_1 36
#define ADC_PIN_2 39
#define ADC_PIN_3 34

#define ADC_COUNT 2
#define MAX_ADC_COUNT 3

#define DIG_PIN_1 25
#define DIG_PIN_2 26
#define DIG_PIN_3 27

#define DIG_COUNT 1
#define MAX_DIG_COUNT 3
#define BYTE_ARRAY_SIZE (ADC_COUNT * 2 + DIG_COUNT + 2)
float alpha = 0.2;
int8 adcPins[MAX_ADC_COUNT] = {ADC_PIN_1, ADC_PIN_2, ADC_PIN_3};
bool digPins[MAX_DIG_COUNT] = {DIG_PIN_1, DIG_PIN_2, DIG_PIN_3};
bool b;
void setup()
{
    Serial.begin(115200);
}
void loop()
{
    int adc[ADC_COUNT];
    int temp = 0;
    int dig[DIG_COUNT];

    for (int i = 0; i < ADC_COUNT; i++)
    {
        temp = 0;
        for (int j = 0; j < 50; j++)
        {
            temp += analogRead(adcPins[i]);
        }
        adc[i] =  temp / 50;
        
        printf("%f  -", ((float)(adc[i]) / 4095) * 3.3);
        // printf("%d  -", adc[i]);
    }

    for (int i = 0; i < DIG_COUNT; i++)
    {
        dig[i] = digitalRead(digPins[i]);
        printf("%d  -", dig[i]);
    }
    printf("\n");
    digitalWrite(26, b);
    b = b^0;
    vTaskDelay(100);
}