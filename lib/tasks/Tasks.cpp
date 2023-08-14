#include "Tasks.h"
// #include "General.h"

QueueHandle_t qUART;
QueueHandle_t qSerialCommands;

void readButtons(bool arr[MAX_DIG_COUNT], bool digPins[MAX_DIG_COUNT])
{

    for (int8 i = 0; i < DIG_COUNT; i++)
        arr[i] = digitalRead(digPins[i]);

    // for (int8 i = 0; i < DIG_COUNT; i++)
    //     joystick.button[i] = digitalRead(digPins[i]);
}

void readADC(int16 arr[MAX_ADC_COUNT], int8 adcPins[MAX_ADC_COUNT])
{

    for (int8 i = 0; i < ADC_COUNT; i++)
        arr[i] = analogRead(adcPins[i]);

    // for (int8 i = 0; i < ADC_COUNT; i++)
    //     joystick.adc[i] = analogRead(adcPins[i]);
}

void constructByteArray(MessageStruct *joystick, byte arr[BYTE_ARRAY_SIZE])
{

    // 0 12
    // 1 34
    // 2 56
    // int a, b, c;
    // int byte;
    // a = byte & 1;
    // b = byte & 2;
    // c = byte & 4;
    // {
    //     0xAA, H.adc1, L.adc1, H.adc2, L.adc2, H.adc3, L.adc3, DIG1, DIG2, DIG3, 0xBB
    // }
    for (int8 i = 0; i < ADC_COUNT; i++)
    {
        arr[i * 2 + 1] = joystick->adc[i] >> 8;
        arr[i * 2 + 2] = joystick->adc[i] & 0xFF;
        if (uart_debug_mode)
            logMsg(__ASSERT_FUNC, numToBin(arr[i * 2 + 1]), pdPASS, i * 2 + 1);
    }
#ifdef USING_SINGLE_BYTE_FOR_BUTTONS

    for (int8 i = 1; i <= MAX_DIG_COUNT; i++)
        arr[MAX_ADC_COUNT + i] = 0;

    for (int8 i = 0; i < DIG_COUNT; i++) // Single byte for multiple buttons layout
    {
        arr[MAX_ADC_COUNT + 1] |= (joystick->button[i] << i);
    }

#else
    for (int8 i = 0; i < DIG_COUNT; i++) // multiple bytes for multiple buttons layout
    {
        arr[MAX_ADC_COUNT + i] = joystick->button[i];
    }
#endif
}
void adcReadTask(void *arg)
{
    int8 adcPins[MAX_ADC_COUNT] = {ADC_PIN_1, ADC_PIN_2, ADC_PIN_3};
    bool digPins[MAX_DIG_COUNT] = {DIG_PIN_1, DIG_PIN_2, DIG_PIN_3};
    MessageStruct joystick;
    int err;
    while (1)

    {
        readADC(joystick.adc, adcPins);
        readButtons(joystick.button, digPins);
        err = xQueueSend(qUART, &joystick, 10);
        logMsg(__ASSERT_FUNC, "QueueSend", err);
        if (err)
        {
        }
        else
        {
        }
        vTaskDelay(10);
    }
}

void transmitByteTask(void *arg)
{
    byte byteArr[BYTE_ARRAY_SIZE];
    byteArr[0] = 0xAA;
    byteArr[BYTE_ARRAY_SIZE - 1] = 0xBB;
    MessageStruct joystick;
    BluetoothSerial SerialBT;
    SerialBT.begin("ESP32_Joystick");
    uint8_t mac[6];
    SerialBT.getBtAddress(mac);
    for (int8 i = 0; i < 6; i++)
    {
        xQueueSend(qSerialCommands, mac + i, 1000);
    }
    bool err = SerialBT.connected(10);
    while (err)
    {
        vTaskDelay(100);
        logMsg(__ASSERT_FUNC, "Bluetooth_Connection", err = SerialBT.connected(10));
    }
    if (SerialBT.available())
    {
        err = SerialBT.read();
        if (err)
            printf("connectrd\n");
        else
            while (1)
            {
                printf("not connected");
                vTaskDelay(1000);
            }
    }
    while (1)
    {
        err = xQueueReceive(qUART, &joystick, 10);
        logMsg(__ASSERT_FUNC, "QueueRead", err);

        if (err)
        {
            constructByteArray(&joystick, byteArr);
            if (SerialBT.connected(10))
            {
                SerialBT.write(byteArr, sizeof(byteArr));
            }
        }
        else
        {
        }

        vTaskDelay(10);
    }
}
void serialCommandsTask(void *arg)
{
    int8 comm[10];
    while (1)
    {
        if (Serial.available())
        {
            comm[0] = Serial.read();
            switch (comm[0])
            {
            case '1':
            {
                int8 mac[6];
                for (int8 i = 0; i < 6; i++)
                {
                    xQueueReceive(qSerialCommands, mac + i, 1000);
                }
                Serial.printf("BT MAC: %d:%d:%d:%d:%d:%d\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
            }
            break;

            default:
                break;
            }
        }
    }
}