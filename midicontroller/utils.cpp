#include <Arduino.h>
#include "utils.h"


int sign(float number) {
    return (number > 0) - (number < 0);
}
