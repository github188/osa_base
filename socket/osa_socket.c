#include "osa_socket.h"



int OSA_createTcpFd(int port, char *local_ip, logbackmsg log_func, int* server_fd)
{
	int fd = -1;

	if (port <= 0) 
	{
		log_func("<OSA_createTcpBindFd IS ERROR>  <FD : %d> <LocalPort :%d >\n", fd, port);
		return OSA_EFAIL;
	}

	struct sockaddr_in LocalAddr;
	bzero(&LocalAddr, sizeof(LocalAddr));

	LocalAddr.sin_family = AF_INET;
	LocalAddr.sin_port = htons(port);
	if (local_ip)
	{
		LocalAddr.sin_addr.s_addr = inet_addr((const char *)local_ip);
	}
	else
	{
		LocalAddr.sin_addr.s_addr	= htonl(INADDR_ANY);
	}

	fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd < 0)
	{
		log_func("<OSA_createTcpBindFd IS ERROR> <socket> <FD : %d> <ERROR_S> <ERROR_D %s,%d>\n", fd, strerror(errno), errno);
		return OSA_EFAIL;
	}
	
	int opt = 1;
	if (setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) 
	{
		log_func("<OSA_createTcpBindFd IS ERROR> <SO_REUSEADDR> <FD : %d> <ERROR_S> <ERROR_D %s,%d>\n", fd, strerror(errno), errno);
		return OSA_EFAIL;
	}

	if (bind(fd, (struct sockaddr *) &LocalAddr, sizeof(LocalAddr)) < 0)
	{
		log_func("<OSA_createTcpBindFd IS ERROR> <BIND> <FD : %d> <ERROR_%s> <ERROR_%d> <LocalPort :%d>\n",
		      fd, strerror(errno), errno, port);
		return OSA_EFAIL;
	}

	if (listen(fd , 30) < 0)
	{
		log_func("listen error:%d,error msg:=%s,port[%u].\n", errno, strerror(errno), port);
	}
	
	*server_fd = fd;
	return OSA_SOK;
}

int OSA_acceptNonblockFd(int fd , int time_out, char *accept_ip, logbackmsg log_func)
{
	if (fd < 3) 
	{
		log_func("<RH_GetConnectNonblockFd IS ERROR>  <FD : %d>\n", fd);
		return OSA_EFAIL;
	}

	int ClnFd = -1;
	int RetSelect = -1;
	int Len = sizeof(struct sockaddr_in);

	fd_set set;

	struct timeval Time;
	Time.tv_sec  = time_out;
	Time.tv_usec = 0;
	FD_ZERO(&set);
	FD_SET(fd, &set);

	struct sockaddr_in ClnAddr;

	RetSelect = select(fd + 1, &set, NULL, NULL, &Time);

	if (RetSelect > 0) 
	{
		ClnFd = accept(fd, (void *)&ClnAddr, (socklen_t *)&Len);

		if (ClnFd < 0)
		{
			//log_func("<RH_GetConnectNonblockFd IS ERROR> <listen> <ERROR_S :%s> <ERROR_D :%d> <FD : %d>\n",
			//      strerror(errno), errno, fd);
			return OSA_EFAIL;
		} 
		else
		{
			log_func("<RH_GetConnectNonblockFd IS OK> <accept> <FD : %d> <ClientAddr :%s>\n", fd, inet_ntoa(ClnAddr.sin_addr));
			memcpy(accept_ip, inet_ntoa(ClnAddr.sin_addr), 16);
			return ClnFd;
		}

	}
	else if(0 == RetSelect) 
	{
		//log_func("<RH_GetConnectNonblockFd IS ERROR> <Connect timeout> <ERROR_S :%s> <ERROR_D :%d> <FD : %d> <Timeout :%d>\n",
		 //     strerror(errno), errno, fd, time_out);
		return OSA_EFAIL;
	} 
	else
	{
		log_func("<RH_GetConnectNonblockFd IS ERROR> <select> <ERROR_S :%s> <ERROR_D :%d> <FD : %d>\n",  strerror(errno), errno, fd);
		return OSA_EFAIL;
	}

	return OSA_SOK;
}

int OSA_setSndTimeoutFd(int fd , int TimeoutSec, int TimeoutUsec, logbackmsg log_func)
{
	if (fd < 0 || TimeoutSec < 0 || TimeoutUsec < 0)
	{
		log_func("<RH_SetSndBufSizeFd IS ERROR>  <FD : %d>   <TimeoutSec :%d> <TimeoutUsec :%d>\n", fd, TimeoutSec, TimeoutUsec);
		return OSA_EFAIL;
	}

	struct timeval Time;

	Time.tv_sec = TimeoutSec;

	Time.tv_usec = TimeoutUsec;

	if (setsockopt(fd, SOL_SOCKET, SO_SNDTIMEO, (char *)&Time, sizeof(struct timeval)) < 0)
	{
		log_func("<OSA_setSndTimeoutFd IS ERROR> <SO_SNDTIMEO> <FD : %d> <ERROR_S :%s> <ERROR_D :%d> <TimeoutSec :%d> <TimeoutUsec :%d>\n",
		      fd, strerror(errno), errno , TimeoutSec, TimeoutUsec);
		return OSA_EFAIL;
	}

	return OSA_SOK;
}


int OSA_tcpSndNonblockFd(int Fd, char *SndBuf, int *SndLen, int Timeout, logbackmsg log_func)
{

	if(Fd < 0 || NULL == SndBuf || SndLen == NULL ) 
	{
		log_func("<sdknet_TcpSndNonblockFd IS ERROR> <FD : %d> <SndBuf :%p> <SndLen :%p> <Timeout :%d>\n",
		      Fd, SndBuf, SndLen, Timeout);
		return OSA_EFAIL;
	}

	if (*SndLen < 0 || *SndLen == 0) 
	{
		log_func("<sdknet_TcpSndNonblockFd IS ERROR> <FD : %d> <SndBuf :%p> <SndLen :%d> <Timeout :%d>\n", Fd, SndBuf, *SndLen, Timeout);
		return OSA_EFAIL;
	}

	int try = 0;
	int RetSelect   = -1;
	int SndTotalLen = 0;
	int SndBytes	= 0;
	int SndTempLen  = *SndLen;
	*SndLen = 0;
	fd_set SndSet;
	struct timeval Time;
	FD_ZERO(&SndSet);
	FD_SET(Fd, &SndSet);

	while(SndTotalLen < SndTempLen) 
	{
		if(Timeout < 0)
		{
			RetSelect = select(Fd + 1, NULL, &SndSet, NULL, NULL);
		}
		else 
		{
			Time.tv_sec = Timeout / 1000;
			Time.tv_usec = 1000 * (Timeout % 1000);
			RetSelect = select(Fd + 1, NULL, &SndSet, NULL, &Time);
		}


		if (RetSelect < 0)
		{
			log_func("<sdknet_TcpSndNonblockFd IS ERROR> <select> <ERROR_S :%s> <ERROR_D :%d> <FD : %d>\n",
			      strerror(errno), errno, Fd);
			return OSA_EFAIL;
		} 
		else if(RetSelect == 0)
		{
			
			if (try++ > 5)
			{
				log_func("<sdknet_TcpSndNonblockFd IS ERROR> <Snd timeout> <ERROR_S :%s> <ERROR_D :%d> <FD : %d> <Timeout :%d> try:%d\n",
								      strerror(errno), errno, Fd, Timeout, try);
					return OSA_EFAIL;
			}
			continue;

		} 
		else
		{
			if(FD_ISSET(Fd, &SndSet))
			{
				SndBytes = send(Fd , SndBuf + SndTotalLen, SndTempLen - SndTotalLen, 0);
				if (SndBytes <= 0) 
				{
					log_func("<sdknet_TcpSndNonblockFd IS ERROR> <Snd > <sendlen :%d> <ERROR_S :%s> <ERROR_D :%d> <FD : %d>\n",
					      SndTempLen - SndTotalLen, strerror(errno), errno, Fd);
					return OSA_EFAIL;
				} 
				else 
				{
					SndTotalLen += SndBytes;
					*SndLen = SndTotalLen;
				}
			}
			else
			{
				log_func("<sdknet_TcpSndNonblockFd IS ERROR> <FD_ISSET> <ERROR_S :%s> <ERROR_D :%d> <FD : %d>\n",
				      strerror(errno), errno, Fd);
				return OSA_EFAIL;
			}
		}
	}

	return 	OSA_SOK;
}


int OSA_tcpRcvBlockFd(int Fd, char *RcvBuf, int RcvLen , int *readlen, logbackmsg log_func)
{
	if (Fd < 0 || NULL == RcvBuf || readlen == NULL)
	{
		log_func( "<OSA_tcpRcvBlockFd IS ERROR> <FD : %d> <RcvBuf :%p> <RcvLen :%p>\n", Fd, RcvBuf, readlen);
		return OSA_EFAIL;
	}

	if (RcvLen < 0 || RcvLen == 0) 
	{
		log_func("<OSA_tcpRcvBlockFd IS ERROR> <FD : %d> <RcvBuf :%p> <RcvLen :%d>\n",	      Fd, RcvBuf, RcvLen);
		return OSA_EFAIL;
	}

	int RcvTotalLen = 0;
	int RcvBytes	= 0;
	int timeout_cnt = 0;
	char *TempBuf = NULL;
	int RcvTempLen  = 0;
	*readlen = 0;

	while (RcvTotalLen < RcvLen) 
	{
		TempBuf = RcvBuf + RcvTotalLen;
		RcvTempLen = RcvLen - RcvTotalLen;
		RcvBytes = recv(Fd , TempBuf, RcvTempLen, 0);
		if (RcvBytes < 0) 
		{
			if (EAGAIN == errno || EWOULDBLOCK == errno || EINTR == errno)
			{
				timeout_cnt++;
				if (timeout_cnt < 3)
				{
					  struct sockaddr_in flocal;
					  int faddr_len = sizeof( struct sockaddr_in);

					  memset(&flocal , 0 ,sizeof(struct sockaddr_in));
					  getpeername(Fd,(struct sockaddr*)&flocal, (socklen_t *)&faddr_len);
					  //log_func("recv failed timeout_cnt=%d errno=%d-<%s> port:%d\n",
					//		timeout_cnt, errno, strerror(errno), ntohs(flocal.sin_port));
					continue;
				}
				else
				{
					 log_func("recv failed timeout_cnt=%d errno=%d-<%s> \n",
							timeout_cnt, errno, strerror(errno));
					//becase it hava beartheart so it can not happend so long time have no msg 
				}
			}

			log_func("<OSA_tcpRcvBlockFd IS ERROR> <RcvAdress : %p> <RcvTempLen :%d> <RcvLen :%d><readlen :%d><Rcv> <ERROR_S :%s> <ERROR_D :%d> <FD : %d>\n",
			     TempBuf,RcvTempLen, RcvLen, *readlen, strerror(errno), errno, Fd);
			return OSA_EFAIL;
		} 
		else if(RcvBytes == 0)
		{
			log_func("ret =%d ---\n", RcvBytes);
			return OSA_EFAIL;
		} 
		else
		{
			RcvTotalLen += RcvBytes;
			*readlen = RcvTotalLen;
		}
	}

	return OSA_SOK;
}

int OSA_connetBlockFd(int Fd, int ServPort, char *ServIp, logbackmsg log_func)
{
	if (Fd < 0 || ServPort < 1 || ServIp == NULL) 
	{
		log_func("<RH_ConnetNonblock IS ERROR>  <FD : %d> <ServPort: %d> <ServIp :%s>\n", Fd, ServPort, ServIp);
		return OSA_EFAIL;
	}

	struct sockaddr_in serv_addr;
	bzero(&serv_addr, sizeof(serv_addr));

	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port	= htons(ServPort);
	inet_aton((const char *)ServIp, (struct in_addr *)&serv_addr.sin_addr);

	if (connect(Fd, (struct sockaddr *)&serv_addr, sizeof(struct sockaddr)) < 0) 
	{
		log_func("<RH_ConnetNonblock IS ERROR> <connect>  <ERROR_S :%s> <ERROR_D :%d> <FD : %d> <ServPort: %d> <ServIp :%s>\n",
		      strerror(errno), errno, Fd, ServPort, ServIp);
		return OSA_EFAIL;
	}

	return OSA_SOK;
}


int OSA_setRevTimeoutFd(int fd , int TimeoutSec, int TimeoutUsec, logbackmsg log_func)
{
	if (fd < 0 || TimeoutSec < 0 || TimeoutUsec < 0)
	{
		log_func("<RH_SetSndBufSizeFd IS ERROR>  <FD : %d>   <TimeoutSec :%d> <TimeoutUsec :%d>\n", fd, TimeoutSec, TimeoutUsec);
		return OSA_EFAIL;
	}

	struct timeval Time;

	Time.tv_sec = TimeoutSec;

	Time.tv_usec = TimeoutUsec;

	if (setsockopt(fd, SOL_SOCKET, SO_RCVTIMEO, (char *)&Time, sizeof(struct timeval)) < 0)
	{
		log_func("<OSA_setRevTimeoutFd IS ERROR> <SO_SNDTIMEO> <FD : %d> <ERROR_S :%s> <ERROR_D :%d> <TimeoutSec :%d> <TimeoutUsec :%d>\n",
		      fd, strerror(errno), errno , TimeoutSec, TimeoutUsec);
		return OSA_EFAIL;
	}

	return OSA_SOK;
}

