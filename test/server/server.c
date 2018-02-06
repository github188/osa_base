#include <stdio.h>
#include <stdarg.h>
#include <time.h>
#include <unistd.h>

#include "osa_communication.h"
#include "osa_file.h"
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


int communtication_server_DealCmd(Commu_CallbackMsg_t* param)
{
	
	if (param->client_type  == MSG_TYPE)
	{
		if (param->msg_head.msg_code == CMD_MSG_HEART)
		{
			Heart_Beart_t* recv = (Heart_Beart_t*)(param->recv_value);
			recv->count++;
			printf("the server recv=%lld\n", recv->count);
			//返回心跳
			OSA_server_sendMsg((OSA_CliOpreateHanle_t *)(param->sOperHandle), (char*)recv, sizeof(Heart_Beart_t), CMD_MSG_HEART);
		}
		else
		{
			printf("the server recv=%s\n", param->recv_value);
			//OSA_server_sendMsg(param->sOperHandle, (char*)recv, sizeof(Heart_Beart_t), CMD_MSG_HEART);
			//返回处理结果
		}
	}
	else
	{
	}
	
	return 0;
}



int communtication_recordclient_netstatus(Client_Status_t status, Commu_Handle_t handle, void *inparam)
{
	printf("********************************************\n");
	return 0;
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


int main(void)
{	
	
	SIGNAL(SIGPIPE, tt);
	//OSA_attachSignalHandler(SIGPIPE, ttt);
	OSA_ServerHandle_t *server_handle = NULL;
	OSA_ServerParam_t server_param;
	memset(&server_param, 0, sizeof(OSA_ServerParam_t));
	server_param.server_type = STREAM_TYPE;
	server_param.logFun = communtication_recordclient_logMsg;
	server_param.statusFun = communtication_recordclient_netstatus;
	server_param.server_port = 19600;
	server_param.max_client = 3;	
	server_param.app_data = NULL;
	server_param.buf_num = 30;
	server_handle =	OSA_server_init(server_param);
	int i = 0;

	OSA_BufInfo bufInfo;
	int max_size = 6221760;
	char *data = (char *)OSA_memAlloc(max_size);
	Uint32  size = 0;
	OSA_fileReadFile("./yy.h264", (Uint8*)data, max_size, &size);
	OSA_printf("end  read....\n");
	bufInfo.data = data;
	bufInfo.head.nDataCodec = H264_CODEC_TYPE;
	bufInfo.head.nFrameLength = size;
	while(1)
	{
		i++;
		usleep(40000);
		OSA_server_sendBuf(server_handle, bufInfo);
	}
	
	OSA_server_printfInfo(server_handle);
	OSA_server_destory(server_handle);

	OSA_memFree(data);
	data = NULL;
	OSA_printf("end server............\n");
	return 0;
}


