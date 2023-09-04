#include "Logging.h"
// void yourFunction(void (*assertFunc)(const char *, const char *, int, const char *));

// ///////////////////////////////////////////////////////////////
////////////// @note char *
// //////////////////////////////////////////////////////////////
bool logMsg(const char *funcName, const char *msg, bool isOK)
{
    // if (uart_debug_mode)
        printf("[%s]:%s-%s(%d)\n", funcName, msg, isOK ? "OK" : "Failure", isOK);
    return isOK;
}


bool logMsg(const char *funcName, int msg, bool isOK)
{
    // if (uart_debug_mode)
        printf("[%s]:%d-%s(%d)\n", funcName, msg, isOK ? "OK" : "Failure", isOK);

    return isOK;
}


bool logMsg(const char *funcName, const char *msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg, isOK ? "OK" : "Failure", isOK);

    return isOK;
}


bool logMsg(const char *funcName, int msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        printf("[%s][%d]:%d-%s(%d)\n", funcName, index, msg, isOK ? "OK" : "Failure", isOK);
    return isOK;
}
bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, const char *msg, bool isOK)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s]:%s-%s(%d)\n", funcName, msg, isOK ? "OK" : "Failure", isOK);
    return isOK;
}


bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, int msg, bool isOK)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s]:%d-%s(%d)\n", funcName, msg, isOK ? "OK" : "Failure", isOK);

    return isOK;
}


bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, const char *msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg, isOK ? "OK" : "Failure", isOK);

    return isOK;
}


bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, int msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s][%d]:%d-%s(%d)\n", funcName, index, msg, isOK ? "OK" : "Failure", isOK);
    return isOK;
}
// ///////////////////////////////////////////////////////////////
////////////// @note String
// //////////////////////////////////////////////////////////////
bool logMsg(const char *funcName, String msg, bool isOK)
{
    // if (uart_debug_mode)
        printf("[%s]:%s-%s(%d)\n", funcName, msg.c_str(), isOK ? "OK" : "Failure", isOK);
    return isOK;
}

bool logMsg(const char *funcName, String msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg.c_str(), isOK ? "OK" : "Failure", isOK);

    return isOK;
}

bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, String msg, bool isOK)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s]:%s-%s(%d)\n", funcName, msg.c_str(), isOK ? "OK" : "Failure", isOK);
    return isOK;
}


bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, String msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg.c_str(), isOK ? "OK" : "Failure", isOK);

    return isOK;
}
// ///////////////////////////////////////////////////////////////
////////////// @note String*
// //////////////////////////////////////////////////////////////
bool logMsg(const char *funcName, String *msg, bool isOK)
{
    // if (uart_debug_mode)
        printf("[%s]:%s-%s(%d)\n", funcName, msg->c_str(), isOK ? "OK" : "Failure", isOK);
    return isOK;
}

bool logMsg(const char *funcName, String *msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg->c_str(), isOK ? "OK" : "Failure", isOK);

    return isOK;
}

bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, String *msg, bool isOK)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s]:%s-%s(%d)\n", funcName, msg->c_str(), isOK ? "OK" : "Failure", isOK);
    return isOK;
}


bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, String *msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg->c_str(), isOK ? "OK" : "Failure", isOK);

    return isOK;
}
// ///////////////////////////////////////////////////////////////
////////////// @note StringSumHelper
// //////////////////////////////////////////////////////////////
bool logMsg(const char *funcName, StringSumHelper msg, bool isOK)
{
    // if (uart_debug_mode)
        printf("[%s]:%s-%s(%d)\n", funcName, msg.c_str(), isOK ? "OK" : "Failure", isOK);
    return isOK;
}

bool logMsg(const char *funcName, StringSumHelper msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg.c_str(), isOK ? "OK" : "Failure", isOK);

    return isOK;
}

bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, StringSumHelper msg, bool isOK)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s]:%s-%s(%d)\n", funcName, msg.c_str(), isOK ? "OK" : "Failure", isOK);
    return isOK;
}


bool logMsgBT(BluetoothSerial * SerialBT,const char *funcName, StringSumHelper msg, bool isOK, uint8_t index)
{
    // if (uart_debug_mode)
        SerialBT->printf("[%s][%d]:%s-%s(%d)\n", funcName, index, msg.c_str(), isOK ? "OK" : "Failure", isOK);

    return isOK;
}

