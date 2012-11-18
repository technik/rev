//----------------------------------------------------------------------------------------------------------------------
// FuVe's Cierva Project.
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on October 18th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Based on arduino code

#ifndef _CIERVA_ARDUINO_HARDWARESERIAL_H_
#define _CIERVA_ARDUINO_HARDWARESERIAL_H_

#include <inttypes.h>

#include <arduino/stream.h>

struct ring_buffer;

class HardwareSerial : public Stream
{
  private:
    ring_buffer *_rx_buffer;
    ring_buffer *_tx_buffer;
    volatile uint8_t *_ubrrh;
    volatile uint8_t *_ubrrl;
    volatile uint8_t *_ucsra;
    volatile uint8_t *_ucsrb;
    volatile uint8_t *_udr;
    uint8_t _rxen;
    uint8_t _txen;
    uint8_t _rxcie;
    uint8_t _udrie;
    uint8_t _u2x;
  public:
    HardwareSerial(ring_buffer *rx_buffer, ring_buffer *tx_buffer,
      volatile uint8_t *ubrrh, volatile uint8_t *ubrrl,
      volatile uint8_t *ucsra, volatile uint8_t *ucsrb,
      volatile uint8_t *udr,
      uint8_t rxen, uint8_t txen, uint8_t rxcie, uint8_t udrie, uint8_t u2x);
    void begin(unsigned long);
    void end();
    virtual int available(void);
    virtual int peek(void);
    virtual int read(void);
    virtual void flush(void);
    virtual size_t write(uint8_t);
    using Print::write; // pull in write(str) and write(buf, size) from Print
    operator bool();
};

#if defined(UBRRH) || defined(UBRR0H)
  extern HardwareSerial Serial; 
#endif
#if defined(UBRR1H)
  extern HardwareSerial Serial1;
#endif
#if defined(UBRR2H)
  extern HardwareSerial Serial2;
#endif
#if defined(UBRR3H)
  extern HardwareSerial Serial3;
#endif

extern void serialEventRun(void) __attribute__((weak));

#endif // _CIERVA_ARDUINO_HARDWARESERIAL_H_