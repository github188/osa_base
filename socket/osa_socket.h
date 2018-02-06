#ifndef	__OSA_SOCKET_H__
#define	__OSA_SOCKET_H__


#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <unistd.h>


#include "osa.h"

#define IP_LEN 16
#define MAX_RECV (4096)
#ifdef __cplusplus
extern "C" {
#endif
typedef int(*logbackmsg)(const char *format, ...);

int OSA_createTcpFd(int port, char *local_ip, logbackmsg log_func, int* server_fd);

int OSA_setSndTimeoutFd(int fd , int TimeoutSec, int TimeoutUsec, logbackmsg log_func);
int OSA_tcpSndNonblockFd(int Fd, char *SndBuf, int *SndLen, int Timeout, logbackmsg log_func);
int OSA_tcpRcvBlockFd(int Fd, char *RcvBuf, int RcvLen , int *readlen, logbackmsg log_func);
int OSA_acceptNonblockFd(int Fd, int ServPort, char *ServIp, logbackmsg log_func);
int OSA_setRevTimeoutFd(int fd , int TimeoutSec, int TimeoutUsec, logbackmsg log_func);
int OSA_connetBlockFd(int Fd, int ServPort, char *ServIp, logbackmsg log_func);

#ifdef __cplusplus
}
#endif

#endif
