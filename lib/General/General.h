#ifndef GENERAL_H
#define GENERAL_H
#include "Arduino.h"

// #include "main.h"
#include "BluetoothSerial.h"
#include "Logging.h"
#include "bitset"
#include "string"

/////////////////////////////** @note DEFINE MACROS *///////////////////////

#define BT_SSID_1 "PIKA Joystick 1"
#define BT_SSID_2 "PIKA Joystick 2"
#define BT_SSID BT_SSID_1

#ifndef int8
#define int8 uint8_t
#endif
#ifndef int16
#define int16 uint16_t
#endif

#define TASKMANAGER_STACK_SIZE 0x3000
#define BLUETOOTHMANAGER_STACK_SIZE 0x8000
#define READJOYSTICK_STACK_SIZE 0x1000
#define ALARM_STACK_SIZE 0x2000
#define LEDMANAGER_STACK_SIZE 0x1000

#define QUEUE_ADC_SIZE 5

#define LED_GREEN 18
#define LED_YELLOW 12
#define LED_RED 23
// #define LED_GREEN 23
// #define LED_YELLOW 18
// #define LED_RED 12
// #define BUZZER_PIN 4
#define ADC_PIN_1 39
#define ADC_PIN_2 34
#define ADC_PIN_3 35
// #define ADC_PIN_4 32

#define ADC_COUNT 2
#define MAX_ADC_COUNT 3

#define DIGITAL_PIN_1 25
#define DIGITAL_PIN_2 33
#define DIGITAL_PIN_3 14
#define ANALOG_PIN_PGOOD_N 27
#define PGOOD_THRESHOLD 170
#define DIGITAL_PIN_CHARGING_N 26
#define DIGITAL_PIN_CHARGER_SENSE 17

#define BUTTON_COUNT 3
#define MAX_BUTTON_COUNT 3

#define BATTERY_SENSE_PIN 36
// #define BYTE_ARRAY_SIZE (ADC_COUNT * 1.5 + 1 + 2)
#define BYTE_ARRAY_SIZE 16
#define BYTE1A 0x1A
#define BYTE1 0xAA
#define CODING_0X1A false

#define ACTION_MODE 0
#define COMMAND_MODE_SERIAL 1
#define COMMAND_MODE_PACKET 2
#define COMMAND_MODE_AUTO_DETECTION 10

#define COMMAND_BYTE_INDEX 8
#define COMMAND_PACKET_SIZE BYTE_ARRAY_SIZE
#define BATTERY_LEVEL_INDEX 14
#define MAC_MESSAGE 0x10

extern uint8_t autoDetectionPacket[COMMAND_PACKET_SIZE];

#define CMD_START_ACTION_MODE 0xA0
#define CMD_TRANSMIT_RATE 0xC0
#define CMD_ALPHA 0xC1
#define CMD_ALARM 0xC2
#define CMD_LED 0xC3
#define CMD_SILENT_MODE 0xC4
#define CMD_BAT_THRESHOLD 0xC5

#define BT_INITIAL_TRANSMIT_RATE_MS 100

#define BT_RESET_TIMEOUT_SEC 100
#define BT_TIMEOUT_ACTIVE true

#define USING_MULTI_SAMPLING
#define SAMPLE_COUNT 50
#define INITIAL_SAMPLING_COEFFICIENT 0.8

#define UART_DEBUG_MODE 0
// #define DEBUG_READAXES
// #define DEBUG_READAXES2
// #define DEBUG_BT_PRINT_VALUES

/////////////////////////////** @note TYPEDEFS *///////////////////////

typedef struct
{
    bool isConnected;
    int8 mode;
    uint16_t transmitRateMS;
    float alpha;
    float battery;
} Joystick_TypeDef;

/////////////////////////////** @note QUEUES *///////////////////////

extern QueueHandle_t qTransmitBT;
extern QueueHandle_t qBluetoothMac;
extern QueueHandle_t qLED;
extern QueueHandle_t qBuzzer;
extern QueueHandle_t qADC;
extern QueueHandle_t qBatteryCharging;

#define ADC_TASK_ENABLED 1
#define BT_TASK_ENABLED 1

/////////////////////////////** @note VARIABLES *///////////////////////

extern Joystick_TypeDef Joystick;
// extern JBT;
extern uint16_t batmV_GLOBAL;

///////////////////////////** @note FUNCTIONS */////////////////////////

extern uint16_t getBatterySOC(uint16_t voltage);
extern std::string intToBinaryString(int number);
extern String numToBin(uint16_t num);
/** @note end FUNCTIONS
 */
#endif
