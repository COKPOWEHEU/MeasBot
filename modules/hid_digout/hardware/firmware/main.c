#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <avr/pgmspace.h>
#include "usbdrv.h"
#include "pinmacro.h"
#include "hardware.h"
#define UART_SIZE_PWR 7
#include "uart.h"

struct data_ports_t{
  uint8_t report_id;
  uint8_t cmd;
};

struct uart_cfg_t{
  uint8_t report_id;
  uint32_t baudrate;  
  uint8_t stopbits; //0=1bit, 2=2bits
  uint8_t parity; //0=none, 1=odd, 2=even
  uint8_t wordsize; //length of data word: 5,6,7,8 or 16 bits
};
struct uart_cfg_t uart_cfg = {.report_id=2, .baudrate=9600, .stopbits=1, .parity=0, .wordsize = 8};

struct uart_data_t{
  uint8_t report_id;
  uint8_t data[4];
};

#define REPID_PORTS	1
#define REPID_UART_CFG	2
#define REPID_UART_0	3
#define UARTS_MAX 7
const uint8_t rep_size[] = {0,
  sizeof(struct data_ports_t), 
  sizeof(struct uart_cfg_t),
  2, 2, 3, 4, 5, 6, 7, 8//uart_data
};
uint8_t buf[UARTS_MAX+2] = {0};
uint8_t report_id = 0;

PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH] = {
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
//PORTS    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x01, //Report ID(1)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, sizeof(struct data_ports_t),//    REPORT_COUNT
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART cfg
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x02, //Report ID(2)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, sizeof(struct uart_cfg_t),//    REPORT_COUNT
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 0 byte
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x03, //Report ID(3)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, 2,    //    REPORT_COUNT               // количество порций
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 1 byte
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x04, //Report ID(4)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, 2,    //    REPORT_COUNT               // количество порций
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 2 byte
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x05, //Report ID(5)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, 3,    //    REPORT_COUNT               // количество порций 
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 3 byte
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x06, //Report ID(6)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, 4,    //    REPORT_COUNT               // количество порций 
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 4 byte
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x07, //Report ID(7)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, 5,    //    REPORT_COUNT               // количество порций 
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 5 byte
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x08, //Report ID(8)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, 6,    //    REPORT_COUNT               // количество порций 
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 6 byte
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 0x09, //Report ID(9)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8) 
      0x95, 7,    //    REPORT_COUNT               // количество порций 
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 7 byte
    0x06, 0x00, 0xff,    // USAGE_PAGE (Generic Desktop)
    0x09, 0x01,          // USAGE (Vendor Usage 1)
    
    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 10, //Report ID(10)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, 8,    //    REPORT_COUNT               // количество порций 
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
//UART 8 byte
/*    0xa1, 0x01,          // COLLECTION (Application)
      0x85, 11, //Report ID(11)
      0x15, 0x00,        //    LOGICAL_MINIMUM (0)
      0x26, 0xff, 0x00,  //    LOGICAL_MAXIMUM (255)
      0x75, 0x08,        //    REPORT_SIZE (8)
      0x95, 9, //    REPORT_COUNT                  // количество порций
      0x09, 0x00,        //    USAGE (Undefined)
      0xb2, 0x02, 0x01,  //    FEATURE (Data,Var,Abs,Buf)
    0xc0,                // END_COLLECTION
    */
};

uint16_t time_ms = 0;
uint16_t time_uled = 0;
uint16_t time_qled = 0;
void timer_init(){
  TCCR1A = (0<<WGM11 | 0<<WGM10); //WGM = 0100
  TCCR1B = (0<<WGM13 | 1<<WGM12) | (0b001<<CS10);
  OCR1A = ((F_CPU/1000) - 1); //1 kHz / 1ms
}
void timer_update(){
  if(! (TIFR & (1<<OCF1A)) )return;
  TIFR |= (1<<OCF1A);
  time_ms++;
  if(time_ms == time_uled)PORT_OFF(ULED);
  if(time_ms == time_qled)PORT_OFF(QLED);
}
void uled_on(){
  PORT_ON(ULED);
  time_uled = time_ms + ULED_TIMEOUT_MS;
}
void qled_on(){
  PORT_ON(QLED);
  time_qled = time_ms + QLED_TIMEOUT_MS;
}

/* Эти переменные хранят статус текущей передачи */
static uchar    currentAddress;
static uchar    bytesRemaining;

/* usbFunctionRead() вызывается когда хост запрашивает порцию данных от устройства
 * Для дополнительной информации см. документацию в usbdrv.h
 */
uchar   usbFunctionRead(uchar *data, uchar len){
  if(len > bytesRemaining)len = bytesRemaining;
  
  if(!currentAddress){        // pdata<=???
    buf[0] = report_id;
    if(report_id == REPID_PORTS){
      buf[1] = 0; //this mode is write-only
    }else if(report_id == REPID_UART_CFG){
      for(uint8_t i=0; i<sizeof(uart_cfg); i++)buf[i] = ((uint8_t*)&uart_cfg)[i];
    }else{
      uint8_t uart_count = report_id - REPID_UART_0;
      for(uint8_t i=0; i<uart_count; i++){
        if(uart_is_empty()){
          buf[0] = i + REPID_UART_0;
          break;
        }
        buf[i+1] = uart_getc();
      }
    }
    if(buf[0] == REPID_UART_0){
      buf[1] = uart_free(); //free space in TX buffer
    }
  }
  uchar j;
  for(j=0; j<len; j++)data[j] = buf[j+currentAddress];
  
  currentAddress += len;
  bytesRemaining -= len;
  return len;
}

/* usbFunctionWrite() вызывается когда хост отправляет порцию данных к устройству
 * Для дополнительной информации см. документацию в usbdrv.h
 */
uchar   usbFunctionWrite(uchar *data, uchar len){
  if(bytesRemaining == 0)return 1;
  
  if(len > bytesRemaining)len = bytesRemaining;
     
  uchar j;
  for(j=0; j<len; j++)buf[j+currentAddress] = data[j];
  
  currentAddress += len;
  bytesRemaining -= len;
  
  if(bytesRemaining == 0){     // ???<=pdata
    if(report_id == REPID_PORTS){
      char let = buf[1] & 0xF0;
      char bits = buf[1];
      if(let == 0xA0){ //VportA
        Vport_out(A, bits);
      }else if(let == 0xB0){ //VportB
        Vport_out(B, bits);
      }else if(let == 0xC0){ //VportC
        Vport_out(C, bits);
      }
      qled_on();
    }else if(report_id == REPID_UART_CFG){
      for(uint8_t i=0; i<sizeof(uart_cfg); i++)((uint8_t*)&uart_cfg)[i] = buf[i];
      uart_config(uart_cfg.baudrate, uart_cfg.stopbits, uart_cfg.parity, uart_cfg.wordsize);
    }else{
      uint8_t uart_count = report_id - REPID_UART_0;
      for(uint8_t i=0; i<uart_count; i++)uart_putc(buf[i+1]);
      uled_on();
    }
  }
  return bytesRemaining == 0; /* 0 означает, что есть еще данные */
}

/* ------------------------------------------------------------------------- */

usbMsgLen_t usbFunctionSetup(uchar data[8]){
 usbRequest_t    *rq = (void *)data;

 if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* HID устройство */
   if(rq->bRequest == USBRQ_HID_GET_REPORT){
     report_id = (rq->wValue.word)&0xFF;
     bytesRemaining = rep_size[report_id];
     currentAddress = 0;
     return USB_NO_MSG;  // используем usbFunctionRead() для отправки данных хосту
   }else if(rq->bRequest == USBRQ_HID_SET_REPORT){
      report_id = (rq->wValue.word)&0xFF;
     bytesRemaining = rep_size[report_id];
     currentAddress = 0;
     return USB_NO_MSG;  // используем usbFunctionWrite() для получения данных от хоста
   }
  }else{
// остальные запросы мы просто игнорируем
  }
  return 0;
}
/* ------------------------------------------------------------------------- */

int main(void){
  cli();
 
  usbInit();
  usbDeviceDisconnect();

  uart_init(9600);
  timer_init();
  DDR_1(ULED); PORT_ON(ULED);
  DDR_1(QLED); PORT_OFF(QLED);
  Vport_init(A);
  Vport_init(B);
  Vport_init(C);

  _delay_ms(1000);
  
  PORT_OFF(ULED);
    
  usbDeviceConnect();
  sei();

  for(;;){
    usbPoll();
    if(uart_poll())uled_on();
    timer_update();
  }
  return 0;
}
