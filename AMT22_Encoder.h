#ifndef AMT22_Encoder_h
#define AMT22_Encoder_h

#include <SPI.h>
#include "Arduino.h"

#define NO_OP 0x00
#define RESET_ENCODER 0x60
#define SET_ZERO_POINT 0x70
#define READ_TURNS 0xA0

#define READ_RATE 10000 // reads the value from the encoder every x microseconds

/*
Read the datasheet here:
    https://www.cuidevices.com/product/resource/amt22-v.pdf
    
Other resources:
    https://www.cuidevices.com/product/resource/sample-code/amt22

This is a module made for VT Sailbot for the 2024 sailbot and pep competitions, which you can use to communicate with the AMT22 encoder from cuidevices.
If you have any questions, please email: chrisnassif@vt.edu 
*/

class AMT22_Encoder {

    public:
        AMT22_Encoder(int chip_select_pin_);
        float get_motor_angle();
        int get_turn_count();
        void zero_encoder_value();

    private:
        uint8_t* read_position();
        uint8_t* read_position_multiturn();
        bool get_bit(uint8_t byte, int index);
        bool verify_packet(uint8_t packet_contents[2]);
        float parse_angle(uint8_t packet_contents[2]);

        float cur_angle;
        float turn_count;
        int chip_select_pin;
};


#endif