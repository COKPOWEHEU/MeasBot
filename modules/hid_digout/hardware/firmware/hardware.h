#ifndef __HARDWARE_H__
#define __HARDWARE_H__

#define USB_DP	D,2,1
#define USB_DM	D,3,1

#define ULED	D,4,1 //LED near UART
#define ULED_TIMEOUT_MS	100
#define QLED	D,6,0 //LED near quartz
#define QLED_TIMEOUT_MS	1000

#define UART_TX	D,1,0
#define UART_RX	D,0,0

//virtual port A
#define VA0	B,0,1
#define VA1	B,1,1
#define VA2	B,2,1
#define VA3	B,3,1
//virtual port B
#define VB0	B,4,1
#define VB1	B,5,1
#define VB2	C,0,1
#define VB3	C,1,1
//virtual port C
#define VC0	C,2,1
#define VC1	C,3,1
#define VC2	C,4,1
#define VC3	C,5,1

#include "hardware.h"
#define _Vpn(p, n)	V ## p ## n
#define Vpn(p, n) _Vpn(p, n)

#define Vport_init(port) do{ \
  PORT_OFF(Vpn(port, 0)); DDR_1(Vpn(port, 0)); \
  PORT_OFF(Vpn(port, 1)); DDR_1(Vpn(port, 1)); \
  PORT_OFF(Vpn(port, 2)); DDR_1(Vpn(port, 2)); \
  PORT_OFF(Vpn(port, 3)); DDR_1(Vpn(port, 3)); \
}while(0)

#define Vport_deinit(port) do{ \
  PORT_OFF(Vpn(port, 0)); DDR_0(Vpn(port, 0)); \
  PORT_OFF(Vpn(port, 1)); DDR_0(Vpn(port, 1)); \
  PORT_OFF(Vpn(port, 2)); DDR_0(Vpn(port, 2)); \
  PORT_OFF(Vpn(port, 3)); DDR_0(Vpn(port, 3)); \
}while(0)

#define Vport_out(port, val) do{ \
  if((val) & 1)PORT_ON(Vpn(port, 0)); else PORT_OFF(Vpn(port, 0)); \
  if((val) & 2)PORT_ON(Vpn(port, 1)); else PORT_OFF(Vpn(port, 1)); \
  if((val) & 4)PORT_ON(Vpn(port, 2)); else PORT_OFF(Vpn(port, 2)); \
  if((val) & 8)PORT_ON(Vpn(port, 3)); else PORT_OFF(Vpn(port, 3)); \
}while(0)

#endif