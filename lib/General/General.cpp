#include "General.h"
uint8_t autoDetectionPacket[COMMAND_PACKET_SIZE] = {0x00, 0xAA, 0x55, 0x00, 0x00, 0x00, 0x00, 0xFF, 0, 0, 0, 0, 0, 0, 0, 0};

std::string intToBinaryString(int number)
{
    std::bitset<sizeof(int) * 8> binary(number);
    return binary.to_string();
}
String numToBin(uint16_t num)
{
    String str = "";
    for (int i = 15; i >= 0; i--)
    {
        str += ((num >> i) & 1) ? '1' : '0';
    }
    return str;
}

uint16_t batterySOCLUT[] = {
    3270,
    3610,
    3690,
    3710,
    3730,
    3750,
    3770,
    3790,
    3800,
    3820,
    3840,
    3850,
    3870,
    3910,
    3950,
    3980,
    4020,
    4080,
    4150,
    4192

};
uint16_t getBatterySOC(uint16_t voltage)
{
    if (voltage < batterySOCLUT[0])
        return 0;
    for (uint8_t i = 0; i < 19; i++)
    {
        if (batterySOCLUT[i] < voltage && voltage < batterySOCLUT[i + 1])
        {
            return i * 5;
        }
        else
            continue;
    }
    if (voltage > batterySOCLUT[19])
        return 100;
    else
        return 1000;
}