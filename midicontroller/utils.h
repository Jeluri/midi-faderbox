#ifndef UTILS_H
#define UTILS_H

#include <Arduino.h>


template <typename T>
Print& operator<<(Print& printer, T value) {
    printer.print(value);
    return printer;
}

int sign(float number);

#endif
