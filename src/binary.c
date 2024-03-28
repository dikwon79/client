#include "binary.h"

#define magic1 7
#define magic2 9
#define magic3 8
#define magic4 16

void textToBinary(const char *text, int textLength, char *binary, int binaryLength)
{
    int i;
    int j;
    for(i = 0; i < textLength; i++)
    {
        char currentChar = text[i];
        for(j = magic1; j >= 0; j--)
        {
            binary[(i * magic2) + (magic1 - j)] = ((currentChar >> j) & 1) ? '1' : '0';
        }
        binary[(i + 1) * magic2 - 1] = ' ';    // Space separator
    }
    binary[binaryLength] = '\0';    // Null-terminate the string
}

void uint8ToBinary(uint8_t number, char *binary)
{
    int i;
    for(i = magic1; i >= 0; i--)
    {
        binary[i] = ((number >> (magic1 - i)) & 1) ? '1' : '0';
    }
    binary[magic3] = '\0';    // 널 종료문자 추가
}

// void uint16ToBinary(uint16_t number, char *binary2)
//{
//     int i;
//     for(i = magic4; i >= 0; i--)
//
//     {
//         binary2[magic4 - i - 1] = ((number >> i) & 1) ? '1' : '0';
//
//     }
//     binary2[magic4] = '\0';    // 널 종료문자 추가
// }
void uint16ToBinary(uint16_t number, char *binary)
{
    int i;
    for(i = magic4 - 1; i >= 0; i--)
    {
        binary[i] = ((number >> (magic4 - 1 - i)) & 1) ? '1' : '0';
    }
    binary[magic4] = '\0';    // 널 종료 문자 추가
}