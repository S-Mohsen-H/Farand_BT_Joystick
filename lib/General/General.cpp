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