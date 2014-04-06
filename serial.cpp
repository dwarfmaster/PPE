
#include "serial.hpp"
#include "rs232.hpp"

Serial::Serial()
    : m_opened(false)
{}

bool Serial::open()
{
    if(RS232_OpenComport(16, 9600) == 1)
        m_opened = false;
    else
        m_opened = true;
    return m_opened;
}

bool Serial::opened() const
{
    return m_opened;
}

int Serial::read(char* buf, int size)
{
    int ret = RS232_PollComport(16, (unsigned char*)buf, size);
    return ret;
}

void Serial::write(char byte)
{
    unsigned char b = byte & 0x7F; /* 01111111 mask */
    RS232_SendByte(16, b);
}

