/* Includes ------------------------------------------------------------------*/
#include "Alarm_FreeRTOS.h"
#include "Logging.h"
uint32_t Pattern = 0;
uint32_t alarmCount = 0;
uint32_t BeepPattern_Shifted = 0;
uint32_t Pattern_Shifted = 0;
uint8_t BeepOnOff = 0;
uint8_t Continious = 0;
uint8_t buzzerPin;
uint8_t Message_Queue_Index = 0;
uint8_t Message_Queue_CurrentIndex = 0;
uint8_t shiftCount = 0;
uint8_t alarm_Time = 0;

bool usingTone = 0;

alarmMessage_typeDef alarmMessageQueue[MESSAGE_QUEUE_SIZE];
QueueHandle_t qAlarmMessage;
// TimerHandle_t alarmTimer;

// void Farand_Alarm(uint32_t PatternInit, uint8_t NumberInit, uint8_t AlarmTime, uint8_t BeepOnOffInit)
// {
// 	// buzzerPin = _buzzerPin;
// 	// xTimerCreate("Alarm Timer",32,1,NULL,)
// 	if (Message_Queue_Index < MESSAGE_QUEUE_SIZE - 15)
// 	{
// 		Message_Queue_Index++;

// 		alarmMessageQueue[Message_Queue_Index].Pattern = PatternInit;
// 		alarmMessageQueue[Message_Queue_Index].AlarmCount = NumberInit;
// 		alarmMessageQueue[Message_Queue_Index].BeepOnOff = BeepOnOffInit;
// 		alarmMessageQueue[Message_Queue_Index].TimePeriod = AlarmTime;

// 		if (Message_Queue_Index == 1) // check if this is the first alarm message
// 		{
// 			// load initial values for the first alarm
// 			Pattern = alarmMessageQueue[1].Pattern;
// 			Pattern_Shifted = Pattern;
// 			alarmCount = alarmMessageQueue[1].AlarmCount;
// 			BeepOnOff = alarmMessageQueue[1].BeepOnOff;
// 			alarmMessageQueue[1].TimePeriod = AlarmTime;
// 			alarm_Time = AlarmTime;
// 			shiftCount = alarm_Time; // if it's the first alarm,  shiftCount should be preset
// 			Message_Queue_CurrentIndex++;
// 		}
// 	}
// }
void addAlarmToQueue(alarmMessage_typeDef *alarm)
{
	for (uint8_t i = 0; i < alarm->AlarmCount; i++)
		xQueueSend(qAlarmMessage, alarm, portMAX_DELAY);
}
void alarm_task(void *arg)
{
	alarmMessage_typeDef alarm;
	qAlarmMessage = xQueueCreate(100, sizeof(alarmMessage_typeDef));
	// alarm.Pattern = SHORT_BEEP_X1;
	// alarm.AlarmCount = 2;
	// alarm.buzzerPin = 19;
	// alarm.frequency = 2000;
	// alarm.BeepOnOff = 1;
	// alarm.TimePeriod = 32;
	// xQueueSend(qAlarmMessage, &alarm, 1);
	while (1)
	{
		if (uxQueueMessagesWaiting(qAlarmMessage) == 100)
		{
			vQueueDelete(qAlarmMessage);
			qAlarmMessage = xQueueCreate(100, sizeof(alarmMessage_typeDef));
			logMsg(__ASSERT_FUNC, "qTransmitBT recreated", 1);
		}
		// if (uxQueueMessagesWaiting(qAlarmMessage) > 0)
		if (xQueueReceive(qAlarmMessage, &alarm, 1))
		{
			printf("got alarm with pattern %x,period %d, count %d\n", alarm.Pattern, alarm.TimePeriod, alarm.AlarmCount);
			uint32_t patternShifted = alarm.Pattern;
			for (uint8_t i = 0; i < alarm.TimePeriod; i++)
			{
				if (alarm.Pattern & (1 << i))
					if (usingTone)
						tone(alarm.buzzerPin, alarm.frequency);
					else
						digitalWrite(alarm.buzzerPin, 1);
				else if (usingTone)
					noTone(alarm.buzzerPin);
				else
					digitalWrite(alarm.buzzerPin, 0);
				// patternShifted >> 1;
				vTaskDelay(31);
			}
			if (usingTone)
				noTone(alarm.buzzerPin);
			else
				digitalWrite(alarm.buzzerPin, 0);
		}
		vTaskDelay(1);
	}
}
// void Farand_Update_Alarm(uint8_t _buzzerPin)
// {
// 	// printf("updating alarm\n");
// 	// digitalWrite(23, !digitalRead(23));
// 	if (Message_Queue_Index > 0)
// 	{
// 		if (Pattern_Shifted & 0x01)
// 		{
// 			if (BeepOnOff == 1)
// 			{
// 				// digitalWrite(buzzerPin, 1);
// 				tone(_buzzerPin, 2000);
// 				printf("1 ok\n");
// 			}
// 		}
// 		else
// 		{
// 			// digitalWrite(buzzerPin, 0);
// 			noTone(_buzzerPin);
// 			printf("2 ok\n");
// 		}

// 		Pattern_Shifted = Pattern_Shifted >> 1;
// 		shiftCount--;
// 		if (shiftCount == 0)
// 		{
// 			printf("3 ok\n");

// 			alarmCount--; // Count Number of alarm messages
// 			if (alarmCount > 0)
// 			{
// 				printf("4 ok\n");

// 				Pattern_Shifted = Pattern; // Reload Pattern
// 				shiftCount = alarm_Time;
// 			}
// 			else
// 			{
// 				printf("5 ok\n");

// 				// Go to next alarm if any
// 				if (Message_Queue_CurrentIndex == Message_Queue_Index)
// 				{
// 					printf("6 ok\n");

// 					// if all alarms are played reset everything to wait for new alarm messages
// 					Message_Queue_CurrentIndex = 0;
// 					Message_Queue_Index = 0;
// 					Pattern_Shifted = 0;
// 					// TurnOff(LED_1);
// 					// digitalWrite(buzzerPin, 0);
// 					noTone(_buzzerPin);
// 				}
// 				else
// 				{
// 					printf("7 ok\n");

// 					// Load next alarm message
// 					Message_Queue_CurrentIndex++; // read next message
// 					Pattern = alarmMessageQueue[Message_Queue_CurrentIndex].Pattern;
// 					Pattern_Shifted = Pattern;
// 					alarmCount = alarmMessageQueue[Message_Queue_CurrentIndex].AlarmCount;
// 					BeepOnOff = alarmMessageQueue[Message_Queue_CurrentIndex].BeepOnOff;
// 					alarm_Time = alarmMessageQueue[Message_Queue_Index].TimePeriod;
// 					shiftCount = alarm_Time;
// 				}
// 			}
// 		}
// 	}
// }
void Farand_Reset_Alarm(void)
{
	Pattern = 0;
	alarmCount = 0;
	BeepPattern_Shifted = 0;
	Pattern_Shifted = 0;
	BeepOnOff = 0;
	Continious = 0;

	Message_Queue_Index = 0;
	Message_Queue_CurrentIndex = 0;
	shiftCount = 0;
	alarm_Time = 0;
}

// void alarm_Task(void *arg)
// {
// 	xQueueCreate(MESSAGE_QUEUE_SIZE, sizeof(alarmMessage_typeDef));
// 	alarmMessage_typeDef *alarmPtr = NULL;
// 	hw_timer_t *hwAlarmTimer = NULL;

// 	hwAlarmTimer = timerBegin(0, getCpuFrequencyMhz(), true);

// 	timerAttachInterrupt(hwAlarmTimer, Farand_Update_Alarm, true);

// 	timerAlarmWrite(hwAlarmTimer, 1000000 / 32, true);

// 	timerAlarmEnable(hwAlarmTimer);

// 	while (1)
// 	{
// 		xQueueReceive(qAlarmMessage, alarmPtr, portMAX_DELAY);
// 		if (alarmPtr != NULL)
// 		{
// 			// Farand_Alarm();
// 		}
// 		alarmPtr = NULL;
// 		vTaskDelay(1);
// 	}
// }

// void IRAM_ATTR hmm_()
// {
// 	digitalWrite(21, !digitalRead(21));
// }