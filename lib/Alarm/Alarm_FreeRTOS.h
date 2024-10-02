#ifndef __ALARM_FREERTOS_H
#define __ALARM_FREERTOS_H
// #include "General.h"
#include "Arduino.h"

#define ALARM_DEBUG_MODE_UART false

#define BUZZER_PIN 21

typedef struct
{
	uint32_t Pattern;
	uint32_t AlarmCount = 1;
	uint8_t BeepOnOff = 1;
	uint8_t TimePeriod = 32;
	uint8_t buzzerPin = BUZZER_PIN;
	uint16_t frequency = 2500;

} alarmMessage_typeDef;

// extern TimerHandle_t alarmTimer;
extern QueueHandle_t qAlarmMessage;
extern hw_timer_t *hwAlarmTimer;
// Alarm Message Definition
#define MEDIUM_BEEP_X1 0x000000FF
#define MEDIUM_BEEP_X2 0x00FF00FF
#define LONG_BEEP 0x0000FFFF
#define VERY_LONG_BEEP 0xFFFFFFFF
#define BEETHOVEN_BEEP 0xFFFFF333
#define EMERGENCY_BEEP 0x0F0F0F0F
#define PULSE_TRAIN_BEEP 0x33333333
#define SHORT_BEEP_X1 0x00000003
#define SHORT_BEEP_X2 0x00000033
#define SHORT_BEEP_X3 0x00000333
#define SHORT_BEEP_X4 0x00003333
#define DELAYED_SHORT_BEEP_X1 0xC0000000
#define DELAYED_SHORT_BEEP_X2 0xCC000000
#define VERYSHORT_BEEP_X1 0x00000001
#define BEEP_ON 0x01
#define BEEP_OFF 0x00
#define MESSAGE_QUEUE_SIZE 32

extern bool usingTone;

extern void Farand_Alarm(uint32_t PatternInit, uint8_t NumberInit, uint8_t AlarmTime, uint8_t BeepOnOffInit);
// extern void Farand_Update_Alarm(uint8_t _buzzerPin);
// extern void Farand_Reset_Alarm(void);
extern void addAlarmToQueue(alarmMessage_typeDef *alarm);

extern void alarm_task(void *arg);
#endif /*__Alarm_H */
