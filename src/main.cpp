// #include "main.h"
#include "General.h"
#include "Logging.h"
#include "Tasks.h"

BluetoothSerial SerialBT;
void setup()
{
    Serial.begin(115200);
    qUART = xQueueCreate(10, sizeof(MessageStruct));
    qSerialCommands = xQueueCreate(50, sizeof(uint8_t));
    xTaskCreate(adcReadTask, "Analog Read Task", 0x800, NULL, 1, NULL);
    xTaskCreate(transmitByteTask, "Bluetooth Transmit Task", 0x2000, NULL, 1, NULL);
    xTaskCreate(serialCommandsTask, "Serial Commands Task", 0x2000, NULL, 1, NULL);
}
void loop()
{
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