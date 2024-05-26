/*
 * ssd1306.cpp
 *
 */ 


#include <avr/io.h> 
#include <util/delay.h> 

#include <ssd1306.hpp>

constexpr uint8_t SSD1306_WIDTH{128};
constexpr uint8_t SSD1306_HEIGHT{64};

// D/C#
constexpr uint8_t SSD1306_I2C_CMD{0x00};
constexpr uint8_t SSD1306_I2C_DATA{0x40};

/* Fundamental Command */
constexpr uint8_t SSD1306_CMD_ENTIRE_ON{0xA4};
constexpr uint8_t SSD1306_CMD_DISPLAY_ON{0xAE};

/* Addressing Setting Command */
constexpr uint8_t SSD1306_CMD_COL_LO_ADDR{0x00};
constexpr uint8_t SSD1306_CMD_COL_HI_ADDR{0x10};
constexpr uint8_t SSD1306_CMD_ADDR_MODE{0x20};
constexpr uint8_t SSD1306_CMD_COL_ADDR{0x21};
constexpr uint8_t SSD1306_CMD_PAGE_ADDR{0x22};
constexpr uint8_t SSD1306_CMD_PAGE_START_ADDR{0xB0};



constexpr uint8_t SSD1306_CMD_CHARGEPUMP{0x8D};


extern void get_ch_data(char ch, uint8_t *buffer);
constexpr uint8_t CHAR_WIDTH{5};



SSD1306::SSD1306(I2C* bus)
    : i2c_bus{bus}
{
    charge_pump_enable();
    entire_on();
    set_addr_mode(PAGE);
    display_on();
}


void SSD1306::send_cmd()
{
    i2c_bus->write(SSD1306_I2C_CMD, cmd_buf[0]);
}

void SSD1306::send_cmd(uint8_t len)
{
    i2c_bus->write(SSD1306_I2C_CMD, cmd_buf, len);
}

void SSD1306::send_data(uint8_t data)
{
    i2c_bus->write(SSD1306_I2C_DATA, data);
}

void SSD1306::send_data(uint8_t* data, uint8_t len)
{
    for (int i=0; i<len; ++i)
    {
        i2c_bus->write(SSD1306_I2C_DATA, *data++);
    }
}


void SSD1306::charge_pump_enable()
{
    cmd_buf[0] = SSD1306_CMD_CHARGEPUMP;
    cmd_buf[1] = 0x14;
    send_cmd(2);
}

void SSD1306::entire_on(bool on)
{
    cmd_buf[0] = SSD1306_CMD_ENTIRE_ON | on;
    send_cmd();
}

void SSD1306::display_on(bool on)
{
    cmd_buf[0] = SSD1306_CMD_DISPLAY_ON | on;
    send_cmd();
}

void SSD1306::clear_screen()
{
    enum addr_mode cur_mode = mode;

    set_addr_mode(HORIZONTAL);
    set_col_addr();
    set_page_addr();

    for (int i=0; i<SSD1306_HEIGHT; ++i)
        for (int j=0; j<SSD1306_WIDTH; ++j)
            send_data(0);

    set_addr_mode(cur_mode);
}

void SSD1306::set_col_addr(uint8_t start, uint8_t end)
{
    cmd_buf[0] = SSD1306_CMD_COL_ADDR;
    cmd_buf[1] = start;
    cmd_buf[2] = end;
    send_cmd(3);
}

void SSD1306::set_page_addr(uint8_t start, uint8_t end)
{
    cmd_buf[0] = SSD1306_CMD_PAGE_ADDR;
    cmd_buf[1] = start;
    cmd_buf[2] = end;
    send_cmd(3);
}

void SSD1306::set_addr_mode(enum addr_mode mode)
{
    this->mode = mode;

    cmd_buf[0] = SSD1306_CMD_ADDR_MODE;
    cmd_buf[1] = mode;
    send_cmd(2);
}

void SSD1306::set_page_start(uint8_t page)
{
    cmd_buf[0] = SSD1306_CMD_PAGE_START_ADDR | page;
    send_cmd();
}

void SSD1306::set_column_start(uint8_t column)
{
    cmd_buf[0] = SSD1306_CMD_COL_LO_ADDR | (column & 0xF); 
    send_cmd();
    cmd_buf[0] = SSD1306_CMD_COL_HI_ADDR | ((column >> 4) & 0xF);
    send_cmd();
}

void SSD1306::print(uint8_t page, uint8_t col , const char* str)
{
    set_addr_mode(PAGE);
    set_page_start(page);
    set_column_start(col);

    uint8_t ch_buf[8];
    while (*str)
    {
        char ch = *str++;
        get_ch_data(ch, ch_buf);
        send_data(ch_buf, CHAR_WIDTH);
        send_data(0);
    }
}


