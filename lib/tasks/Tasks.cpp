#include "Tasks.h"
QueueHandle_t qTransmitBT;
QueueHandle_t qBluetoothMac;
QueueHandle_t qBuzzer;
QueueHandle_t qLED;
QueueHandle_t qADC;
QueueHandle_t qBatteryCharging;
Joystick_TypeDef Joystick;
uint16_t chargerVoltage;
float batteryVoltageThreshold = 3.45;
bool silentMode = 0;

void readBattery(uint16_t *bat, float alpha)
{
    static uint64_t now = millis();
    uint32_t temp;
    static int16 prev;
#ifndef USING_MULTI_SAMPLING
    for (int8 i = 0; i < ADC_COUNT; i++)
        arr = analogRead(BATTERY_SENSE_PIN);
#else

    temp = 0;
    for (int j = 0; j < SAMPLE_COUNT; j++)
    {
        temp += analogReadMilliVolts(BATTERY_SENSE_PIN);
    }
    *bat = ((1 - alpha) * prev) + (alpha * (temp / SAMPLE_COUNT)) * 2;
    prev = *bat;
    if (millis() - now > 1000)
    {
        printf("Battery: %d , Battery Voltage:%f , pgood: %d, Charger Sense: %d\n", *bat, Joystick.battery, analogReadMilliVolts(ANALOG_PIN_PGOOD_N), !digitalRead(DIGITAL_PIN_CHARGER_SENSE));
        now = millis();
    }

#endif
}
bool dummyF = 0;
void readButtons(bool arr[MAX_BUTTON_COUNT], uint8_t digPins[MAX_BUTTON_COUNT])
{

    for (uint8_t i = 0; i < BUTTON_COUNT; i++)
    {
        arr[i] = digitalRead(digPins[i]);
        arr[0] = 1; // MIDDLE BUTTON IS ALWAYS HIGH.
    }
}

void readAxes(int16 arr[MAX_ADC_COUNT], uint8_t adcPins[MAX_ADC_COUNT], float alpha)
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
    }
#endif

#ifdef DEBUG_READAXES2
    printf("X: %d - Y: %d\n", arr[0], arr[1]);
#endif
}

void constructByteArray(MessageStruct *message, byte *arr)
{
    int8 temp = 0;

    for (int8 i = 0; i < BYTE_ARRAY_SIZE; i++)
        arr[i] = 0;

    for (int8 i = 0; i < ADC_COUNT; i++)
    {
        arr[i * 2 + 2] = message->adc[i] >> 8;
        arr[i * 2 + 3] = message->adc[i] & 0xFF;
    }
    for (uint8_t i = 0; i < BUTTON_COUNT; i++)
    {
        arr[1] |= ((message->button[i]) << i);
    }
    arr[COMMAND_BYTE_INDEX] = CMD_START_ACTION_MODE;
    arr[BATTERY_LEVEL_INDEX] = message->bat >> 8;
    arr[BATTERY_LEVEL_INDEX + 1] = message->bat & 0xFF;

    if (CODING_0X1A)
        Code_0x1A(arr, BYTE_ARRAY_SIZE);
}
void readJoystick_task(void *arg)
{
    uint8_t adcPins[MAX_ADC_COUNT] = {ADC_PIN_1, ADC_PIN_2, ADC_PIN_3};
    uint8_t digPins[MAX_BUTTON_COUNT] = {DIGITAL_PIN_1, DIGITAL_PIN_2, DIGITAL_PIN_3};
    alarmMessage_typeDef buttonBeep;
    buttonBeep.AlarmCount = 1;
    buttonBeep.Pattern = 0b11;
    buttonBeep.TimePeriod = 2;
    buttonBeep.BeepOnOff = !silentMode;
    for (int i = 0; i < BUTTON_COUNT; i++)
        pinMode(digPins[i], INPUT);

    bool prevButton[MAX_BUTTON_COUNT];
    for (uint8_t i = 0; i < MAX_BUTTON_COUNT; i++)
        prevButton[i] = 1;
    MessageStruct message;
    ledStateStruct ledMessage;
    ledMessage.pin = 4;
    int err;
    uint32_t now;
    while (1)

    {
        readAxes(message.adc, adcPins, Joystick.alpha);
        readButtons(message.button, digPins);
        for (uint8_t i = 0; i < BUTTON_COUNT; i++)
        {
            if (message.button[i] != prevButton[i] && prevButton[i] == 1 && Joystick.isConnected)
                addAlarmToQueue(&buttonBeep);
            prevButton[i] = message.button[i];
        }
        readBattery(&(message.bat), Joystick.alpha);
        Joystick.battery = float(message.bat) / 1000;

        uint8_t msg = 2;

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
void spp_CB(esp_spp_cb_event_t event, esp_spp_cb_param_t *param)
{
    if (event == ESP_SPP_CLOSE_EVT || event == ESP_SPP_SRV_STOP_EVT)
        Joystick.isConnected = false;
}
void bluetoothManager_task(void *arg)
{
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
    SerialBT.register_callback(spp_CB);
    alarmMessage_typeDef alarmMessage;
    alarmMessage.BeepOnOff = 1;
    alarmMessage.frequency = 2000;
    alarmMessage.buzzerPin = BUZZER_PIN;
    alarmMessage.BeepOnOff = !silentMode;
    for (int8 i = 0; i < 6; i++)
    {
        xQueueSend(qTransmitBT, mac + i, 1000);
    }
    bool err;
    alarmMessage_typeDef rebootAlarm;
    rebootAlarm.Pattern = MEDIUM_BEEP_X1;
    rebootAlarm.AlarmCount = 1;
    rebootAlarm.BeepOnOff = !silentMode;
    rebootAlarm.buzzerPin = BUZZER_PIN;
    rebootAlarm.frequency = 3000;
    rebootAlarm.TimePeriod = 32;
    addAlarmToQueue(&rebootAlarm);
    alarmMessage_typeDef disconnectionAlarm;
    disconnectionAlarm.Pattern = SHORT_BEEP_X2;
    disconnectionAlarm.TimePeriod = 16;
    disconnectionAlarm.BeepOnOff = !silentMode;
    ledStateStruct ledsOff = {.pin = 4, .state = 0};
    while (1)
    {
        uint32_t now;
        uint32_t now2;

        now2 = millis();
        if (!Joystick.isConnected)
        {

            xQueueSend(qLED, &ledsOff, 100);
            while (!Joystick.isConnected)
            {

                Joystick.isConnected = SerialBT.connected(portMAX_DELAY);
                if ((!(Joystick.isConnected)) && millis() - now2 > 5000)
                {
                    uint8_t pgood = analogReadMilliVolts(ANALOG_PIN_PGOOD_N) > PGOOD_THRESHOLD;
                    if (pgood == HIGH)
                        addAlarmToQueue(&disconnectionAlarm);

                    now2 = millis();
                    printf("not connected - charger: %d\n", pgood);
                }
                if (!Joystick.isConnected && millis() - now > BT_RESET_TIMEOUT_SEC * 1000 && BT_TIMEOUT_ACTIVE)
                {
                    alarmMessage_typeDef rebootAlarm;
                    rebootAlarm.Pattern = VERY_LONG_BEEP;
                    rebootAlarm.buzzerPin = BUZZER_PIN;
                    rebootAlarm.TimePeriod = 32;
                    rebootAlarm.BeepOnOff = !silentMode;
                    addAlarmToQueue(&rebootAlarm);
                    logMsg(__ASSERT_FUNC, "restarting esp", 0);
                    // vTaskDelay(1010);
                    ESP.restart(); /** @note The device might need a reboot if connection isn't established within the timeout*/
                }
                // if (!Joystick.isConnected && (millis() - now2) > 3000)
                // {
                //     alarmMessage_typeDef notConnectedAlarm;
                //     notConnectedAlarm.Pattern = 0xFFF;
                //     notConnectedAlarm.AlarmCount = 1;
                //     notConnectedAlarm.BeepOnOff = 1;
                //     notConnectedAlarm.buzzerPin = BUZZER_PIN;
                //     notConnectedAlarm.frequency = 3000;
                //     notConnectedAlarm.TimePeriod = 12;
                //     addAlarmToQueue(&notConnectedAlarm);
                //     now2 = millis();
                // }
                vTaskDelay(1);
            }
            alarmMessage.Pattern = 0xF0F00000;
            alarmMessage.frequency = 3000;
            alarmMessage.TimePeriod = 32;
            alarmMessage.AlarmCount = 1;
            addAlarmToQueue(&alarmMessage);
            logMsg(__ASSERT_FUNC, "Bluetooth_Connection", Joystick.isConnected);

            printf("Took %d seconds\n", (millis() - now) / 1000);
            vQueueDelete(qTransmitBT);
            qTransmitBT = xQueueCreate(10, sizeof(MessageStruct));
            printf("Recreated Queue\n");
            now = millis();
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
                    switch (commBT[0])
                    {
                    case '1':
                        Joystick.mode = ACTION_MODE;
                        break;
                    case '2':
                    {
                        bool f = 1;
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
                    printf("got command in packet: %x\n", commBT[COMMAND_BYTE_INDEX]);
                    switch (commBT[COMMAND_BYTE_INDEX])
                    {
                    case CMD_START_ACTION_MODE:
                        Joystick.mode = ACTION_MODE;
                        vQueueDelete(qTransmitBT);
                        qTransmitBT = xQueueCreate(10, sizeof(MessageStruct));
                        printf("Recreated Queue\n");
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
            if (Joystick.isConnected == true)
            {
                SerialBT.write(byteArr, sizeof(byteArr));
                if (SerialBT.available())
                {
                    SerialBT.readBytes(commBT, COMMAND_PACKET_SIZE);
                    printf("got command: %x:%d:%d\n", commBT[COMMAND_BYTE_INDEX], commBT[COMMAND_BYTE_INDEX + 1], commBT[COMMAND_BYTE_INDEX + 2]);
                    switch (commBT[COMMAND_BYTE_INDEX])
                    {
                    case CMD_TRANSMIT_RATE:
                        Joystick.transmitRateMS = commBT[COMMAND_BYTE_INDEX + 1] * 10;
                        break;
                    case CMD_ALPHA:
                        Joystick.alpha = (float)(commBT[COMMAND_BYTE_INDEX + 1]) / 100.0;
                        break;
                    case CMD_LED:
                        ledStateStruct ledMessage;
                        ledMessage.pin = commBT[COMMAND_BYTE_INDEX + 1];
                        ledMessage.state = commBT[COMMAND_BYTE_INDEX + 2];
                        xQueueSend(qLED, &ledMessage, 100);
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

                        addAlarmToQueue(&alarmMessage);
                        break;
                    }
                    case CMD_SILENT_MODE:
                        silentMode = commBT[COMMAND_BYTE_INDEX + 1];
                        break;
                    case CMD_BAT_THRESHOLD:
                    {
                        batteryVoltageThreshold = commBT[COMMAND_BYTE_INDEX + 1] / 10.0;
                    }
                    default:
                        break;
                    }
                }


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
    TaskHandle_t bluetoothManager_taskHandle = NULL;
    int8 loop = 1;
    int8 comm[10];
    uint8_t ledMessage[50];
    if (ADC_TASK_ENABLED)
        xTaskCreate(readJoystick_task, "Analog Read Task", READJOYSTICK_STACK_SIZE, NULL, 20, NULL);
    if (BT_TASK_ENABLED)
    {
        xTaskCreate(bluetoothManager_task, "Bluetooth Transmit Task", BLUETOOTHMANAGER_STACK_SIZE, NULL, 21, &bluetoothManager_taskHandle);
    }
    while (1)
    {
        vTaskDelay(1);
        if (uxQueueMessagesWaiting(qTransmitBT) == 100)
        {
            vQueueDelete(qTransmitBT);
            qTransmitBT = xQueueCreate(100, sizeof(MessageStruct));
            logMsg(__ASSERT_FUNC, "qTransmitBT recreated", 1);
            // Joystick.isConnected = 0;
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

                uint8_t mac[6];
                for (int8 i = 0; i < 6; i++)
                {
                    xQueueReceive(qTransmitBT, mac + i, 1000);
                }
                Serial.printf("BT MAC: %x:%x:%x:%x:%x:%x\n", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5]);
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
void ledManager_task(void *arg)
{
    pinMode(LED_GREEN, OUTPUT);
    pinMode(LED_YELLOW, OUTPUT);
    pinMode(LED_RED, OUTPUT);
    uint8_t leds[3] = {LED_GREEN, LED_YELLOW, LED_RED};
    uint32_t now = millis();
    bool f = 0;
    ledStateStruct ledStates;
    ledStates.state = 10;
    ledStates.pin = 0;
    ledStateStruct msgTemp;
    ledStateStruct green = {.pin = LED_GREEN, .state = 0};
    ledStateStruct yellow = {.pin = LED_RED, .state = 0};
    ledStateStruct red = {.pin = LED_YELLOW, .state = 0};
    digitalWrite(green.pin, green.state);
    digitalWrite(yellow.pin, yellow.state);
    digitalWrite(red.pin, red.state);
    while (1)
    {
        vTaskDelay(10);
        if (uxQueueMessagesWaiting(qLED) > 0)
        {
            xQueueReceive(qLED, &msgTemp, 0);
            ledStates = msgTemp;
            switch (ledStates.pin)
            {
            case 1:
                green.state = ledStates.state;
                break;

            case 2:
                red.state = ledStates.state;
                break;

            case 3:
                yellow.state = ledStates.state;
                break;

            case 4:
                green.state = ledStates.state;
                red.state = ledStates.state;
                yellow.state = ledStates.state;
                break;

            default:
                break;
            }
            digitalWrite(green.pin, green.state);
            digitalWrite(yellow.pin, yellow.state);
            digitalWrite(red.pin, red.state);
            now = millis();
        }
        // if (Joystick.battery <3.3)

        if (millis() - now > 1000)
        {
            if (Joystick.battery < batteryVoltageThreshold)
            {
                digitalWrite(green.pin, green.state && f);
                digitalWrite(yellow.pin, yellow.state && f);
                digitalWrite(red.pin, red.state && f);
                uint8_t f1, s1, s2, s3;
                s1 = green.state;
                s2 = yellow.state;
                s3 = red.state;
                f1 = f ? 1 : 0;
                // printf("\n low ledStates: F:%d Green:%d Yellow:%d Red:%d\n", f1, s1, s2, s3);
                f = !f;
            }
            else
            {
                digitalWrite(green.pin, green.state);
                digitalWrite(yellow.pin, yellow.state);
                digitalWrite(red.pin, red.state);
                uint8_t f1, s1, s2, s3;
                s1 = green.state;
                s2 = yellow.state;
                s3 = red.state;
                f1 = f ? 1 : 0;
                // printf("\n high ledStates: F:%d Green:%d Yellow:%d Red:%d\n", f1, s1, s2, s3);
                f = true;
            }

            now = millis();
        }
    }
}

void readBatteryWhileCharging_task(void *arg)
{
    uint16_t batmV;
    uint16_t timer = 0;
    while (1)
    {
        timer++;
        batmV = 2 * analogReadMilliVolts(BATTERY_SENSE_PIN);
        if (batmV < 2000)
        {
            timer = 0;
            batmV_GLOBAL = 0;
        }
        if (timer > 400)
        {
            batmV_GLOBAL = batmV;
        }
        vTaskDelay(5);
    }
}
