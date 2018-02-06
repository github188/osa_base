#ifndef	__OSA_STREAM_H__
#define	__OSA_STREAM_H__


#include <stdio.h>
#include <stdlib.h>

#include "osa_mutex.h"
#include "osa_list.h"
#include "osa_thr.h"
#include "osa_buf.h"

#define IP_LEN 16
#define MAX_RECV (4096)
#ifdef __cplusplus
extern "C" {
#endif

/*//audio and video frame header */
#define CMD_MSG_HEART (0x800)

typedef enum
{
	MSG_TYPE = 0,
	STREAM_TYPE = 1,
}OSA_ComType_t;

typedef void * Commu_Handle_t;
//typedef void * ServerCom_Handle_t;


typedef struct Net_Msg_Head
{
	char identifier[4];	//@#$&
	int load_len;
	int msg_code;
	int reserve;
} Com_MsgHead_t;


typedef enum
{
	SERVER_STAR = 0,
	SERVER_STOP,
	SERVER_ABOR,
}Server_Staus_t;

#define NETWAITIME 4000
#define MAX_CLIENT 16

typedef struct heartbeat
{
	Uint64 count;
}Heart_Beart_t;

typedef enum Net_Status
{
	CLIENT_DISCONNECT = 0,
	CLIENT_CONNECT = 1,
	CLIENT_ABOR,
}Client_Status_t;


typedef struct _NetCallBackMesssage
{
	void* app_data;
	OSA_ComType_t client_type;
	Com_MsgHead_t msg_head;
	Frame_Head_t stream_head;
	char * recv_value;
	Commu_Handle_t *sOperHandle;
}Commu_CallbackMsg_t;


typedef int(*OSA_deal_func)(Commu_CallbackMsg_t*);
typedef int(*OSA_status_func)(Client_Status_t status, Commu_Handle_t handle, void *inparam);
typedef int(*OSA_log_func)(const char *format, ...);
typedef char *(*OSA_get_emptyBuf)(int size, void *inparam);

#if 1
typedef struct ServerParam
{
	int server_type;
	char server_ip[IP_LEN];
	int server_port;
	int buf_num;
	int max_client;
	OSA_status_func statusFun;
	OSA_deal_func dealFun;
	OSA_log_func logFun;
	OSA_get_emptyBuf emptyBufFun;	
	void *app_data;
}OSA_ServerParam_t;

typedef struct OSA_Server_Handle
{
	OSA_ServerParam_t init_param;
	int server_socket;
	OSA_MutexHndl mutex_handle;	
	OSA_ThrHndl thr_handle;
	List_Handle_t *clientList_Hndl;
	OSA_MutexHndl list_mutex;
	int server_status;
}OSA_ServerHandle_t;

typedef struct Net_Opreate_Hanle
{
	int socket;
	int status;
	char client_ip[IP_LEN];
	OSA_ServerHandle_t *server_handle;
	OSA_ThrHndl cliThr_handle;
	OSA_ThrHndl sendThr_handle;
	OSA_BufHndl clientBuf_Hndl;
	OSA_MutexHndl opreate_lock;
}OSA_CliOpreateHanle_t;


OSA_ServerHandle_t *OSA_server_init(OSA_ServerParam_t server_param);

int OSA_server_sendBuf(OSA_ServerHandle_t *server_handle, OSA_BufInfo bufInfo);

int OSA_server_sendMsg(OSA_CliOpreateHanle_t *pOprHandle, char* message, int nLen, int code);

int OSA_server_printfInfo(OSA_ServerHandle_t *server_handle);

int OSA_server_destory(OSA_ServerHandle_t *server_handle);

#endif
/*******************************************************************************/


typedef struct ClientParam
{
	int recon_flag;
	OSA_ComType_t client_type;
	char server_ip[IP_LEN];
	int server_port;
	OSA_status_func statusFun;
	OSA_deal_func msgFun;
	OSA_log_func logFun;
	OSA_get_emptyBuf get_emptybuf;
	int buf_num;
	void *app_data;
}OSA_ClientParam_t;

typedef struct OSA_Cli_Handle
{
	int client_socket;
	OSA_ClientParam_t init_param;
	OSA_MutexHndl mutex_handle;
	OSA_ThrHndl thr_handle;
	OSA_ThrHndl heartThr_handle;
	int client_status;
}OSA_ClientHandle_t;


OSA_ClientHandle_t *OSA_client_init(OSA_ClientParam_t client_param);

int OSA_client_sendMsg(OSA_ClientHandle_t *handle, char* message, int nLen, int code);

int OSA_client_destory(OSA_ClientHandle_t *client_handle);


#ifdef __cplusplus
}
#endif
#endif	//__RH_STREAM_H__



