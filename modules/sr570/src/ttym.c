#include "ttym.h"

#if defined(linux) || defined(__linux) || defined(__linux__) || defined(__GNU__) || defined(__GLIBC__)
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <termios.h>
#include <stdlib.h>

struct ttym{
  int fd;
  struct termios ttyset, oldset;
};

//знаю что выглядит костыльно, но я не могу гарантировать, что B9600 == 9600
speed_t speed_convert(unsigned int baudrate){
  switch(baudrate){
    case 0: return B0;
    case 50: return B50;
    case 75: return B75;
    case 110: return B110;
    case 134: return B134;
    case 150: return B150;
    case 200: return B200;
    case 300: return B300;
    case 600: return B600;
    case 1200: return B1200;
    case 1800: return B1800;
    case 2400: return B2400;
    case 4800: return B4800;
    case 9600: return B9600;
    case 19200: return B19200;
    case 38400: return B38400;
    case 57600: return B57600;
    case 115200: return B115200;
    case 230400: return B230400;
#ifdef B460800
    case 460800: return B460800;
#endif
#ifdef B500000
    case 500000: return B500000;
#endif
#ifdef B576000
    case 576000: return B576000;
#endif
#ifdef B921600
    case 921600: return B921600;
#endif
#ifdef B1000000
    case 1000000: return B1000000;
#endif
#ifdef B1152000
    case 1152000: return B1152000;
#endif
#ifdef B1500000
    case 1500000: return B1500000;
#endif
#ifdef B2000000
    case 2000000: return B2000000;
#endif
#ifdef B2500000
    case 2500000: return B2500000;
#endif
#ifdef B3000000
    case 3000000: return B3000000;
#endif
#ifdef B3500000
    case 3500000: return B3500000;
#endif
#ifdef B4000000
    case 4000000: return B4000000;
#endif
    default: return B0;
  }
}

ttym_t ttym_open(char name[], unsigned int baudrate){
  speed_t speed = speed_convert(baudrate);
  ttym_t res = (struct ttym*)malloc(sizeof(struct ttym));
  if(res == NULL)return res;
  res->fd = open(name, O_RDWR | O_NOCTTY);
  if(res->fd == -1){
    free(res);
    return NULL;
  }
  tcgetattr( res->fd, &(res->ttyset) );
  memcpy( &(res->oldset), &(res->ttyset), sizeof(struct termios));
  res->ttyset.c_cflag = speed | CLOCAL | CREAD | CS8 | CSTOPB;
  res->ttyset.c_iflag = IGNPAR;
  res->ttyset.c_oflag = 0;
  res->ttyset.c_lflag = 0;
  res->ttyset.c_cc[VMIN] = 0;
  res->ttyset.c_cc[VTIME]= 0;
  tcflush(res->fd, TCIFLUSH);
  tcsetattr(res->fd, TCSANOW, &(res->ttyset) );
  return res;
}

int ttym_close(ttym_t tty){
  if(tty == NULL)return -1;
  tcsetattr( tty->fd, TCSANOW, &(tty->oldset) );
  close( tty->fd );
  free(tty);
  return 0;
}

ssize_t ttym_write(ttym_t tty, void *buf, size_t count){
  if(tty == NULL)return -1;
  return write( tty->fd, buf, count );
}

ssize_t ttym_read(ttym_t tty, void *buf, size_t count){
  if(tty == NULL)return -1;
  return read( tty->fd, buf, count );
}

int ttym_timeout(ttym_t tty, ssize_t time_ms){
  if(tty == NULL)return -1;
  tty->ttyset.c_cc[VTIME]= time_ms/100;
  tcsetattr( tty->fd, TCSANOW, &(tty->ttyset) );
  return 0;
}

#elif defined(_WIN32) || defined(__WIN32__) || defined(WIN32)
#include <windows.h>

struct ttym{
  HANDLE handle;
};

ttym_t ttym_open(char name[], unsigned int baudrate){
  ttym_t res = (struct ttym*)malloc(sizeof(struct ttym));
  if(res == NULL)return res;
  res->handle = CreateFileA(name,
                GENERIC_READ | GENERIC_WRITE,
                0,
                NULL,
                OPEN_EXISTING, 
                FILE_ATTRIBUTE_NORMAL,
                NULL);
  if(res->handle == (HANDLE)-1){
    free(res);
    return NULL;
  }
  
  SetCommMask(res->handle, EV_RXCHAR); //какие сигналы отслеживать (только RX)
  SetupComm(res->handle, 1000, 1000); //размер буферов на прием и передачу

  COMMTIMEOUTS timeout;
  timeout.ReadIntervalTimeout = 0;
  timeout.ReadTotalTimeoutMultiplier = 1;
  timeout.ReadTotalTimeoutConstant = 0;
  timeout.WriteTotalTimeoutMultiplier = 1;
  timeout.WriteTotalTimeoutConstant = 0;

  if( !SetCommTimeouts(res->handle, &timeout) ){
    CloseHandle(res->handle);
    free(res);
    return NULL;
  }

  DCB dcb;

  memset(&dcb,0,sizeof(DCB));
  dcb.DCBlength = sizeof(DCB);
  GetCommState(res->handle, &dcb);
  
  dcb.BaudRate = (DWORD)baudrate;
  dcb.ByteSize = 8;
  dcb.Parity = NOPARITY;
  dcb.StopBits = ONESTOPBIT;
  dcb.fAbortOnError = FALSE;
  dcb.fDtrControl = DTR_CONTROL_DISABLE;
  dcb.fRtsControl = RTS_CONTROL_DISABLE;
  dcb.fBinary = TRUE;
  dcb.fParity = FALSE;
  dcb.fInX = FALSE;
  dcb.fOutX = FALSE;
  dcb.XonChar = 0;
  dcb.XoffChar = (unsigned char)0xFF;
  dcb.fErrorChar = FALSE;
  dcb.fNull = FALSE;
  dcb.fOutxCtsFlow = FALSE;
  dcb.fOutxDsrFlow = FALSE;
  dcb.XonLim = 128;
  dcb.XoffLim = 128;

  if( !SetCommState(res->handle, &dcb) ){
    CloseHandle(res->handle);
    free(res);
    return NULL;
  }
  return res;
}

int ttym_close(ttym_t tty){
  CloseHandle(tty->handle);
  free(tty);
  return 0;
}

ssize_t ttym_write(ttym_t tty, void *buf, size_t count){
  DWORD cnt;
  if( !WriteFile(tty->handle, buf, (DWORD)count, &cnt, NULL))return -1;
  return cnt;
}

ssize_t ttym_read(ttym_t tty, void *buf, size_t count){
  DWORD cnt;
  if( !ReadFile(tty->handle, buf, (DWORD)count, &cnt, NULL))return -1;
  return cnt;
}

int ttym_timeout(ttym_t tty, ssize_t time_ms){
  COMMTIMEOUTS timeout;
  GetCommTimeouts(tty->handle, &timeout);
  timeout.ReadIntervalTimeout = time_ms;
  timeout.ReadTotalTimeoutMultiplier = 1;
  timeout.ReadTotalTimeoutConstant = time_ms;
  timeout.WriteTotalTimeoutMultiplier = 1;
  timeout.WriteTotalTimeoutConstant = time_ms;
  if( !SetCommTimeouts(tty->handle, &timeout) ){
    return -1;
  }
  return 0;
}

#else
  #error "Unsupported platform"
#endif

// COMMON

ssize_t ttym_writechar(ttym_t tty, char data){
  return ttym_write(tty, &data, 1);
}

int ttym_readchar(ttym_t tty){
  char data;
  if( ttym_read(tty, &data, 1) != 1 )return -1;
  return data;
}
