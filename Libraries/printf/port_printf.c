#include "SEGGER_RTT.h"
#include "printf.h"

void _putchar(char character)
{
    SEGGER_RTT_PutChar(0, character);
}
