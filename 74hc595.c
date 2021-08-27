// #define F_CPU 1200000 // 1MHz

#include <avr/io.h>
#include <util/delay.h>

#define size_t unsigned int

typedef enum
{
    MSBFIRST,
    LSBFIRST
} DIRECTION;

#define ARRAY_COUNT(x) (sizeof(x) / sizeof((x)[0]))

/***************************************

Configure Connections

****************************************/

#define HC595_PORT PORTB
#define HC595_DDR DDRB

#define HC595_DS_POS PB3 // Data pin (DS) pin location

#define HC595_SH_CP_POS PB2 // Shift Clock (SH_CP) pin location
#define HC595_ST_CP_POS PB1 // Store Clock (ST_CP) pin location

/***************************************
Configure Connections ***ENDS***
****************************************/

//Initialize HC595 System

void HC595Init()
{
    //Make the Data(DS), Shift clock (SH_CP), Store Clock (ST_CP) lines output
    HC595_DDR |= ((1 << HC595_SH_CP_POS) | (1 << HC595_ST_CP_POS) | (1 << HC595_DS_POS));
}

//Low level macros to change data (DS)lines
#define HC595DataHigh() (HC595_PORT |= (1 << HC595_DS_POS))

#define HC595DataLow() (HC595_PORT &= (~(1 << HC595_DS_POS)))

//Sends a clock pulse on SH_CP line
void HC595Pulse()
{
    //Pulse the Shift Clock

    HC595_PORT |= (1 << HC595_SH_CP_POS); //HIGH

    HC595_PORT &= (~(1 << HC595_SH_CP_POS)); //LOW
}

//Sends a clock pulse on ST_CP line
void HC595Latch()
{
    //Pulse the Store Clock

    HC595_PORT |= (1 << HC595_ST_CP_POS); //HIGH
    _delay_loop_1(1);

    HC595_PORT &= (~(1 << HC595_ST_CP_POS)); //LOW
    _delay_loop_1(1);
}

void H595LatchLow()
{
    HC595_PORT &= (~(1 << HC595_ST_CP_POS));
}

void H595LatchHigh()
{
    HC595_PORT |= (1 << HC595_ST_CP_POS);
}

/*

Main High level function to write a single byte to
Output shift register 74HC595. 

Arguments:
   single byte to write to the 74HC595 IC

Returns:
   NONE

Description:
   The byte is serially transfered to 74HC595
   and then latched. The byte is then available on
   output line Q0 to Q7 of the HC595 IC.

*/
void HC595Write(uint8_t data, DIRECTION direction)
{
    //Send each 8 bits serially
    H595LatchLow();
    //Order is MSB first
    for (uint8_t i = 0; i < 8; i++)
    {
        //Output the data on DS line according to the
        //Value of MSB
        if (direction == MSBFIRST)
        {
            if (data & 0b10000000)
            {
                //MSB is 1 so output high
                HC595DataHigh();
            }
            else
            {
                //MSB is 0 so output high
                HC595DataLow();
            }
        }
        else
        {
            if (data & 0b00000001)
            {
                //LSB is 1 so output high
                HC595DataHigh();
            }
            else
            {
                //LSB is 0 so output high
                HC595DataLow();
            }
        }

        HC595Pulse();                                           // Pulse the Clock line
        data = (direction == MSBFIRST) ? data << 1 : data >> 1; // Now bring next bit depending upon the direction requested
    }

    // All bits have been transferred to shift register, so next step is to send a high control on latch pin
    H595LatchHigh();
}

/*

Simple Delay function approx 0.5 seconds

*/

void Wait()
{
    for (uint8_t i = 0; i < 30; i++)
    {
        _delay_loop_2(0);
    }
}

int main()
{
    uint8_t led_pattern1[] = {
        0b10000000,
        0b01000000,
        0b00100000,
        0b00010000,
        0b00001000,
        0b00000100,
        0b00000010,
        0b00000001,
        0b00000000,
    };

    uint8_t led_pattern2[] = {
        0b10000000,
        0b11000000,
        0b11100000,
        0b11110000,
        0b11111000,
        0b11111100,
        0b11111110,
        0b11111111,
        0b00000000,
    };

    uint8_t led_pattern3[] = {
        0b00011000,
        0b00111100,
        0b01111110,
        0b11111111,
        0b11100111,
        0b11000011,
        0b10000001,
        0b00000000,
        0b10000001,
        0b11000011,
        0b11100111,
        0b11111111,
        0b00000000,
    };

    size_t len_pattern1 = ARRAY_COUNT(led_pattern1);
    size_t len_pattern2 = ARRAY_COUNT(led_pattern2);
    size_t len_pattern3 = ARRAY_COUNT(led_pattern3);

    //Initialize HC595 system

    HC595Init();

    while (1)
    {
        for (uint8_t i = 0; i < len_pattern1; i++)
        {
            HC595Write(led_pattern1[i], MSBFIRST); //Write the data to HC595
            _delay_ms(100);
        }

        for (uint8_t i = 0; i < len_pattern1; i++)
        {
            HC595Write(led_pattern1[i], LSBFIRST); //Write the data to HC595
            _delay_ms(100);
        }

        for (uint8_t i = 0; i < len_pattern2; i++)
        {
            HC595Write(led_pattern2[i], MSBFIRST); //Write the data to HC595
            _delay_ms(100);
        }

        for (uint8_t i = 0; i < len_pattern2; i++)
        {
            HC595Write(led_pattern2[i], LSBFIRST); //Write the data to HC595
            _delay_ms(100);
        }

        for (uint8_t i = 0; i < len_pattern3; i++)
        {
            HC595Write(led_pattern3[i], LSBFIRST); //Write the data to HC595
            _delay_ms(100);
        }

        for (uint8_t i = 0; i < len_pattern3; i++)
        {
            HC595Write(led_pattern3[i], MSBFIRST); //Write the data to HC595
            _delay_ms(100);
        }

        for (int repeat = 0; repeat < 4; repeat++)
        {
            for (uint8_t i = 0; i < len_pattern1; i++)
            {
                HC595Write(led_pattern1[i], MSBFIRST); //Write the data to HC595
                _delay_ms(20);
            }

            for (uint8_t i = 0; i < len_pattern1; i++)
            {
                HC595Write(led_pattern1[i], LSBFIRST); //Write the data to HC595
                _delay_ms(20);
            }
        }
    }
}