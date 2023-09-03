#ifndef GENERAL_H
#define GENERAL_H
#include "Arduino.h"

// #include "main.h"
#include "BluetoothSerial.h"
#include "Logging.h"
#include "bitset"
#include "string"

/////////////////////////////** @note DEFINE MACROS *///////////////////////

#define uart_debug_mode 0

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

#define BUTTON_COUNT 1
#define MAX_BUTTON_COUNT 3

// #define BYTE_ARRAY_SIZE (ADC_COUNT * 1.5 + 1 + 2)
#define BYTE_ARRAY_SIZE 8

#define ACTION_MODE 0
#define COMMAND_MODE_SERIAL 1
#define COMMAND_MODE_PACKET 2
#define COMMAND_PACKET_SIZE 2

#define BT_TRANSMIT_RATE_MS 100

#define USING_MULTI_SAMPLING
#define SAMPLE_COUNT 50
#define INITIAL_SAMPLING_COEFFICIENT 0.8
/////////////////////////////** @note TYPEDEFS *///////////////////////

typedef struct
{
    bool isConnected;
    bool mode;
    uint16_t transmitRate;
} Joystick_TypeDef;

/////////////////////////////** @note QUEUES *///////////////////////

extern QueueHandle_t qUART;
extern QueueHandle_t qSerialCommands;
// extern QueueHandle_t qTaskManager;
extern QueueHandle_t qADC;


/////////////////////////////** @note VARIABLES *///////////////////////

extern Joystick_TypeDef Joystick;
// extern JBT;

///////////////////////////** @note FUNCTIONS */////////////////////////

extern std::string intToBinaryString(int number);

extern String numToBin(uint16_t num);
/** @note end FUNCTIONS
 */
#endif
