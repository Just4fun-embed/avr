
#include <avr/io.h> 
#include <util/delay.h> 

#include <i2c.hpp> 

#define  USI_PORT  PORTB
#define  USI_PIN   PINB
#define  USI_DDR   DDRB
constexpr uint8_t SCL{0x04};
constexpr uint8_t SDA{0x01};


// USICR Register
constexpr uint8_t SIE  {0x80};
constexpr uint8_t OIE  {0x40};
constexpr uint8_t WM1  {0x20};
constexpr uint8_t WM0  {0x10};
constexpr uint8_t CS1  {0x08};
constexpr uint8_t CS0  {0x04};
constexpr uint8_t CLK  {0x02};
constexpr uint8_t TC   {0x01};

// USISR Register
constexpr uint8_t SIF  {0x80};
constexpr uint8_t OIF  {0x40};
constexpr uint8_t PF   {0x20};
constexpr uint8_t DC   {0x10};
constexpr uint8_t CNT3 {0x08};
constexpr uint8_t CNT2 {0x04};
constexpr uint8_t CNT1 {0x02};
constexpr uint8_t CNT0 {0x01};

#define TWI_RD      0x01
#define TWI_WR      0x00

I2C::I2C(uint8_t dev)
    : dev_addr{dev}
{
    USI_PORT |= (SCL | SDA);
    USI_DDR  |= (SCL | SDA);

    USIDR = 0xFF;
    USICR = WM1 | CS1 | CLK;
    USISR = SIF | OIF | PF | DC;
}

int I2C::write(uint8_t data)
{
    send_byte(data);
    if (get_ack())
        return -1;

    return 0;
}

int I2C::write(uint8_t reg, uint8_t data)
{
    if (start())
        return -1;

    if (write((dev_addr << 1) | TWI_WR))
        return -1;

    if (write(reg))
        return -1;

    if (write(data))
        return -1;

    if (stop())
        return -1;

    return 0;
}

int I2C::write(uint8_t* data, uint8_t len)
{
    if (start())
        return -1;

    if (write((dev_addr << 1) | TWI_WR))
        return -1;

    for (int i=0; i<len; ++i)
    {
        if (write(*data++))
            return -1;
    }

    if (stop())
        return -1;

    return 0;
}

int I2C::write(uint8_t reg, uint8_t* data, uint8_t len)
{
    if (start())
        return -1;

    if (write((dev_addr << 1) | TWI_WR))
        return -1;

    if (write(reg))
        return -1;

    for (int i=0; i<len; ++i)
    {
        if (write(*data++))
            return -1;
    }

    if (stop())
        return -1;

    return 0;
}

int I2C::read(uint8_t& data, bool ack)
{
    data = recv_byte();
    if (ack)
        set_ack(0);
    else
        set_ack(0xFF);

    return 0;
}

int I2C::read(uint8_t reg, uint8_t& data)
{
    if (start())
        return -1;

    if (write((dev_addr << 1) | TWI_WR))
        return -1;

    if (write(reg))
        return -1;

    if (restart())
        return -1;

    if (write((dev_addr << 1) | TWI_RD))
        return -1;

    if (read(data, false))
        return -1;

    if (stop())
        return -1;

    return 0;
}

int I2C::read(uint8_t reg, uint8_t* data, uint8_t len)
{
    if (start())
        return -1;

    if (write((dev_addr << 1) | TWI_WR))
        return -1;

    if (write(reg))
        return -1;

    if (restart())
        return -1;

    if (write((dev_addr << 1) | TWI_RD))
        return -1;

    uint8_t rd{};
    for (int i=0; i<len-1; ++i)
    {
        if (read(rd))
            return -1;

        *data++ = rd;
    }
    if (read(rd, false))
        return -1;
    *data = rd;

    if (stop())
        return -1;

    return 0;
}

void I2C::delay()
{
    _delay_us(2);
}

int I2C::start()
{
    USI_PORT |= SCL;
    while (!(USI_PIN & SCL));
    delay();

    USI_PORT &= ~SDA;
    delay();
    USI_PORT &= ~SCL;
    USI_PORT |= SDA;
    delay();

    if (!(USISR & SIF))
        return -1;

    return 0;
}

int I2C::restart()
{
    USI_PORT |= SDA;
    delay();
    USI_PORT |= SCL;
    while (!(USI_PIN & SCL));
    delay();

    USI_PORT &= ~SDA;
    delay();
    USI_PORT &= ~SCL;
    USI_PORT |= SDA;
    delay();

    return 0;
}

int I2C::stop()
{
    USI_PORT &= ~SDA;
    USI_PORT |= SCL;
    while (!(USI_PIN & SCL));

    delay();
    USI_PORT |= SDA;
    delay();

    if (!(USISR & PF))
        return -1;

    return 0;
}

void I2C::send_byte(uint8_t data)
{
    USI_PORT &= ~SCL;
    USIDR = data;

    USISR = SIF | OIF | PF | DC;
    do
    {
        delay();
        USICR = WM1 | CS1 | CLK | TC;
        while (!(USI_PIN & SCL));
        delay();
        USICR = WM1 | CS1 | CLK | TC;
    } while(!(USISR & OIF));

    delay();
    USIDR = 0xFF;
    USI_DDR |= SDA;
}

uint8_t I2C::recv_byte()
{
    USI_DDR &= ~SDA;
    USISR = SIF | OIF | PF | DC;
    do
    {
        delay();
        USICR = WM1 | CS1 | CLK | TC;
        while (!(USI_PIN & SCL));
        delay();
        USICR = WM1 | CS1 | CLK | TC;
    } while(!(USISR & OIF));

    delay();
    uint8_t data = USIDR;
    USIDR = 0xFF;
    USI_DDR |= SDA;

    return data;
}

int I2C::get_ack()
{
    USI_DDR &= ~SDA;
    USISR = SIF | OIF | PF | DC | 0x0E;
    do
    {
        delay();
        USICR = WM1 | CS1 | CLK | TC;
        while (!(USI_PIN & SCL));
        delay();
        USICR = WM1 | CS1 | CLK | TC;
    } while(!(USISR & OIF));

    delay();
    uint8_t data = USIDR;
    USIDR = 0xFF;
    USI_DDR |= SDA;

    return data & 1;
}

void I2C::set_ack(uint8_t data)
{
    USI_PORT &= ~SCL;
    USIDR = data;

    USISR = SIF | OIF | PF | DC | 0x0E;
    do
    {
        delay();
        USICR = WM1 | CS1 | CLK | TC;
        while (!(USI_PIN & SCL));
        delay();
        USICR = WM1 | CS1 | CLK | TC;
    } while(!(USISR & OIF));

    delay();
    USIDR = 0xFF;
    USI_DDR |= SDA;
}

