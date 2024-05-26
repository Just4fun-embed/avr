
using namespace std;

class I2C
{
    uint8_t     dev_addr;

    void delay();
    int start();
    int restart();
    int stop();
    int send_addr(uint8_t);
    void send_byte(uint8_t);
    uint8_t recv_byte();
    int get_ack();
    void set_ack(uint8_t);

public:
    I2C(uint8_t);

    int write(uint8_t);
    int write(uint8_t, uint8_t);
    int write(uint8_t*, uint8_t);
    int write(uint8_t, uint8_t*, uint8_t);

    int read(uint8_t&, bool ack=true);
    int read(uint8_t, uint8_t&);
    int read(uint8_t, uint8_t*, uint8_t);
};

