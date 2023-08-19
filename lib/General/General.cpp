#include "General.h"

std::string intToBinaryString(int number)
{
    std::bitset<sizeof(int) * 8> binary(number);
    return binary.to_string();
}
String numToBin(uint16_t num)
{
    String str;
    for (int i = 15; i >= 0; i--)
    {
        str += ((num >> i) & 1) ? '1' : '0';
    }
    return str;
}