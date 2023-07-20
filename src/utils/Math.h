#pragma once

#include <iostream>

class Math
{
public:
    static int decimalToOctal(int dec)
    {
        int octal = 0;
        int remainder = 0;
        int i = 1;

        while (dec != 0)
        {
            remainder = dec % 8;
            dec /= 8;
            octal += remainder * i;
            i *= 10;
        }
        
        return octal;
    }
};