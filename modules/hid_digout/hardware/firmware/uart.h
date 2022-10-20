#ifndef _MY_UART_
#define _MY_UART_
#include "pinmacro.h"

#ifndef UART_TX
  #define UART_TX   D,1
#endif

#ifndef UART_RX
  #define UART_RX   D,0
#endif

#ifndef UART_SIZE_PWR
  #define UART_SIZE_PWR 3
#endif

#define UART_SIZE (1<<UART_SIZE_PWR)
#define UART_MASK (UART_SIZE-1)

typedef struct{
  volatile unsigned char st,en;
  volatile char arr[UART_SIZE];
}uart_buffer;

static uart_buffer uart_rx;
static uart_buffer uart_tx;

void uart_config(uint32_t baud, uint8_t stop, uint8_t parity, uint8_t wordsize){
  uint16_t ubrr = (F_CPU/baud - (8 - 4)) / 8;
  UBRRH = 0x0F & (char)(ubrr>>8);
  UBRRL = (char)ubrr;
  uint8_t csrc = (1<<URSEL);
  if(stop == 2)csrc |= (1<<USBS);
  if(parity == 1){
    csrc |= (1<<UPM0) | (1<<UPM1);
  }else if(parity == 2){
    csrc |= (1<<UPM1);
  }
  switch(wordsize){
    case 5:  csrc |= (0<<UCSZ1) | (0<<UCSZ0); break;
    case 6:  csrc |= (0<<UCSZ1) | (1<<UCSZ0); break;
    case 7:  csrc |= (1<<UCSZ1) | (0<<UCSZ0); break;
    default: csrc |= (1<<UCSZ1) | (1<<UCSZ0); break;
  }
  UCSRC = csrc;
}

void uart_init( uint32_t baud ){
  DDR_1( UART_TX );
  DDR_0( UART_RX );
  PORT_1( UART_TX );
  PORT_1( UART_RX );
  
  UCSRA = (1<<U2X);
  UCSRB = (1<<RXEN | 1<<TXEN);
  uart_config(baud, 1, 0, 8);
  //UCSRC = (1<<URSEL | 1<<UCSZ1 | 1<<UCSZ0);
  uart_rx.st=0; uart_rx.en=0; uart_tx.st=0; uart_tx.en=0;
  (void)UDR; //void reading UDR
}

unsigned char uart_size( uart_buffer *buf ){
  return ((buf->en - buf->st) & UART_MASK);
}

char uart_buf_read( uart_buffer *buf ){
  char res;
  if(uart_size(buf) == 0)return 0;
  res = buf->arr[buf->st];
  buf->st++;
  buf->st &= UART_MASK;
  return res;
}

void uart_buf_write( uart_buffer *buf,char dat ){
  if(uart_size(buf) < UART_MASK){
    buf->arr[buf->en]=dat;
    buf->en++; buf->en &= UART_MASK;
  }
}

#define uart_is_empty()	(uart_size(&uart_rx)==0)
#define uart_getc()		uart_buf_read(&uart_rx)
#define uart_free() (UART_MASK - uart_size(&uart_tx))
#define uart_full() (uart_size(&uart_tx)==UART_MASK)

char uart_scan( void ){
  return uart_rx.arr[uart_rx.st];
}

void uart_putc( char ch ){
  uart_buf_write( &uart_tx, ch );
}

void uart_puts( char *str ){
  while(str[0]){
    uart_buf_write( &uart_tx, str[0] );
    str++;
  }
}

void uart_putx( unsigned char x ){
  unsigned char lb = x & 0x0F;
  x >>=4;
  if(x <= 9)uart_putc( x+'0' ); else uart_putc( x+'A'-0x0A);
  if(lb<= 9)uart_putc( lb+'0'); else uart_putc( lb+'A'-0x0A);
}

char uart_poll(){
  if( UCSRA & (1<<UDRE) ){
    if( uart_size( &uart_tx ) != 0 )UDR = uart_buf_read( &uart_tx );
  }
  if( UCSRA & (1<<RXC) ){
    char temp=UDR;
    uart_buf_write( &uart_rx, temp );
    return 1;
  }
  return 0;
}


#endif
