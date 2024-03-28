#ifndef BINARY_H
#define BINARY_H

#include <stdint.h>

void textToBinary(const char *text, int textLength, char *binary, int binaryLength);
void uint8ToBinary(uint8_t number, char *binary);
void uint16ToBinary(uint16_t number, char *binary);

#endif /* BINARY_H */
