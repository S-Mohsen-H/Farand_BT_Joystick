#include "Tasks.h"
// #include "General.h"
QueueHandle_t qTransmitBT;
QueueHandle_t qTaskManager;
QueueHandle_t qADC;

Joystick_TypeDef Joystick;

void readButtons(bool arr[MAX_BUTTON_COUNT], int8 digPins[MAX_BUTTON_COUNT])
{

    for (int8 i = 0; i < BUTTON_COUNT; i++)
        arr[i] = digitalRead(digPins[i]);

    // for (int8 i = 0; i < BUTTON_COUNT; i++)
    //     message.button[i] = digitalRead(digPins[i]);
}

void readAxes(int16 arr[MAX_ADC_COUNT], int8 adcPins[MAX_ADC_COUNT], float alpha)
{
    if (alpha > 1 || alpha < 0)
    {
        logMsg(__ASSERT_FUNC, "Wrong alpha", 0);
        vTaskDelay(1000);
        return;
    }
    uint32_t temp;
    static int16 prev[3];
#ifndef USING_MULTI_SAMPLING
    for (int8 i = 0; i < ADC_COUNT; i++)
        arr[i] = analogRead(adcPins[i]);
#else
    for (int i = 0; i < ADC_COUNT; i++)
    {
        temp = 0;
        for (int j = 0; j < SAMPLE_COUNT; j++)
        {
            temp += analogRead(adcPins[i]);
        }
        arr[i] = ((1 - alpha) * prev[i]) + (alpha * (temp / SAMPLE_COUNT));
        prev[i] = arr[i];
#ifdef DEBUG_READAXES
        for (int8 i = 0; i < ADC_COUNT; i++)
            logMsg(__ASSERT_FUNC, "Axis: " + String(arr[i]), 1);
#endif
        // printf("%.2f  -", ((float)(adc[i]) / 4095) * 3.3);
        // SerialBT.printf("%.2f  -", ((float)(adc[i]) / 4095) * 3.3);
    }
#endif
}

void constructByteArray(MessageStruct *message, byte *arr)
{
    int8 temp = 0;
    // 0 12
    // 1 34
    // 2 56
    // int a, b, c;
    // int byte;
    // a = byte & 1;
    // b = byte & 2;
    // c = byte & 4;
    // {
    //     0xAA, L1, L2, L3, H1H2, H3b1b2b3b4, 0xBB
    //     0xAA, , DIG1, H.adc1, L.adc1, H.adc2, L.adc2, H.adc3, L.adc3

    // }
    for (int8 i = 0; i < BYTE_ARRAY_SIZE; i++)
        arr[i] = 0;

    for (int8 i = 0; i < ADC_COUNT; i++)
    {
        arr[i * 2 + 2] = message->adc[i] >> 8;
        arr[i * 2 + 3] = message->adc[i] & 0xFF;
    }
    arr[1] = message->button[0];
    arr[COMMAND_BYTE_INDEX] = CMD_START_ACTION_MODE;
    if (CODING_0X1A)
        Code_0x1A(arr, BYTE_ARRAY_SIZE);
}
void readJoystick_task(void *arg)
{
    int8 adcPins[MAX_ADC_COUNT] = {ADC_PIN_1, ADC_PIN_2, ADC_PIN_3};
    int8 digPins[MAX_BUTTON_COUNT] = {DIG_PIN_1, DIG_PIN_2, DIG_PIN_3};
    // printf("1 ok\n");

    for (int i = 0; i < BUTTON_COUNT; i++)
        pinMode(digPins[i], INPUT);
    // printf("2 ok\n");
    MessageStruct message;
    int err;
    // float alpha = INITIAL_SAMPLING_COEFFICIENT;
    while (1)

    {
        // if (uxQueueMessagesWaiting(qADC) > 0)
        // {
        //     xQueueReceive(qADC, &alpha, 0);
        //     logMsg(__ASSERT_FUNC, ("Sampling coefficient changed to " + String(alpha)).c_str(), 1);
        // }

        readAxes(message.adc, adcPins, Joystick.alpha);
        readButtons(message.button, digPins);
        // printf("3 ok\n");

        err = xQueueSend(qTransmitBT, &message, Joystick.transmitRateMS);
        if (UART_DEBUG_MODE)
            logMsg(__ASSERT_FUNC, "QueueSend", err);
        if (err)
        {
        }
        else
        {
        }
        vTaskDelay(Joystick.transmitRateMS);
    }
}

void bluetoothManager_task(void *arg)
{
    // printf("0 ok\n");
    Joystick.mode = COMMAND_MODE_PACKET;
    Joystick.transmitRateMS = BT_INITIAL_TRANSMIT_RATE_MS;
    Joystick.alpha = INITIAL_SAMPLING_COEFFICIENT;
    uint8_t commBT[COMMAND_PACKET_SIZE];
    uint8_t byteArr[BYTE_ARRAY_SIZE];
    byteArr[BYTE_ARRAY_SIZE - 1] = 0xBB;
    MessageStruct message;
    BluetoothSerial SerialBT;
    SerialBT.begin(BT_SSID, false);
    uint8_t mac[6];
    SerialBT.getBtAddress(mac);

    alarmMessage_typeDef alarmMessage;
    alarmMessage.BeepOnOff = 1;
    alarmMessage.frequency = 2000;
    alarmMessage.buzzerPin = BUZZER_PIN;
    for (int8 i = 0; i < 6; i++)
    {
        xQueueSend(qTaskManager, mac + i, 1000);
    }

    bool err;

    while (1)
    {
        uint32_t now;
        if (!Joystick.isConnected)
        {
            now = millis();
            while (!Joystick.isConnected)
            {
                Joystick.isConnected = SerialBT.connected(portMAX_DELAY);
                if (!Joystick.isConnected && millis() - now > BT_RESET_TIMEOUT_SEC * 1000 && BT_TIMEOUT_ACTIVE)
                {
                    alarmMessage_typeDef rebootAlarm;
                    rebootAlarm.Pattern = 0xF0F0FFFF;
                    rebootAlarm.AlarmCount = 1;
                    rebootAlarm.BeepOnOff = 1;
                    rebootAlarm.buzzerPin = BUZZER_PIN;
                    rebootAlarm.frequency = 3000;
                    rebootAlarm.TimePeriod = 32;
                    addAlarmToQueue(&rebootAlarm);
                    logMsg(__ASSERT_FUNC, "restarting esp", 0);
                    vTaskDelay(1010);
                    ESP.restart(); /** @note The device might need a reboot if connection isn't established within the timeout*/
                }
                vTaskDelay(1);
            }
            logMsg(__ASSERT_FUNC, "Bluetooth_Connection", Joystick.isConnected);
            printf("Took %d seconds\n", (millis() - now) / 1000);
        }

        while (Joystick.mode) // command modes
        {
            if (SerialBT.available())
            {
                switch (Joystick.mode)
                {
                case COMMAND_MODE_SERIAL:
                {
                    commBT[0] = SerialBT.read();
                    // SerialBT.printf("got command 1: %c\n", comm[0]);
                    switch (commBT[0])
                    {
                    case '1':
                        Joystick.mode = ACTION_MODE;
                        // logMsgBT(&SerialBT, __ASSERT_FUNC, "Entering action mode", 1);
                        break;
                    case '2':
                    {
                        bool f = 1;
                        // SerialBT.printf("Enter transmit rate:\n");
                        while (f)
                        {
                            if (SerialBT.available())
                            {
                                Joystick.transmitRateMS = SerialBT.parseInt();
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
                                    Joystick.transmitRateMS = 1;
                                    break;
                                case 1:
                                    Joystick.transmitRateMS = 2;
                                    break;
                                case 2:
                                    Joystick.transmitRateMS = 5;
                                    break;
                                case 3:
                                    Joystick.transmitRateMS = 10;
                                    break;
                                case 4:
                                    Joystick.transmitRateMS = 20;
                                    break;
                                case 5:
                                    Joystick.transmitRateMS = 50;
                                    break;
                                case 6:
                                    Joystick.transmitRateMS = 100;
                                    break;
                                case 7:
                                    Joystick.transmitRateMS = 200;
                                    break;
                                case 8:
                                    Joystick.transmitRateMS = 500;
                                    break;
                                case 9:
                                    Joystick.transmitRateMS = 1000;
                                    break;
                                case 10:
                                    Joystick.transmitRateMS = 2000;
                                    break;
                                case 11:
                                    Joystick.transmitRateMS = 5000;
                                    break;
                                case 12:
                                    Joystick.transmitRateMS = 10000;
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
                break;
                case COMMAND_MODE_PACKET:
                {
                    SerialBT.readBytes(commBT, COMMAND_PACKET_SIZE);
                    printf("got command: %x\n", commBT[COMMAND_BYTE_INDEX]);
                    switch (commBT[COMMAND_BYTE_INDEX])
                    {
                    case CMD_START_ACTION_MODE:
                        Joystick.mode = ACTION_MODE;
                        break;

                    case CMD_TRANSMIT_RATE:
                        Joystick.transmitRateMS = commBT[COMMAND_BYTE_INDEX + 1] * 10;
                        break;
                    case CMD_ALPHA:
                        Joystick.alpha = (float)(commBT[COMMAND_BYTE_INDEX + 1]) / 100.0;
                        break;

                    default:
                        break;
                    }
                }
                break;
                case COMMAND_MODE_AUTO_DETECTION:
                {
                    bool isPortCorrect = true;
                    SerialBT.readBytes(commBT, 8);
                    for (int8 i = 0; i < 8; i++)
                    {
                        if (commBT[i] != autoDetectionPacket[i])
                            isPortCorrect = false;
                    }
                    if (isPortCorrect)
                    {
                        SerialBT.write(autoDetectionPacket, sizeof(autoDetectionPacket));
                        Joystick.mode = COMMAND_MODE_PACKET;
                    }
                    else
                    {
                        SerialBT.println("You picked the wrong house, fool!");
                        SerialBT.end();
                        ESP.restart();
                    }
                }
                break;
                default:
                    break;
                }
            }
            vTaskDelay(10);
        }

        err = xQueueReceive(qTransmitBT, &message, Joystick.transmitRateMS);
        if (UART_DEBUG_MODE)
            logMsg(__ASSERT_FUNC, "QueueRead", err);

        if (err)
        {
            constructByteArray(&message, byteArr);
            if (SerialBT.connected(10))
            {
                SerialBT.write(byteArr, sizeof(byteArr));
                if (SerialBT.available())
                {
                    SerialBT.readBytes(commBT, COMMAND_PACKET_SIZE);
                    printf("got command: %x\n", commBT[COMMAND_BYTE_INDEX]);
                    switch (commBT[COMMAND_BYTE_INDEX])
                    {
                    case CMD_TRANSMIT_RATE:
                        Joystick.transmitRateMS = commBT[COMMAND_BYTE_INDEX + 1] * 10;
                        break;
                    case CMD_ALPHA:
                        Joystick.alpha = (float)(commBT[COMMAND_BYTE_INDEX + 1]) / 100.0;
                        break;
                    case CMD_LED:
                        digitalWrite(LED_PIN, commBT[COMMAND_BYTE_INDEX + 1]);
                        break;
                    case CMD_ALARM:
                    {
                        alarmMessage.AlarmCount = commBT[COMMAND_BYTE_INDEX + 5];
                        alarmMessage.Pattern = commBT[COMMAND_BYTE_INDEX + 1];
                        alarmMessage.Pattern |= commBT[COMMAND_BYTE_INDEX + 2] << 8;
                        alarmMessage.Pattern |= commBT[COMMAND_BYTE_INDEX + 3] << 16;
                        alarmMessage.Pattern |= commBT[COMMAND_BYTE_INDEX + 4] << 24;
                        int8 i;
                        for (i = 31; i >= 0; i--)
                        {
                            if (alarmMessage.Pattern & (1 << i))
                            {
                                alarmMessage.TimePeriod = i;
                                break;
                            }
                        }

                        // alarmMessage.TimePeriod = 32;
                        // Farand_Alarm(alarmMessage.Pattern, alarmMessage.AlarmCount, alarmMessage.TimePeriod, alarmMessage.BeepOnOff, BUZZER_PIN);
                        addAlarmToQueue(&alarmMessage);
                        break;
                    }
                    default:
                        break;
                    }
                }
                // #ifdef

#ifdef DEBUG_BT_PRINT_VALUES
                int16 arr[3];
                arr[0] = byteArr[3] & 0xFF;
                arr[0] |= (byteArr[2] & 0xF) << 8;
                arr[1] = byteArr[5] & 0xFF;
                arr[1] |= (byteArr[4] & 0xF) << 8;
                arr[2] = byteArr[7] & 0xFF;
                arr[2] |= (byteArr[6] & 0xF) << 8;
                for (int8 i = 0; i < 3; i++)
                    printf("%d :%d  __", message.adc[i], arr[i]);
                printf("\n");
#endif
            }
            else
            {
                Joystick.isConnected = 0;
                // static bool ledStat = 0;
                // digitalWrite(LED_PIN, ledStat);
                // ledStat = !ledStat;
            }
        }
        else
        {
            logMsg(__ASSERT_FUNC, "No message received", 0);
        }
        vTaskDelay(Joystick.transmitRateMS);
    }
}
void taskManager_task(void *arg)
{
    int8 loop = 1;
    int8 comm[10];
    if (ADC_TASK_ENABLED)
        xTaskCreate(readJoystick_task, "Analog Read Task", READJOYSTICK_STACK_SIZE, NULL, 20, NULL);
    if (BT_TASK_ENABLED)
        xTaskCreate(bluetoothManager_task, "Bluetooth Transmit Task", BLUETOOTHMANAGER_STACK_SIZE, NULL, 20, NULL);
    while (1)
    {
        vTaskDelay(1);
        if (uxQueueMessagesWaiting(qTransmitBT) == 10)
        {
            vQueueDelete(qTransmitBT);
            qTransmitBT = xQueueCreate(10, sizeof(MessageStruct));
            logMsg(__ASSERT_FUNC, "qTransmitBT recreated", 1);
        }

        if (Serial.available())
        {
            comm[0] = Serial.read();
            printf("Got command 1: %c\n", comm[0]);
            switch (comm[0])
            {
            case '3':
                printf("Command mode: ");
                switch (Joystick.mode)
                {
                case ACTION_MODE:
                    printf(" Action Mode\n");
                    break;
                case COMMAND_MODE_PACKET:
                    printf(" PACKET Mode\n");
                    break;
                case COMMAND_MODE_SERIAL:
                    printf(" SERIAL Mode\n");
                    break;
                default:
                    break;
                }
                break;
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
                    xQueueReceive(qTaskManager, mac + i, 1000);
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
                        printf("Got command 2: %c\n", comm[1]);
                        switch (comm[1])
                        {
                        case 'A':
                        case 'a':
                            loop = 0;
                            xTaskCreate(readJoystick_task, "Analog Read Task", READJOYSTICK_STACK_SIZE, NULL, 20, NULL);
                            xTaskCreate(bluetoothManager_task, "Bluetooth Transmit Task", BLUETOOTHMANAGER_STACK_SIZE, NULL, 20, NULL);
                            break;
                        case '1':
                            loop = 0;
                            xTaskCreate(readJoystick_task, "Analog Read Task", READJOYSTICK_STACK_SIZE, NULL, 20, NULL);
                            break;
                        case '2':
                            loop = 0;
                            xTaskCreate(bluetoothManager_task, "Bluetooth Transmit Task", BLUETOOTHMANAGER_STACK_SIZE, NULL, 20, NULL);
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

void Code_0x1A(uint8_t *Buf, uint32_t Len)
{
    uint16_t Code_Byte = 0;
    uint16_t dataPack = 0;
    uint16_t codeIndex;
    uint16_t n = 0;
    uint16_t loopCount = Len / 8;

    // 		This coding sets a Code Byte in the form abc0 defg with the following dataPack sequence
    //		CODE_0_6	| code_byte6	| code_byte5	| code_byte4	|	0	| code_byte3 |	code_byte2 |	code_byte1 |	code_byte0		0x1A never happens
    //		byte0
    //		byte1
    //		byte2
    //		byte3
    //		byte4
    //		byte5
    //		byte6

    // Code 0x1A for normal command data (8 bytes) -----------------------------------------------------------------------------
    for (n = 0; n < loopCount; n++)
    {
        Buf[8 * n] = 0;
        for (codeIndex = 0; codeIndex < 8; codeIndex++)
        {
            if (Buf[8 * n + codeIndex + 1] == 0x1A)
            {
                Buf[8 * n] |= 0x01 << codeIndex;
                Buf[8 * n + codeIndex + 1] = 0xA1; //  this is replaced to 0x1A to help finding this code
            }
        }

        // Shift high nibble of code byte one bit to left to clear bit position 4 to 0, this avoids occurence of 0x1A in code byte itself
        Code_Byte = (0xF0 & Buf[8 * n]) << 1;
        Buf[8 * n] = (0x0F & Buf[8 * n]) | Code_Byte;
    }
}
