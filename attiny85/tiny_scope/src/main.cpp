#include <stdio.h> 
#include <string.h> 
#include <avr/io.h>
#include <util/delay.h>

#include <ssd1306.hpp>

I2C oled_bus{0x3c};
SSD1306 oled{&oled_bus};


static void setup()
{
    oled.clear_screen();
    oled.print(0, 1, ">> AtTiny85 Scope <<");
}

static void loop()
{
    _delay_ms(200);
}

int main(void)
{
    setup();

    while (1) 
        loop();
}

