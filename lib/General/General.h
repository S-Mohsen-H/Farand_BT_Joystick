#ifndef GENERAL_H
#define GENERAL_H
#include "Arduino.h"

// #include "main.h"
#include "BluetoothSerial.h"
#include "Logging.h"
#include "bitset"
#include "string"

#define uart_debug_mode 1

#ifndef int8
#define int8 uint8_t
#endif
#ifndef int16
#define int16 uint16_t
#endif

#define ADC_PIN_1 36
#define ADC_PIN_2 39
#define ADC_PIN_3 34

#define ADC_COUNT 3
#define MAX_ADC_COUNT 3

#define DIG_PIN_1 25
#define DIG_PIN_2 26
#define DIG_PIN_3 27

#define DIG_COUNT 1
#define MAX_DIG_COUNT 3

#define BYTE_ARRAY_SIZE (ADC_COUNT * 2 + DIG_COUNT + 2)

extern QueueHandle_t qUART;
extern QueueHandle_t qSerialCommands;
// extern QueueHandle_t qTaskManager;

extern std::string intToBinaryString(int number);

extern String numToBin(uint16_t num);

#endif
