#ifndef UTIL_H
#define UTIL_H

#include <Arduino.h>

void strLPad(char retString[], byte len, char pad);

void truncFloat(char retStr[], byte len);
void vftoa(unsigned long val, char retStr[], unsigned int divisor, boolean decimal);

unsigned long pow10(byte power);
unsigned long pow2(byte power);

template<class T, size_t N>
constexpr size_t array_size(T(&)[N]) { return N; }
template<class T, size_t N, size_t M>
constexpr size_t array_size(T(&)[N][M]) { return N; }

#endif