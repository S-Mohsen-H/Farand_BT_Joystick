#include "Tasks.h"
// #include "General.h"

QueueHandle_t qUART;
QueueHandle_t qSerialCommands;
// QueueHandle_t qTaskManager;

Joystick_TypeDef Joystick;

void readButtons(bool arr[MAX_DIG_COUNT], bool digPins[MAX_DIG_COUNT])
{

    for (int8 i = 0; i < DIG_COUNT; i++)
        arr[i] = digitalRead(digPins[i]);

    // for (int8 i = 0; i < DIG_COUNT; i++)
    //     message.button[i] = digitalRead(digPins[i]);
}

void readADC(int16 arr[MAX_ADC_COUNT], int8 adcPins[MAX_ADC_COUNT])
{

    for (int8 i = 0; i < ADC_COUNT; i++)
        arr[i] = analogRead(adcPins[i]);

    // for (int8 i = 0; i < ADC_COUNT; i++)
    //     message.adc[i] = analogRead(adcPins[i]);
}

void constructByteArray(MessageStruct *message, byte arr[BYTE_ARRAY_SIZE])
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
        arr[i * 2 + 1] = message->adc[i] >> 8;
        arr[i * 2 + 2] = message->adc[i] & 0xFF;
        if (uart_debug_mode)
            logMsg(__ASSERT_FUNC, numToBin(arr[i * 2 + 1]).c_str(), pdPASS, i * 2 + 1);
    }
#ifdef USING_SINGLE_BYTE_FOR_BUTTONS

    for (int8 i = 1; i <= MAX_DIG_COUNT; i++)
        arr[MAX_ADC_COUNT + i] = 0;

    for (int8 i = 0; i < DIG_COUNT; i++) // Single byte for multiple buttons layout
    {
        arr[MAX_ADC_COUNT + 1] |= (message->button[i] << i);
    }

#else
    for (int8 i = 0; i < DIG_COUNT; i++) // multiple bytes for multiple buttons layout
    {
        arr[MAX_ADC_COUNT + i] = message->button[i];
    }
#endif
}
void adcReadTask(void *arg)
{
    int8 adcPins[MAX_ADC_COUNT] = {ADC_PIN_1, ADC_PIN_2, ADC_PIN_3};
    bool digPins[MAX_DIG_COUNT] = {DIG_PIN_1, DIG_PIN_2, DIG_PIN_3};
    MessageStruct message;
    int err;
    while (1)

    {
        readADC(message.adc, adcPins);
        readButtons(message.button, digPins);
        err = xQueueSend(qUART, &message, 10);
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

void btTask(void *arg)
{
    // printf("0 ok\n");
    Joystick.commandMode = COMMAND_MODE_SERIAL;
    uint8_t comm[0x100];
    byte byteArr[BYTE_ARRAY_SIZE];
    byteArr[0] = 0xAA;
    byteArr[BYTE_ARRAY_SIZE - 1] = 0xBB;
    MessageStruct message;
    BluetoothSerial SerialBT;
    // printf("1 ok\n");
    SerialBT.begin("ESP32_Joystick", false);
    // printf("2 ok\n");
    // SerialBT.end
    uint8_t mac[6];
    SerialBT.getBtAddress(mac);
    for (int8 i = 0; i < 6; i++)
    {
        printf("3 %d ok\n", i);

        xQueueSend(qSerialCommands, mac + i, 1000);
    }
    uint32_t now = millis();
    Joystick.isConnected = SerialBT.connected(portMAX_DELAY);
    logMsg(__ASSERT_FUNC, "Bluetooth_Connection", Joystick.isConnected);
    printf("Took %d seconds\n", (millis() - now) / 1000);
    // printf("4 ok\n");
    // uint32_t now = millis();
    // if err
    // while (!(SerialBT.connected(10)))
    // {
    //     printf("5 ok\n");

    //     vTaskDelay(100);
    //     logMsg(__ASSERT_FUNC, "Bluetooth_Connection", 0);
    // }
    bool err;
    // if (SerialBT.available())
    // {
    //     err = SerialBT.read();
    //     if (err)
    //         printf("connectrd\n");
    //     else
    //         while (1)
    //         {
    //             printf("not connected");
    //             vTaskDelay(1000);
    //         }
    // }

    while (1)
    {
        if (SerialBT.available())
        {
            comm[0] = SerialBT.read();
            switch (comm[0])
            {
            case '1':
                printf("%d",SerialBT.getBtAddress)
                break;
            
            default:
                break;
            }
        }
        err = xQueueReceive(qUART, &message, 10);
        logMsg(__ASSERT_FUNC, "QueueRead", err);

        if (err)
        {
            constructByteArray(&message, byteArr);
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
    int8 loop = 1;
    int8 comm[10];
    while (1)
    {
        if (Serial.available())
        {
            comm[0] = Serial.read();
            printf("Got command: %c\n", comm[0]);
            switch (comm[0])
            {
            case '1':
            {
                // BluetoothSerial SerialBT;
                // printf("1 ok\n");
                // SerialBT.begin("ESP32_Joystick", false);
                // printf("2 ok\n");
                // SerialBT.end
                uint8_t mac[6];
                // SerialBT.getBtAddress(mac);
                for (int8 i = 0; i < 6; i++)
                {
                    xQueueReceive(qSerialCommands, mac + i, 1000);
                }
                Serial.printf("BT MAC: %x:%x:%x:%x:%x:%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
                // SerialBT.end();
                break;
            }

            case 't':
                while (loop)
                {
                    if (Serial.available())
                    {

                        comm[1] = Serial.read();
                        printf("Got command: %c\n", comm[1]);
                        switch (comm[1])
                        {
                        case 'a':
                            loop = 0;
                            xTaskCreate(adcReadTask, "Analog Read Task", 0x800, NULL, 1, NULL);
                            xTaskCreate(btTask, "Bluetooth Transmit Task", 0x2000, NULL, 1, NULL);
                            break;
                        case '1':
                            loop = 0;
                            xTaskCreate(adcReadTask, "Analog Read Task", 0x800, NULL, 1, NULL);
                            break;
                        case '2':
                            loop = 0;
                            xTaskCreate(btTask, "Bluetooth Transmit Task", 0x2000, NULL, 1, NULL);
                            break;
                        default:
                            break;
                        }
                    }
                    vTaskDelay(1);
                }

            default:
                break;
            }
        }
    }
}
// void taskManagerTask(void* arg)
// {
//     int8 comm[10];
//     while (1)
//     {
//         if (xQueueReceive(qTaskManager))
//     }

// }