#include "Arduino.h"
#include "RS485_protocol.h"

RS485::RS485(uint8_t _this_node, int DE)
{
  int TxControl = DE;
  pinMode(TxControl, OUTPUT);
  digitalWrite(TxControl, RS485Receive);
  uint8_t this_node = _this_node;
}

void RS485::init(unsigned long baud, Stream *_rs485)
{
  rs485 = _rs485;
  tx_delay = 10000000*(double(2)/double(baud));
}

bool RS485::Transmit(String _buffer, uint8_t client_id) // Transmit to client id from this sender id.
{
  ack_flag = false;

  msg_length = _buffer.length();
  message_packet[0] = client_id;
  message_packet[1] = this_node;
  message_packet[2] = msg_length;

  for (int i = 0; _buffer[i] != '\0'; i++)
  {
    message_packet[i + 3] = _buffer[i];
  }
  digitalWrite(TxControl, RS485Transmit);
  for (int i = 0; i < msg_length + 3; i++)
  {
    rs485->write(message_packet[i]);
  }
  
  digitalWrite(TxControl, RS485Receive);
  clear_msg();
  ack_flag = check_ack(client_id);

  return ack_flag;
}

String RS485::Receive() // Receive if client id is same as this node id.
{
  uint8_t i = 0;
  if (rs485->available())
  {
    for (i = 0; (rs485->available() > 0 && i < 255); i++)
    {
      message_packet[i] = rs485->read();
    }
  }

  if (this_node == message_packet[0])
  {
    String msg = "";
    for (int i = 0; i < message_packet[1]; i++)
    {
      msg += char(message_packet[i + 3]);
    }
    send_ack(message_packet[1]); // send acknowledge to client from this node id.
    clear_msg();
    return msg;
  }
  else {
    clear_msg();
    return "'\0'";
  }
}


void RS485::clear_msg()
{
  for (uint8_t i = 0; i < 255; i++)
  {
    message_packet[i] = '\0';
  }
}

bool RS485::check_ack(uint8_t client_node) // check ack from client node for this node.
{
  unsigned long timeout = millis();
  uint8_t msg[10];
  digitalWrite(TxControl, RS485Receive);

  while (!rs485->available() && (millis() - timeout < 1000));

  if (rs485->available())
  {
    for (int i = 0; (rs485->available() > 0 && i < 10); i++)
    {
      msg[i] = rs485->read();
    }
  }

  if (msg[0] == this_node)
  {
    if (msg[1] == client_node)
    {
      if (msg[2] == 4)
      {
        if (msg[3] == ack)
        {
          return true;
        }
        else
        {
          return false;
        }
      }
      else
      {
        return false;
      }
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
}

void RS485::send_ack(uint8_t client_id)
{
  digitalWrite(TxControl, RS485Transmit);
  rs485->write(client_id);
  rs485->write(this_node);
  rs485->write(4);
  rs485->write(ack);
  while ( !( UCSR3A & (1 << UDRE3)) )
  {}
  digitalWrite(TxControl, RS485Receive);
}

