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
        printf("ADC %d:%d\n", i, message->adc[i]);
    }
#ifdef USING_SINGLE_BYTE_FOR_BUTTONS

    for (int8 i = 1; i <= MAX_DIG_COUNT; i++)
        arr[MAX_ADC_COUNT + i] = 0;

    for (int8 i = 0; i < DIG_COUNT; i++) // Single byte for multiple buttons layout
    {
        arr[MAX_ADC_COUNT + 1] |= (message->button[i] << i);
        printf("Button %d:%d\n", i, message->button[i]);
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
        err = xQueueSend(qUART, &message, Joystick.transmitRate);
        logMsg(__ASSERT_FUNC, "QueueSend", err);
        if (err)
        {
        }
        else
        {
        }
        vTaskDelay(Joystick.transmitRate);
    }
}

void btTask(void *arg)
{
    // printf("0 ok\n");
    Joystick.mode = COMMAND_MODE_SERIAL;
    Joystick.transmitRate = BT_TRANSMIT_RATE_MS;
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
    while (1)
    {
        Joystick.isConnected = SerialBT.connected(portMAX_DELAY);
        vTaskDelay(1);
        if (Joystick.isConnected)
            break;
    }
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
        while (Joystick.mode)
        {
            if (SerialBT.available())
            {
                comm[0] = SerialBT.read();
                SerialBT.printf("got command: %c\n", comm[0]);
                switch (comm[0])
                {
                case '1':
                    Joystick.mode = ACTION_MODE;
                    logMsgBT(&SerialBT, __ASSERT_FUNC, "Entering action mode", 1);
                    break;
                case '2':
                {
                    bool f = 1;
                    SerialBT.printf("Enter transmit rate:\n");
                    while (f)
                    {
                        if (SerialBT.available())
                        {
                            Joystick.transmitRate = SerialBT.parseInt();
                            f = 0;
                        }
                        else
                            vTaskDelay(1);
                    }
                    break;
                }
                case '3':
                {
                    // bool f = 1;
                    uint8_t buf[COMMAND_PACKET_SIZE];
                    SerialBT.printf("Send commad packet:\n");
                    if (SerialBT.available())
                    {
                        SerialBT.readBytes(buf, COMMAND_PACKET_SIZE);
                        if (buf[0] == 0xcc)
                        {
                            switch (buf[1])
                            {
                            case 0:
                                Joystick.transmitRate = 1;
                                break;
                            case 1:
                                Joystick.transmitRate = 2;
                                break;
                            case 2:
                                Joystick.transmitRate = 5;
                                break;
                            case 3:
                                Joystick.transmitRate = 10;
                                break;
                            case 4:
                                Joystick.transmitRate = 20;
                                break;
                            case 5:
                                Joystick.transmitRate = 50;
                                break;
                            case 6:
                                Joystick.transmitRate = 100;
                                break;
                            case 7:
                                Joystick.transmitRate = 200;
                                break;
                            case 8:
                                Joystick.transmitRate = 500;
                                break;
                            case 9:
                                Joystick.transmitRate = 1000;
                                break;
                            case 10:
                                Joystick.transmitRate = 2000;
                                break;
                            case 11:
                                Joystick.transmitRate = 5000;
                                break;
                            case 12:
                                Joystick.transmitRate = 10000;
                                break;

                            default:
                                break;
                            }
                        }
                    }
                    else
                        vTaskDelay(1);
                    break;
                }
                default:
                    break;
                }
            }
            vTaskDelay(10);
        }

        err = xQueueReceive(qUART, &message, Joystick.transmitRate);
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

        vTaskDelay(Joystick.transmitRate);
    }
}
void serialCommandsTask(void *arg)
{
    int8 loop = 1;
    int8 comm[10];
    while (1)
    {
        vTaskDelay(1);
        if (uxQueueMessagesWaiting(qUART) == 10)
        {
            vQueueDelete(qUART);
            qUART = xQueueCreate(10, sizeof(MessageStruct));
            logMsg(__ASSERT_FUNC, "qUART recreated", 1);
        }
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
            case '2':
                logMsg(__ASSERT_FUNC, "BT connection status:", Joystick.isConnected);
                break;
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
                            xTaskCreate(adcReadTask, "Analog Read Task", 0x800, NULL, 20, NULL);
                            xTaskCreate(btTask, "Bluetooth Transmit Task", 0x2000, NULL, 20, NULL);
                            break;
                        case '1':
                            loop = 0;
                            xTaskCreate(adcReadTask, "Analog Read Task", 0x800, NULL, 20, NULL);
                            break;
                        case '2':
                            loop = 0;
                            xTaskCreate(btTask, "Bluetooth Transmit Task", 0x2000, NULL, 20, NULL);
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