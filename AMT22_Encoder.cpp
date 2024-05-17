#include "AMT22_Encoder.h"

   
// PUBLIC METHODS
AMT22_Encoder::AMT22_Encoder(int chip_select_pin_) {
    chip_select_pin = chip_select_pin_;

    SPI.begin();
    // Serial.begin(9600);
    
    // Arduino UNO has a clock rate of 16 MHz
    // The encoder has a maximum clock rate of 2 MHz
    SPI.beginTransaction(SPISettings(SPISettings(1000000, MSBFIRST, SPI_MODE0))); 
    pinMode(chip_select_pin, OUTPUT);
    digitalWrite(chip_select_pin, HIGH);  // disable
}

float AMT22_Encoder::get_motor_angle() {
    delayMicroseconds(READ_RATE);

    uint8_t* packet_array = read_position();
    // Serial.println("hi");
    // Serial.print(packet_array[0]); Serial.print(", "); Serial.println(packet_array[1]);
    if (verify_packet(packet_array) == 1) {
        // Serial.println("hi");
        cur_angle = parse_angle(packet_array);
    }
    return cur_angle;

}

void AMT22_Encoder::zero_encoder_value() {
    delayMicroseconds(40);

    digitalWrite(chip_select_pin, LOW); // enable


    delayMicroseconds(3);
    SPI.transfer(NO_OP);
    delayMicroseconds(3);
    SPI.transfer(SET_ZERO_POINT);
    delayMicroseconds(3);
    // SPI.transfer(SET_ZERO_POINT);
    // delayMicroseconds(3);
    // SPI.transfer(NO_OP);
    // delayMicroseconds(3);
    // SPI.transfer(SET_ZERO_POINT);
    // delayMicroseconds(3);
    // delay(100000000);
    digitalWrite(chip_select_pin, HIGH); // disable

}



// PRIVATE METHODS
uint8_t* AMT22_Encoder::read_position() {
    delayMicroseconds(40);

    digitalWrite(chip_select_pin, LOW); // enable

    static uint8_t bytes_read [2] = {0x00, 0x00};
    delayMicroseconds(3);
    bytes_read[0] = SPI.transfer(NO_OP);
    delayMicroseconds(3);
    bytes_read[1] = SPI.transfer(NO_OP);
    delayMicroseconds(3);

    digitalWrite(chip_select_pin, HIGH); // disable
    
    return bytes_read;
}

bool AMT22_Encoder::get_bit(uint8_t byte, int index) {
    // checks the bit at the index in the byte in LSB first
    // for example with the byte 10000110: index 0 is 0, index 1 is 1 and so on
    return (byte & 1<<(index)) != 0;
}

bool AMT22_Encoder::verify_packet(uint8_t packet_contents[2]) {
    uint8_t first_byte = packet_contents[0];
    uint8_t second_byte = packet_contents[1];

    bool odd_parity = get_bit(first_byte, 7);
    bool even_parity = get_bit(first_byte, 6);
    
    bool odd_bits [7] = {
        get_bit(first_byte, 1), get_bit(first_byte, 3), get_bit(first_byte, 5), 
        get_bit(second_byte, 1), get_bit(second_byte, 3), get_bit(second_byte, 5), get_bit(second_byte, 7)
    };

    bool even_bits [7] = {
        get_bit(first_byte, 0), get_bit(first_byte, 2), get_bit(first_byte, 4), 
        get_bit(second_byte, 0), get_bit(second_byte, 2), get_bit(second_byte, 4), get_bit(second_byte, 6)
    };

    if (odd_parity == (odd_bits[0] ^ odd_bits[1] ^ odd_bits[2] ^ odd_bits[3] ^ odd_bits[4] ^ odd_bits[5] ^ odd_bits[6])) {
        return false;
    }
    
    if (even_parity == (even_bits[0] ^ even_bits[1] ^ even_bits[2] ^ even_bits[3] ^ even_bits[4] ^ even_bits[5] ^ even_bits[6])) {
        return false;
    } 

    return true;
}

float AMT22_Encoder::parse_angle(uint8_t packet_contents[2]) {
    // we would like to get rid of the error checking bits
    packet_contents[0] = packet_contents[0] & ~0b11000000;    // byte & ~mask is the general solution to completely zeroing out some bits where the mask is 1

    uint16_t angle_raw = packet_contents[0];
    angle_raw <<= 8;
    angle_raw |= packet_contents[1];

    angle_raw >>= 2;  // correct for 12 bit precision
    float angle = ((float)angle_raw * 360) / (float)(pow(2, 12));

    return angle;
}
