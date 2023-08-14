#include "Logging.h"
// void yourFunction(void (*assertFunc)(const char *, const char *, int, const char *));

bool logMsg(const char *funcName, const char *msg, bool isOK)
{
    if (uart_debug_mode)
        printf("[%s]:%s-%s\n", funcName, msg, isOK ? "OK" : "Failure");
    return isOK;
}

bool logMsg(const char *funcName, int msg, bool isOK)
{
    if (uart_debug_mode)
        printf("[%s]:%d\n", funcName, msg);
    return isOK;
}

bool logMsg(const char *funcName, const char *msg, bool isOK, uint8_t index)
{
    if (uart_debug_mode)
        printf("[%s][%d]:%s\n", funcName, index, msg);
    return isOK;
}

bool logMsg(const char *funcName, int msg, bool isOK, uint8_t index)
{
    if (uart_debug_mode)
        printf("[%s][%d]:%d\n", funcName, index, msg);
    return isOK;
}
