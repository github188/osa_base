#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>
#include "osa_file.h"
#include "osa_communication.h"
#include "osa_prf.h"
int communtication_recordclient_logMsg(const char *format, ...)
{
	char buffer[512] = { 0 };
	//CString str;

	va_list args;
	va_start(args, format);
	vsnprintf(buffer,sizeof(buffer), format, args);
	va_end(args);
	printf("%s\n", buffer);
	return 0;

}

static int ooo = 0;

int communtication_recordclient_DealCmd(Commu_CallbackMsg_t* param)
{
	
	if (param->client_type  == MSG_TYPE)
	{
		if (param->msg_head.msg_code == CMD_MSG_HEART)
		{
			Heart_Beart_t* recv = (Heart_Beart_t*)(param->recv_value);
			recv->count++;
			printf("the beart recv=%lld\n", recv->count);
			//返回心跳
			
		}
		else
		{
			printf("the msg recv=%s\n", param->recv_value);
			//返回处理结果
		}
	}
	else
	{
		if (param->stream_head.nDataCodec == H264_CODEC_TYPE)
		{
			//printf("video len=%d\n", param->stream_head.nFrameLength);
			if (param->stream_head.nIframe && ooo == 0)
			{
				OSA_prfFps((OSA_FrameHndl_t *)(param->app_data));
				OSA_fileWriteFile("./yy.h264", (Uint8*)(param->recv_value), param->stream_head.nFrameLength);
				
			}
		}
		else
		{
			//printf("audio len=%d\n", param->stream_head.nFrameLength);
			OSA_fileWriteFile("./yy.aac", (Uint8*)(param->recv_value), param->stream_head.nFrameLength);
		}

		OSA_memFree(param->recv_value);
	}
	
	return 0;
}




int communtication_client_netstatus(Client_Status_t status, Commu_Handle_t handle, void *inparam)
{
	printf("********************************************\n");
	return 0;
}

char * communtication_get_data(int           size,void  *inparam)
{
	char *data = (char *)OSA_memAlloc(size);
	return data;
}


void tt(int id)
{
	printf("fuck.............\n");
}

typedef	void SIGFUNC(int);
SIGFUNC *SIGNAL(int signo, SIGFUNC *func)
{
	SIGFUNC	*sigfunc;
	if((SIG_ERR == (sigfunc = signal(signo, func))))
	{
		printf("ERROR:  signal error \n");
	}
	return (sigfunc);
}
#if 1
int main(int argc , char *argv[])
{	

	SIGNAL(SIGPIPE, tt);
	//OSA_attachSignalHandler(SIGPIPE, ttt);
	OSA_FrameHndl_t *hndl = OSA_memAlloc(sizeof(OSA_FrameHndl_t));
	OSA_ClientHandle_t *client_handle = NULL;
	OSA_ClientParam_t client_param;
	memset(&client_param, 0, sizeof(OSA_ClientParam_t));
	client_param.client_type = STREAM_TYPE;
	client_param.logFun = communtication_recordclient_logMsg;
	client_param.msgFun = communtication_recordclient_DealCmd;
	sprintf(client_param.server_ip,"%s", argv[1]);
	client_param.server_port = atoi(argv[2]);
	client_param.statusFun = communtication_client_netstatus;
	client_param.get_emptybuf = communtication_get_data;
	client_param.app_data = (void *)hndl;
	client_param.recon_flag = 1;
	client_handle =	OSA_client_init(client_param);
	char msg[12] = {0};
	sprintf(msg, "%s", "hehe");
	int i = 0;
	
	if (MSG_TYPE == client_param.client_type)
	{
		while(1)
		{	
			i++;
			sleep(4);
			OSA_client_sendMsg(client_handle, msg, sizeof(msg), 12);
			if (i == 6)
			{
				printf("no send ....................\n");
				sleep(10);
				sleep(12);
				printf("end no send ....................\n");
			}
		}
	}
	else
	{
		while(1)
		{
			i++;
			
			sleep(1);
		}
	}

	OSA_client_destory(client_handle);
	while(1)
	{
		sleep(2);
	}
}
#endif

