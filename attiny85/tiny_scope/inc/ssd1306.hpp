/*
 * ssd1306.h
 *
 */ 


#ifndef SSD1306_H_
#define SSD1306_H_

#include "i2c.hpp"

class SSD1306
{
    I2C*     i2c_bus;
    uint8_t  cmd_buf[8];

    void send_cmd();
    void send_cmd(uint8_t);
    void send_data(uint8_t);
    void send_data(uint8_t*, uint8_t);

    void charge_pump_enable();
    void entire_on(bool on=false);
    void display_on(bool on=true);

public:
    SSD1306(I2C*);

    void clear_screen();
    void set_col_addr(uint8_t start=0, uint8_t end=127);
    void set_page_addr(uint8_t start=0, uint8_t end=7);
    void set_page_start(uint8_t start=0);
    void set_column_start(uint8_t column=0);
    void print(uint8_t, uint8_t, const char*);

    enum addr_mode
    {
        HORIZONTAL,
        VERTICAL,
        PAGE,
    };
    void set_addr_mode(enum addr_mode);

private:
    enum addr_mode mode;
};


#endif /* SSD1306_H_ */
