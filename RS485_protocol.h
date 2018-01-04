#ifndef Morse_h
#define Morse_h

#include "Arduino.h"
#define RS485Transmit    HIGH
#define RS485Receive     LOW



class RS485
{
  public:
    RS485(uint8_t this_node, int DE);
    void init(unsigned long baud, Stream *_rs485);
    bool Transmit(String _buffer, uint8_t client_id);
    String Receive();
  private:
    void clear_msg();
    bool check_ack(uint8_t client_node);
    void send_ack(uint8_t client_id);
    Stream *rs485;
    int TxControl;
    uint8_t message_packet[255];
    uint8_t node_id;
    uint8_t msg_length;
    bool ack_flag = false;
    int ack = 180;
    uint8_t this_node = 1;
    uint8_t tx_delay = 0;

};

#endif
