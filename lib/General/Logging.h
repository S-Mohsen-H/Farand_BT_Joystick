#ifndef LOGGING_H
#define LOGGING_H
#include "General.h"
// #include "main.h"
extern bool logMsg(const char *funcName, int msg, bool isOK, uint8_t index);
extern bool logMsg(const char *funcName, const char *msg, bool isOK, uint8_t index);
extern bool logMsg(const char *funcName, int msg, bool isOK);
extern bool logMsg(const char *funcName, const char *msg, bool isOK);
#endif