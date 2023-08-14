#ifndef TASKS_DEFINITION_H
#define TASKS_DEFINITION_H

#include "General.h"
#include "BluetoothSerial.h"
// #include "main.h"

/** @note Uncomment in case of using multiple bytes for multiple buttons*/
#define USING_SINGLE_BYTE_FOR_BUTTONS
// #define USING_MULTI_BYTES_FOR_BUTTONS

typedef struct
{
    uint16_t adc[MAX_ADC_COUNT];
    bool button[MAX_DIG_COUNT];

} MessageStruct;

/**
 * @brief Reads values from analog joystick axes.
 * @note Encapsulated in function to serve modularity. Change if needed.
 * @param arr array of adc values of joystick axes.
 * @param adcPins array of adc pin numbers.
 * @retval does not return. the values are changed in the array.
 */
extern void readADC(int16 arr[MAX_ADC_COUNT], int8 adcPins[MAX_ADC_COUNT]);

/**
 * @brief Reads values from joystick buttons.
 * @note Encapsulated in function to serve modularity. Change if needed.
 * @param arr array of digital values of joystick buttons.
 * @param digPins array of digital pin numbers.
 * @retval does not return. the values are changed in the array.
 */
extern void readButtons(bool arr[MAX_DIG_COUNT], bool digPins[MAX_DIG_COUNT]);

/**
 * @brief Constructs the message to be transmitted through bluetooth
 * @note Encapsulated in function to serve modularity. Change if needed.
 * @param joystick Axes and buttons values are stored in this MessageStruct variable
 * @param arr The message buffer. An array of bytes for ease of transfer with HC-05
 * @retval does not return. the values are changed in the array.
 */
extern void constructByteArray(MessageStruct *joystick, byte arr[BYTE_ARRAY_SIZE]);

extern void adcReadTask(void *arg);
extern void transmitByteTask(void *arg);
extern void serialCommandsTask(void *arg);

#endif