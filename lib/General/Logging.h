#ifndef LOGGING_H
#define LOGGING_H
#include "General.h"
// #include "main.h"

/**
 * @brief printf("[%s][%d]:%d-%s(%d)\n", funcName, index, msg, isOK ? "OK" : "Failure", isOK);
 * @note "[%funcName][%index]:%msg-%isOK(%isOK)\n"
 */
extern bool logMsg(const char *funcName, int msg, bool isOK, uint8_t index);

/**
 * @brief printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg, isOK ? "OK" : "Failure", isOK);
 * @note "[%funcName][%index]:%msg-%isOK(%isOK)\n"
 */
extern bool logMsg(const char *funcName, const char *msg, bool isOK, uint8_t index);

/**
 * @brief printf("[%s]:%d-%s(%d)\n", funcName, msg, isOK ? "OK" : "Failure", isOK);
 * @note "[%funcName]:%msg-%isOK(%isOK)\n", fu
 */
extern bool logMsg(const char *funcName, int msg, bool isOK);

/**
 * @brief printf("[%s]:%s-%s(%d)\n", funcName, msg, isOK ? "OK" : "Failure", isOK);
 * @note "[%funcName]:%msg-%isOK(%isOK)\n", fu
 */
extern bool logMsg(const char *funcName, const char *msg, bool isOK);

/**
 * @brief SerialBT->printf("[%s][%d]:%d-%s(%d)\n", funcName, index, msg, isOK ? "OK" : "Failure", isOK);
 * @note "[%funcName][%index]:%msg-%isOK(%isOK)\n"
 */
extern bool logMsgBT(BluetoothSerial *SerialBT, const char *funcName, int msg, bool isOK, uint8_t index);

/**
 * @brief SerialBT->printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg, isOK ? "OK" : "Failure", isOK);
 * @note "[%funcName][%index]:%msg-%isOK(%isOK)\n"
 */
extern bool logMsgBT(BluetoothSerial *SerialBT, const char *funcName, const char *msg, bool isOK, uint8_t index);

/**
 * @brief SerialBT->printf("[%s]:%d-%s(%d)\n", funcName, msg, isOK ? "OK" : "Failure", isOK);
 * @note "[%funcName]:%msg-%isOK(%isOK)\n", fu
 */
extern bool logMsgBT(BluetoothSerial *SerialBT, const char *funcName, int msg, bool isOK);

/**
 * @brief SerialBT->printf("[%s]:%s-%s(%d)\n", funcName, msg, isOK ? "OK" : "Failure", isOK);
 * @note "[%funcName]:%msg-%isOK(%isOK)\n", fu
 */
extern bool logMsgBT(BluetoothSerial *SerialBT, const char *funcName, const char *msg, bool isOK);


// ///////////////////////////////////////////////////////////////
////////////// @note String
// //////////////////////////////////////////////////////////////
extern bool logMsg(const char *funcName, String *msg, bool isOK);

extern bool logMsg(const char *funcName, String *msg, bool isOK, uint8_t index);

extern bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, String *msg, bool isOK);

extern bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, String *msg, bool isOK, uint8_t index);






#endif