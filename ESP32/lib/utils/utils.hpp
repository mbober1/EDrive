#pragma once
#include "freertos/FreeRTOS.h"

void * memcpy (void * destination, const void * source, size_t num);
void * memset (void * ptr, int value, size_t num);
int memcmp (const void * ptr1, const void * ptr2, size_t num);