#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <inttypes.h>
#include <ctype.h>
#include <stdlib.h>
#include <time.h>
#include "vhid.h"
#include <lua5.2/lua.h>

const unsigned short VID = 0x16C0;
const unsigned short PID = 0x05DF;
const wchar_t MANUFACTURER[]= L"COKPOWEHEU";
const wchar_t PRODUCT[]= L"HID Digout 3x4";

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
}__attribute__((packed));
struct uart_cfg_t uart_cfg = {.report_id=2, .baudrate=9600, .stopbits=1, .parity=0, .wordsize = 8};
struct uart_data_t{
  uint8_t report_id;
  uint8_t data[4];
};
#define REPID_PORTS	1
#define REPID_UART_CFG	2
#define REPID_UART_0	3
#define REPID_UART_TXSIZE 3
#define UARTS_MAX 7

//configure Hid Digout UART
//  baud	baudrate (if =0 then using default 9600)
//  stopbits	0, 1 or 2 (0 = default value, 1)
//  parity	0=none, 1=odd, 2=even
//  wordsize	5, 6, 7 or 8 (default) bits
//return value: -1 on error connecting device, 0 on transmission error, >0 on success
char uart_config(uint32_t baud, uint8_t stopbits, uint8_t parity, uint8_t wordsize){
  hiddevice_t *dev = NULL;
  dev = HidOpen(VID, PID, MANUFACTURER, PRODUCT);
  if(!HidIsConnected(dev)){
    printf("Hid digout: Can not find device %.4X:%.4X [ %ls | %ls ]\n", VID, PID, MANUFACTURER, PRODUCT);
    return -1;
  }
  
  if(baud == 0)baud = 9600;
  if(stopbits == 0)stopbits = 1;
  if(wordsize == 0)wordsize = 8;
  struct uart_cfg_t cfg = {
    .report_id = REPID_UART_CFG,
    .baudrate = baud,
    .stopbits = stopbits,
    .parity = parity,
    .wordsize = wordsize,
  };
  char res = HidWrite(dev, REPID_UART_CFG, (uint8_t*)&cfg, sizeof(cfg)-1);
  HidClose(dev);
  return res;
}

//send (size) bytes to UART. If size is larger than device buffer, function waits until transmit all data
//  data	bytes to send
//  size	number of bytes
//return value: -1 on error connecting device, 0 on transmission error, >0 on success
size_t uart_send(uint8_t *data, size_t size){
  hiddevice_t *dev = NULL;
  dev = HidOpen(VID, PID, MANUFACTURER, PRODUCT);
  if(!HidIsConnected(dev)){
    printf("Hid digout: Can not find device %.4X:%.4X [ %ls | %ls ]\n", VID, PID, MANUFACTURER, PRODUCT);
    return -1;
  }
  
  size_t bsend;
  size_t count = 0;
  char buf[UARTS_MAX+2];
  int res, bufsize;
  res = HidRead(dev, REPID_UART_TXSIZE, buf, 1);
  bufsize = buf[1];
  if(bufsize <= 4)bufsize=4;
  printf("send (free %i) %i\n", buf[1], (int)size);
  while(size > 0){
    if(size > UARTS_MAX)bsend = UARTS_MAX; else bsend = size;
    if(bsend > bufsize)bsend = bufsize;
    buf[0] = bsend + REPID_UART_0;
    for(int i=0; i<bsend; i++)buf[i+1] = data[i];
    res = HidWrite(dev, buf[0], buf, bsend);
    printf("uart_send. %i/%i: ", res, (int)bsend);
    for(int i=0; i<bsend; i++)printf("%.2X ", buf[i]);
    printf("\n");
    if(res > 0)res--;
    count += res;
    data += bsend;
    size -= bsend;
    if(res != bsend)break;
    bufsize -= res;
    printf("send %i %i\n", res, bufsize);
    while(bufsize <= 0){
      res = HidRead(dev, REPID_UART_TXSIZE, buf, 1);
      if(res != 2)return count;
      bufsize = buf[1];
      printf("send (free %i)\n", bufsize);
      usleep(10000);
    }
  }
  HidClose(dev);
  return count;
}

//Read bytes from UART. Read (size) bytes or while device buffer is not empty
//  data	buffer to read
//  size	buffer size
//return value: -1 on error connecting device, 0 on transmission error, >0 on success
size_t uart_receive(uint8_t *data, size_t size){
  hiddevice_t *dev = NULL;
  dev = HidOpen(VID, PID, MANUFACTURER, PRODUCT);
  if(!HidIsConnected(dev)){
    printf("Hid digout: Can not find device %.4X:%.4X [ %ls | %ls ]\n", VID, PID, MANUFACTURER, PRODUCT);
    return -1;
  }
  
  int res;
  char buf[UARTS_MAX+2];
  char repid;
  size_t bsend;
  size_t count = 0;
  while(size > 0){
    if(size > UARTS_MAX)bsend = UARTS_MAX; else bsend = size;
    repid = bsend + REPID_UART_0;
    buf[0] = repid;
    res = HidRead(dev, repid, buf, bsend);
    printf("uart_receive. %i/%i: ", res, (int)bsend);
    for(int i=0; i<bsend; i++)printf("%.2X ", buf[i]);
    printf("\n");
    if(res > 0)res--;
    if(buf[0] != repid){
      uint8_t rd = buf[0] - REPID_UART_0;
      printf(" +-- err. %i %i\n", rd, res);
      if(res > rd)res = rd;
    }
    for(int i=0; i<bsend; i++)data[i] = buf[i+1];
    count += res;
    data += res;
    size -= res;
    if(res != bsend)break;
  }
  HidClose(dev);
  return count;
}

//output 4-bit value to pors 'A', 'B' or 'C'
//  letter	port name, char 'A', 'B' or 'C'
//  bits	value to output
//return value: -1 on error connecting device, 0 on transmission error, >0 on success
//example: port_out('B', 0x3); //sets 0,1 bits, clears 2,3 bits on PortB
int port_out(char letter, uint8_t bits){
  hiddevice_t *dev = NULL;
  dev = HidOpen(VID, PID, MANUFACTURER, PRODUCT);
  if(!HidIsConnected(dev)){
    printf("Hid digout: Can not find device %.4X:%.4X [ %ls | %ls ]\n", VID, PID, MANUFACTURER, PRODUCT);
    return -1;
  }
  
  char buf[2];
  if((letter >= 'A') && (letter <= 'C')){
    letter = (letter - 'A' + 0x0A)<<4;
  }else if((letter >= 'a') && (letter <= 'c')){
    letter = (letter - 'a' + 0x0A)<<4;
  }else return 0;
  buf[0] = REPID_PORTS;
  buf[1] = letter | (bits & 0x0F);
  char res = HidWrite(dev, buf[0], buf, sizeof(buf)-1);
  
  HidClose(dev);
  return res;
}

///////////////////////////////////////////////////////////////////////
//  LUA
///////////////////////////////////////////////////////////////////////
static int L_help(lua_State *L){
  const char helpstring[] = 
  "HID digout module\n"
  "  connectNewDevice():table - connecting to the device\n"
  "  A - variable to write bits to PortA\n"
  "  B - variable to write bits to PortB\n"
  "  C - variable to write bits to PortC\n"
  "  set(P, bits) - set [bits] on port P (bitwise OR)\n"
  "  clr(P, bits) - clear [bits] on port P (nitwise AND NOT_\n"
  "\n"
  "Example:\n"
  "  dev = require(\"hid_digout\"):connectNewDevice()\n"
  "  dev.A = 1   -- write 0b0001 to PortA\n"
  "  dev.B = 0x8 -- write 0b1000 to PortB\n"
  "  dev.C = 0xF -- write 0b1111 to PortC\n"
  "  dev:set('A', 2) -- set bit 1, ignore 0, 2, 3\n"
  "  dev:clr('C', 6) -- clear bits 1, 2 and ignore 0, 3\n"
  ;

  lua_pushstring(L, helpstring);
  return 1;
}

static int L_iterator(lua_State *L){
  int val = lua_tonumber(L, lua_upvalueindex(1));
  if(val == -1){
    int prev = lua_next(L, -2);
    if(prev > 0)return 2;
    val = 0;
    lua_pushnil(L);  //какой-то костыль. Что туда кладется?
  }
  if(val < 3){
    char name[] = "A";
    name[0] = val+'A';
    
    lua_getmetatable(L, 1);
    if(!lua_istable(L, 1)){
      printf("Hid_digout: Not metatable!\n");
      return 0;
    }
    lua_getfield(L, -1, name);
    int value = lua_tonumber(L, -1);
    lua_pop(L, 2);
    
    lua_pushstring(L, name);
    lua_pushnumber(L, value);
    
    val++;
    lua_pushnumber(L, val);
    lua_replace(L, lua_upvalueindex(1));
    return 2;
  }
  return 0;
}

static int L_pairs(lua_State *L){
  lua_pushnumber(L, -1);
  lua_pushcclosure(L, L_iterator, 1);
  lua_pushvalue(L, -2);
  return 2;
}

static void L_letter(lua_State *L, char let[], char dir_set){
  int val = 0;
  
  if(dir_set){
    if(lua_isnumber(L, -1)){
      val = lua_tonumber(L, -1);
    }else{
      printf("Hid digout: not number\n");
    }
  }
  
  lua_getmetatable(L, 1);
    if(!lua_istable(L, 1)){
      printf("Hid_digout: Not metatable!\n");
      return;
    }
    
  if(dir_set){
    lua_pushnumber(L, val);
    lua_setfield(L, -2, let);
    port_out(let[0], val);
    lua_pop(L, 3);
  }else{
    lua_getfield(L, -1, let);
  }
}

static int L_setter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -2))idx = lua_tostring(L, -2); else return 0;
  
  if(idx[1] == 0){
    if(idx[0]=='A' || idx[0]=='a'){
      L_letter(L, "A", 1);
      return 0;
    }else if(idx[0]=='B' || idx[0]=='b'){
      L_letter(L, "B", 1);
      return 0;
    }else if(idx[0]=='C' || idx[0]=='c'){
      L_letter(L, "C", 1);
      return 0;
    }
  }

  lua_pushstring(L, idx);
  lua_pushvalue(L, -2);
  lua_rawset(L, -3-2);

  lua_pop(L, 3);
  return 0;
}

static int L_getter(lua_State *L){
  const char *idx = NULL;
  if(lua_isstring(L, -1))idx = lua_tostring(L, -1); else return 0;
  lua_pop(L, 1);
  
  if(idx[1] == 0){
    if(idx[0]=='A' || idx[0]=='a'){
      L_letter(L, "A", 0);
      return 1;
    }else if(idx[0]=='B' || idx[0]=='b'){
      L_letter(L, "B", 0);
      return 1;
    }else if(idx[0]=='C' || idx[0]=='c'){
      L_letter(L, "C", 0);
      return 1;
    }
  }
  
  lua_pushnil(L);
  return 1;
}

static int L_bit(lua_State *L, char op){
  int bits;
  if(!lua_isnumber(L, -1)){printf("Hid digout set/clr: not number\n"); return 0;}
  bits = lua_tonumber(L, -1);
  const char *idx = NULL;
  if(!lua_isstring(L, -2)){printf("Hid digout set/clr: not port name\n"); return 0;}
  idx = lua_tostring(L, -2);
  if((idx[1] != 0) || ((idx[0]!='A')&&(idx[0]!='a')&&(idx[0]!='B')&&(idx[0]!='b')&&(idx[0]!='C')&&(idx[0]!='c'))){
    printf("Hid digout set/clr: wrong port name\n");
    return 0;
  }
  char let[2] = "A";
  if((idx[0] >= 'a')&&(idx[0]<='c'))let[0] = idx[0]-'a'+'A'; else let[0] = idx[0];
  
  lua_getmetatable(L, 1);
  if(!lua_istable(L, 1)){
    printf("Hid digout clr/set: Not metatable!\n");
    return 0;
  }
  lua_getfield(L, -1, let);
  int value = lua_tonumber(L, -1);
  if(op == 's'){
    value |= bits;
  }else if(op == 'c'){
    value &=~ bits;
  }
  value &= 0x0F;
  lua_pushnumber(L, value);
  lua_setfield(L, -3, let);
  port_out(let[0], value);
  
  lua_pop(L, 4);
  return 0;
}
static int L_set(lua_State *L){
  return L_bit(L, 's');
}

static int L_clr(lua_State *L){
  return L_bit(L, 'c');
}

static int L_connectNewDevice(lua_State *L){
  hiddevice_t *dev = NULL;
  dev = HidOpen(VID, PID, MANUFACTURER, PRODUCT);
  if(!HidIsConnected(dev)){
    printf("Hid digout: Can not find device %.4X:%.4X [ %ls | %ls ]\n", VID, PID, MANUFACTURER, PRODUCT);
    return 0;
  }
  HidClose(dev);
  
  lua_newtable(L);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    //lua_newtable(L);
      //lua_pushcfunction(L, L_tty_config);
      //lua_setfield(L, -2, "config");
      //lua_pushcfunction(L, L_tty_puts);
      //lua_setfield(L, -2, "puts");
      //lua_pushcfunction(L, L_tty_write);
      //lua_setfield(L, -2, "write");
      //lua_pushcfunction(L, L_tty_gets);
      //lua_setfield(L, -2, "gets");
      //lua_pushcfunction(L, L_tty_read);
      //lua_setfield(L, -2, "read");
    //lua_setfield(L, -2, "TTY");
    lua_pushcfunction(L, L_set);
    lua_setfield(L, -2, "set");
    lua_pushcfunction(L, L_clr);
    lua_setfield(L, -2, "clr");
    lua_newtable(L);
      lua_pushnumber(L, 0);
      lua_setfield(L, -2, "A");
      lua_pushnumber(L, 0);
      lua_setfield(L, -2, "B");
      lua_pushnumber(L, 0);
      lua_setfield(L, -2, "C");
      lua_pushcfunction(L, L_setter);
      lua_setfield(L, -2, "__newindex");
      lua_pushcfunction(L, L_getter);
      lua_setfield(L, -2, "__index");
      lua_pushcfunction(L, L_pairs);
      lua_setfield(L, -2, "__pairs");
    lua_setmetatable(L, -2);
  return 1;
}

int luaopen_hid_digout(lua_State *L){
  lua_createtable(L, 0, 0);
    lua_pushcfunction(L, L_help);
    lua_setfield(L, -2, "help");
    
    lua_pushcfunction(L, L_connectNewDevice);
    lua_setfield(L, -2, "connectNewDevice");
  return 1;
}
