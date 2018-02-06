#include <stdio.h>
#include <stdlib.h>

#include "osa.h"
#include "osa_communication.h"
#include "osa_socket.h"

#define NET_CHECHK_PC_IDEENTIFIER_FIRST_BIT	    	'@'
#define NET_CHECHK_PC_IDEENTIFIER_SECOND_BIT	 	'#'
#define NET_CHECHK_PC_IDEENTIFIER_THIRD_BIT 		'$'
#define NET_CHECHK_PC_IDEENTIFIER_FOURTH_BIT		'&'


static int client_check_msgHead(Com_MsgHead_t *head)
{
	if ((head == NULL) || (head->identifier[0] != NET_CHECHK_PC_IDEENTIFIER_FIRST_BIT)
		|| (head->identifier[1] != NET_CHECHK_PC_IDEENTIFIER_SECOND_BIT)
		|| (head->identifier[2] != NET_CHECHK_PC_IDEENTIFIER_THIRD_BIT)
		|| (head->identifier[3] != NET_CHECHK_PC_IDEENTIFIER_FOURTH_BIT))
	{
		return -1;
	}

	if (head->load_len <= 0)
	{
		return -1;
	}

	return 0;
}

static int net_init_msgHead(Com_MsgHead_t *head)
{
	if (head == NULL)
	{
		return -1;
	}

	memset(head, 0, sizeof(Com_MsgHead_t));
	head->identifier[0] = NET_CHECHK_PC_IDEENTIFIER_FIRST_BIT;
	head->identifier[1] = NET_CHECHK_PC_IDEENTIFIER_SECOND_BIT;
	head->identifier[2] = NET_CHECHK_PC_IDEENTIFIER_THIRD_BIT;
	head->identifier[3] = NET_CHECHK_PC_IDEENTIFIER_FOURTH_BIT;

	return 0;
}

static int client_check_streamHead(Frame_Head_t *head)
{
	if ((head == NULL) || (head->check_start[0] != NET_CHECHK_PC_IDEENTIFIER_FIRST_BIT)
		|| (head->check_start[1] != NET_CHECHK_PC_IDEENTIFIER_SECOND_BIT)
		|| (head->check_start[2] != NET_CHECHK_PC_IDEENTIFIER_THIRD_BIT)
		|| (head->check_start[3] != NET_CHECHK_PC_IDEENTIFIER_FOURTH_BIT))
	{
		return -1;
	}

	if ((head == NULL) || (head->check_end[0] != NET_CHECHK_PC_IDEENTIFIER_FIRST_BIT)
		|| (head->check_end[1] != NET_CHECHK_PC_IDEENTIFIER_SECOND_BIT)
		|| (head->check_end[2] != NET_CHECHK_PC_IDEENTIFIER_THIRD_BIT)
		|| (head->check_end[3] != NET_CHECHK_PC_IDEENTIFIER_FOURTH_BIT))
	{
		return -1;
	}
		
	if (head->nFrameLength <= 0)
	{
		return -1;
	}

	return 0;
}

static int net_init_streamHead(Frame_Head_t *head)
{
	if (head == NULL)
	{
		return -1;
	}

	head->check_start[0] = NET_CHECHK_PC_IDEENTIFIER_FIRST_BIT;
	head->check_start[1] = NET_CHECHK_PC_IDEENTIFIER_SECOND_BIT;
	head->check_start[2] = NET_CHECHK_PC_IDEENTIFIER_THIRD_BIT;
	head->check_start[3] = NET_CHECHK_PC_IDEENTIFIER_FOURTH_BIT;

	head->check_end[0] = NET_CHECHK_PC_IDEENTIFIER_FIRST_BIT;
	head->check_end[1] = NET_CHECHK_PC_IDEENTIFIER_SECOND_BIT;
	head->check_end[2] = NET_CHECHK_PC_IDEENTIFIER_THIRD_BIT;
	head->check_end[3] = NET_CHECHK_PC_IDEENTIFIER_FOURTH_BIT;
	return 0;
}



#if 1
static int set_server_status(OSA_ServerHandle_t *handle, Server_Staus_t status)
{
	if (handle == NULL)
	{
		return -1;
	}
	
	OSA_mutexLock(&(handle->mutex_handle));
	handle->server_status = status;
	OSA_mutexUnlock(&(handle->mutex_handle));
	return 0;
}
static Server_Staus_t get_server_status(OSA_ServerHandle_t *handle)
{
	Server_Staus_t status = SERVER_ABOR;
	if (handle == NULL)
	{
		return status;
	}
	
	OSA_mutexLock(&(handle->mutex_handle));
	status = handle->server_status;
	OSA_mutexUnlock(&(handle->mutex_handle));
	
	return status;
}

static int set_client_status(OSA_CliOpreateHanle_t *handle, Client_Status_t status)
{
	if (handle == NULL)
	{
		return -1;
	}
	
	OSA_mutexLock(&(handle->opreate_lock));
	handle->status = status;
	OSA_mutexUnlock(&(handle->opreate_lock));
	return 0;
}

static int get_client_status(OSA_CliOpreateHanle_t *handle)
{
	if (handle == NULL)
	{
		return -1;
	}

	Client_Status_t status = CLIENT_DISCONNECT;
	
	OSA_mutexLock(&(handle->opreate_lock));
	status = handle->status;
	OSA_mutexUnlock(&(handle->opreate_lock));
	return status;
}


static int get_client_socket(OSA_CliOpreateHanle_t *handle)
{
	if (handle == NULL)
	{
		return -1;
	}

	int socket = -1;
	OSA_mutexLock(&(handle->opreate_lock));
	socket = handle->socket;
	OSA_mutexUnlock(&(handle->opreate_lock));
	return socket;
}

static int set_client_socket(OSA_CliOpreateHanle_t *handle, int socket)
{
	if (handle == NULL)
	{
		return -1;
	}
	
	OSA_mutexLock(&(handle->opreate_lock));
	handle->socket = socket;
	OSA_mutexUnlock(&(handle->opreate_lock));
	return 0;
}

int server_fill_buf(OSA_CliOpreateHanle_t *pOprHandle, OSA_BufInfo bufInfo)
{
	int socket = 0;
	OSA_log_func logFun = pOprHandle->server_handle->init_param.logFun;
	
	socket = get_client_socket(pOprHandle);
	if (socket < 3)
	{
		logFun("getclientsocket socket is err!!!\n");
		return -1;
	}

	int ret = -1;
	int bufId = -1;
	ret = OSA_bufGetEmpty(&(pOprHandle->clientBuf_Hndl), &bufId, OSA_TIMEOUT_NONE);
	if (OSA_SOK != ret)
	{
		logFun("11111 OSA_bufGetEmpty is err!!!\n");
		goto EXIT;
	}
	net_init_streamHead(&(bufInfo.head));
	memcpy(&(pOprHandle->clientBuf_Hndl.bufInfo[bufId].head), &(bufInfo.head), sizeof(Frame_Head_t));

	pOprHandle->clientBuf_Hndl.bufInfo[bufId].data = (char *)OSA_memAlloc(bufInfo.head.nFrameLength);
	if (NULL == pOprHandle->clientBuf_Hndl.bufInfo[bufId].data)
	{
		logFun("(char *)malloc(stream_info->head.nFrameLength)is err!!!\n");
		OSA_assert(NULL != pOprHandle->clientBuf_Hndl.bufInfo[bufId].data);
	}
	
	memcpy(pOprHandle->clientBuf_Hndl.bufInfo[bufId].data, bufInfo.data, bufInfo.head.nFrameLength);
	
	ret = OSA_bufPutFull(&(pOprHandle->clientBuf_Hndl), bufId);
	if (OSA_SOK != ret)
	{
		logFun("OSA_bufPutFull is err!!!\n");
	}

	EXIT:
	return 0;
}


int OSA_server_sendBuf(OSA_ServerHandle_t *server_handle, OSA_BufInfo bufInfo)
{
	List_Handle_t pListClient = NULL;
	List_CurNode_t pFindNode = NULL;
	DataNode* pNode = NULL;

	if (NULL == server_handle || NULL == bufInfo.data)
	{
		OSA_printf("OSA_server_sendBuf");
		return OSA_EFAIL;
	}
	
	OSA_mutexLock(&(server_handle->list_mutex));
	pListClient = server_handle->clientList_Hndl;
	if (pListClient != NULL)
	{
		for (pFindNode = list_begin(pListClient); pFindNode != list_end(pListClient); pFindNode = list_next(pListClient, pFindNode))
		{
			pNode = (DataNode*)pFindNode;
			server_fill_buf(pNode->pData, bufInfo);
		}
	}
	OSA_mutexUnlock(&(server_handle->list_mutex));
	return OSA_SOK;
}


int net_send_buf(OSA_CliOpreateHanle_t *pOprHandle, OSA_BufInfo bufInfo, OSA_log_func logFun)
{
	if (NULL == pOprHandle || NULL == bufInfo.data)
	{
		logFun("it is err net_send_msgdeal param!!\n");
		return -1;
	}
	
	int ret = 0;
	int sendlen = 0;
	int socket = 0;

	socket = get_client_socket(pOprHandle);
	if (socket < 3)
	{
		logFun("it is err getclientsocket is err!!\n");
		return -1;
	}

	sendlen = sizeof(bufInfo.head);
	
	ret =  OSA_tcpSndNonblockFd(socket, (char *)(&(bufInfo.head)), &sendlen, 2, logFun);
	if (ret != OSA_SOK)
	{
		logFun( "1 OSA_tcpSndNonblockFd*********\n");
		goto EXIT;
	}

	sendlen = bufInfo.head.nFrameLength;
	ret = OSA_tcpSndNonblockFd(socket, bufInfo.data, &sendlen, 2, logFun);
	if (ret != OSA_SOK)
	{
		logFun( "2 OSA_tcpSndNonblockFd*********\n", ret);
		goto EXIT;
	}

	EXIT:
	return ret;
}

int OSA_server_sendMsg(OSA_CliOpreateHanle_t *pOprHandle, char* message, int nLen, int code)
{
	Com_MsgHead_t head_s;
	int ret = -1;
	int sendlen = 0;
	int socket = 0;

	if (pOprHandle == NULL || message == NULL || nLen <= 0)
	{
		OSA_printf("net_send_msg is err!!!\n");
		return -1;
	}
	
	socket = get_client_socket(pOprHandle);
	if (socket < 3)
	{
		pOprHandle->server_handle->init_param.logFun("it is err get_client_socket....\n");
		return -1;
	}

	net_init_msgHead(&head_s);
	head_s.msg_code = code;
	head_s.load_len = nLen;

	sendlen = sizeof(Com_MsgHead_t);
	ret =  OSA_tcpSndNonblockFd(socket, (char *)(&head_s), &sendlen, 2, pOprHandle->server_handle->init_param.logFun);
	if (ret != OSA_SOK)
	{
		pOprHandle->server_handle->init_param.logFun("1server OSA_tcpSndNonblockFd*********\n");
		goto EXIT;
	}

	sendlen = nLen;
	ret = OSA_tcpSndNonblockFd(socket, message, &sendlen, 2, pOprHandle->server_handle->init_param.logFun);
	if (ret != OSA_SOK)
	{
		pOprHandle->server_handle->init_param.logFun("2server RH_TcpSndNonblockFd********* \n");
		goto EXIT;
	}

	EXIT:
	return ret;
}


void * OSA_processClient_thread(void * argv)
{
	if (NULL == argv)
	{
		OSA_printf("OSA_server_thread param is err!!\n");
		return NULL;
	}

	int ret = 0;
	OSA_CliOpreateHanle_t *pOprHandle = (OSA_CliOpreateHanle_t*)argv;
	OSA_ServerParam_t *init_param = &(pOprHandle->server_handle->init_param);
	init_param->logFun("clientip:%s is connect\n", pOprHandle->client_ip);
	int buf_id = 0;
	
	ret = OSA_mutexCreate(&(pOprHandle->opreate_lock));
	if (OSA_SOK != ret)
	{
		init_param->logFun("OSA_mutexCreate is err!!\n");
		return NULL;
	}

	set_client_status(pOprHandle, CLIENT_CONNECT);

	if (STREAM_TYPE == init_param->server_type)
	{
		OSA_bufCreate(&(pOprHandle->clientBuf_Hndl), init_param->buf_num);
		if (OSA_SOK != ret)
		{
			init_param->logFun("OSA_bufCreate is err!!\n");
			set_client_status(pOprHandle, CLIENT_DISCONNECT);
		}
	}
	
	Com_MsgHead_t msg_head;
	int recv_len = 0;
	int read_len = 0;
	int client_socket = get_client_socket(pOprHandle);
	if (client_socket < 3)
	{
		init_param->logFun("get_client_socket:%d is err!!\n", client_socket);
		set_client_status(pOprHandle, CLIENT_DISCONNECT);
	}
	else
	{
		init_param->logFun("get_client_socket:%d!!\n", client_socket);
	}
	
	OSA_mutexLock(&(pOprHandle->server_handle->list_mutex));
	list_push_back(pOprHandle->server_handle->clientList_Hndl, pOprHandle);
	OSA_mutexUnlock(&(pOprHandle->server_handle->list_mutex));
	int hearttime = NETWAITIME;
	OSA_setRevTimeoutFd(pOprHandle->socket , hearttime/1000, hearttime%1000, init_param->logFun);
	while (CLIENT_CONNECT == get_client_status(pOprHandle) && SERVER_STAR == get_server_status(pOprHandle->server_handle))
	{
		if (STREAM_TYPE == init_param->server_type)
		{
			ret = OSA_bufGetFull(&(pOprHandle->clientBuf_Hndl), &buf_id, OSA_TIMEOUT_FOREVER);
			if (OSA_SOK != ret)
			{
				init_param->logFun("OSA_bufGetFull is err!!\n");
			}
		
			if (buf_id > -1 && buf_id <= init_param->buf_num)
			{
				ret = net_send_buf(pOprHandle, pOprHandle->clientBuf_Hndl.bufInfo[buf_id], init_param->logFun);
				if (ret != 0)
				{
					init_param->logFun("it is err net_send_msgdeal!!\n");
					goto EXIT;
				}
				
				OSA_bufPutEmpty(&(pOprHandle->clientBuf_Hndl), buf_id);
				if (NULL != pOprHandle->clientBuf_Hndl.bufInfo[buf_id].data)
				{
					OSA_memFree(pOprHandle->clientBuf_Hndl.bufInfo[buf_id].data);
					pOprHandle->clientBuf_Hndl.bufInfo[buf_id].data = NULL;
				}
			}
			else
			{
				init_param->logFun("it is err OSA_bufGetFull!!\n");
				usleep(10000);
			}
		}
		else
		{
		
			char *tmp_msg = NULL;
			char msg_l[MAX_RECV] = {0};
			int flag = 0;
			char *head_buf = NULL;
			
			memset(&msg_head, 0, sizeof(Com_MsgHead_t));
			recv_len = sizeof(Com_MsgHead_t);
			head_buf = (char *)&msg_head;
			
			ret =  OSA_tcpRcvBlockFd(client_socket, head_buf, recv_len, &read_len, init_param->logFun);
			if (ret != OSA_SOK)
			{
				init_param->logFun("OSA_tcpRcvBlockFd head is err!!!\n");
				goto EXIT;
			}
			
			if (client_check_msgHead((Com_MsgHead_t*)head_buf) != 0)
			{
				init_param->logFun("communtication_check_head is failed\n");
				goto EXIT;
			}

			recv_len = ((Com_MsgHead_t*)head_buf)->load_len;
			if (recv_len > MAX_RECV)
			{
				tmp_msg = (char *)OSA_memAlloc(recv_len);
				flag = 1;
			}
			else
			{
				tmp_msg = msg_l;
			}
			
			ret = OSA_tcpRcvBlockFd(client_socket, tmp_msg, recv_len, &read_len, init_param->logFun);
			if (ret != OSA_SOK)
			{
				if (NULL != tmp_msg && flag != 0)
				{
					OSA_memFree(tmp_msg);
					tmp_msg = NULL;
				}
				
				init_param->logFun("%s %d OSA_tcpRcvBlockFd msg is failed\n");
				goto EXIT;
				
			}

			//处理函数
			Commu_CallbackMsg_t dealmsg;
			dealmsg.client_type = init_param->server_type;
			if (MSG_TYPE == dealmsg.client_type)
			{
				memcpy((char *)&(dealmsg.msg_head), head_buf, sizeof(Com_MsgHead_t));
			}
			
			dealmsg.sOperHandle = (Commu_Handle_t*)pOprHandle;
			dealmsg.recv_value = tmp_msg;
			dealmsg.app_data = init_param->app_data;
			if (init_param->dealFun(&dealmsg) != 0)
			{
				init_param->logFun("%s %d dealCmd is failed,the msg_code is [%d],\n", msg_head.msg_code);
			}
		
			EXIT:
			if (NULL != tmp_msg && flag != 0)
			{
				OSA_memFree(tmp_msg);
				tmp_msg = NULL;
			}

			if (ret != 0)
			{
				init_param->logFun("OSA_Client is err  client ip =[%s] !!!\n", pOprHandle->client_ip);
				set_client_status(pOprHandle, CLIENT_DISCONNECT);
				shutdown(client_socket, SHUT_RDWR);
				close(client_socket);
				set_client_socket(pOprHandle, -1);
			}
		}
	}

	init_param->logFun("OSA_Client is exit  client ip =[%s] !!!\n", pOprHandle->client_ip);
	OSA_mutexLock(&(pOprHandle->server_handle->list_mutex));
	list_earse_data(pOprHandle->server_handle->clientList_Hndl, pOprHandle);
	OSA_mutexUnlock(&(pOprHandle->server_handle->list_mutex));
	shutdown(client_socket, SHUT_RDWR);
	close(client_socket);	
	OSA_mutexDelete(&(pOprHandle->opreate_lock));
	if (STREAM_TYPE == init_param->server_type)
	{
		while (1)
		{
			ret = OSA_bufGetFull(&(pOprHandle->clientBuf_Hndl), &buf_id, OSA_TIMEOUT_NONE);
			if (OSA_SOK != ret)
			{
				init_param->logFun("OSA_bufGetFull is err!!\n");
				break;
			}
			if (NULL != pOprHandle->clientBuf_Hndl.bufInfo[buf_id].data)
			{
				OSA_memFree(pOprHandle->clientBuf_Hndl.bufInfo[buf_id].data);
				pOprHandle->clientBuf_Hndl.bufInfo[buf_id].data = NULL;
			}
		}
		OSA_bufDelete(&(pOprHandle->clientBuf_Hndl));
	}
	OSA_memFree(pOprHandle);
	pOprHandle = NULL;

	return NULL;
}


int OSA_server_printfInfo(OSA_ServerHandle_t *server_handle)
{
	if (NULL == server_handle)
	{
		OSA_printf("OSA_server_thread param is err!!\n");
		return -1;
	}

	OSA_ServerParam_t *server_param = &(server_handle->init_param);
	OSA_CliOpreateHanle_t *opreateHanle = NULL;
	List_CurNode_t pFindNode = NULL;
	DataNode* pNode = NULL;
	int client_count = 0;

	server_param->logFun("***************server   info****************");
	OSA_mutexLock(&(server_handle->mutex_handle));
	for(pFindNode = list_begin(server_handle->clientList_Hndl); pFindNode != list_end(server_handle->clientList_Hndl); pFindNode = list_next(server_handle->clientList_Hndl, pFindNode))
	{
		client_count++;
		pNode = (DataNode*)pFindNode;
		//destory resouce
		opreateHanle = (OSA_CliOpreateHanle_t *)(pNode->pData);
		server_param->logFun("***************client_ip=[%s]", opreateHanle->client_ip);
	}
	OSA_mutexUnlock(&(server_handle->mutex_handle));
	server_param->logFun("***************client_count=[%d]", client_count);
	server_param->logFun("***************server   info****************");
	return 0;
}

void * OSA_server_thread(void * argv)
{
	if (NULL == argv)
	{
		OSA_printf("OSA_server_thread param is err!!\n");
		return NULL;
	}

	int ret = 0;
	OSA_ServerHandle_t *server_handle = (OSA_ServerHandle_t *)argv;
	OSA_ServerParam_t *server_param = &(server_handle->init_param);
	
	ret = OSA_createTcpFd(server_param->server_port, NULL, server_param->logFun, &(server_handle->server_socket));
	if (OSA_SOK != ret)
	{
		server_param->logFun("OSA_server_thread OSA_createTcpBindFd is err!!!\n");
		return NULL;
	}
	
	char client_ip[16] = {0};
	int client_socket = -1;
	OSA_CliOpreateHanle_t * pOprHandle = NULL;
	
	set_server_status(server_handle, SERVER_STAR);
	server_param->logFun("creat server is sucessful*********\n");
	int client_size = 0;
	while (get_server_status(server_handle) == SERVER_STAR)
	{	
		client_socket = OSA_acceptNonblockFd(server_handle->server_socket, 5, client_ip, server_param->logFun);
		if (client_socket > 2)
		{
			
			OSA_mutexLock(&(server_handle->mutex_handle));
			client_size = list_size(server_handle->clientList_Hndl);
			OSA_mutexUnlock(&(server_handle->mutex_handle));
			
			if ((client_size < server_param->max_client) && (SERVER_STAR == get_server_status(server_handle)))
			{
				pOprHandle =(OSA_CliOpreateHanle_t*)OSA_memAlloc(sizeof(OSA_CliOpreateHanle_t));
				if (pOprHandle == NULL)
				{
					server_param->logFun("%s %d OSA_memAlloc is err!!*********\n");
					sleep(2);
					continue;
				}
			
				struct sockaddr_in flocal;
				int faddr_len = sizeof(struct sockaddr_in);
				getpeername(client_socket,(struct sockaddr*)&flocal, (socklen_t *)&faddr_len);
				memset(pOprHandle, 0 , sizeof(OSA_CliOpreateHanle_t));
				pOprHandle->server_handle = server_handle;
				strncpy(pOprHandle->client_ip, client_ip, sizeof(pOprHandle->client_ip));
				pOprHandle->socket = client_socket;
				ret = OSA_thrCreate(&(server_handle->thr_handle), OSA_processClient_thread, OSA_DETACH, 0, (void *)(pOprHandle));
				if (OSA_SOK != ret)
				{
					server_param->logFun("OSA_processClient_thread OSA_thrCreate is err!!*********\n");
				}
				else
				{
					server_param->logFun("OSA_processClient_thread OSA_thrCreate is sucess!!*********\n");
				}
			}
			else
			{
				server_param->logFun("%s %d server max connect is full*********\n");
			}
		}
		
	}

	set_server_status(server_handle, SERVER_ABOR);
	return NULL;	
}

OSA_ServerHandle_t *OSA_server_init(OSA_ServerParam_t server_param)
{
	if (server_param.logFun == NULL || server_param.statusFun == NULL || server_param.server_port <= 0 ||\
		 server_param.max_client <= 0)
	{
		OSA_printf("OSA_server_init param is err!!\n");
		return NULL;
	}

	int ret = 0;
	
	OSA_ServerHandle_t * server_handle = (OSA_ServerHandle_t*)OSA_memAlloc(sizeof(OSA_ServerHandle_t));
	if (NULL == server_handle)
	{
		server_param.logFun("sdkserver_init_net is malloc fail\n");
		return server_handle;
	}
	
	memset(server_handle, 0 ,sizeof(OSA_ServerHandle_t));
	memcpy(&(server_handle->init_param), &server_param, sizeof(OSA_ServerParam_t));

	ret = OSA_mutexCreate(&(server_handle->mutex_handle));
	if (OSA_SOK != ret)
	{
		server_param.logFun("OSA_mutexCreate is  fail\n");
		OSA_memFree(server_handle);
		server_handle = NULL;
		return server_handle;
	}

	
	ret = OSA_mutexCreate(&(server_handle->list_mutex));
	if (OSA_SOK != ret)
	{
		server_param.logFun("OSA_mutexCreate is  fail\n");
		OSA_mutexDelete(&(server_handle->mutex_handle));
		OSA_memFree(server_handle);
		server_handle = NULL;
		return server_handle;
	}
	
	server_handle->clientList_Hndl = list_create();
	if (NULL == server_handle->clientList_Hndl)
	{
		server_param.logFun("OSA_queCreate is  fail\n");
		OSA_mutexDelete(&(server_handle->mutex_handle));
		OSA_mutexDelete(&(server_handle->list_mutex));
		OSA_memFree(server_handle);
		server_handle = NULL;
		return server_handle;
	}
	
	ret = OSA_thrCreate(&(server_handle->thr_handle), OSA_server_thread, OSA_DETACH, 0, (void *)(server_handle));
	if (0 != ret)
	{
		server_param.logFun("crate OSA_thrCreate thread failed\n");
		OSA_mutexDelete(&(server_handle->mutex_handle));
		OSA_mutexDelete(&(server_handle->list_mutex));
		list_destory(server_handle->clientList_Hndl);
		OSA_memFree(server_handle);
		server_handle = NULL;
		return server_handle;
	}

	sleep(1);
	return server_handle;
}


int OSA_server_destory(OSA_ServerHandle_t *server_handle)
{
	if (NULL == server_handle)
	{
		OSA_printf("OSA_server_destory param is err!!!\n");
		return -1;
	}

	int status = SERVER_STAR;
	
	set_server_status(server_handle, SERVER_STOP);
	int client_count = 0;
	while (1)
	{
		OSA_mutexLock(&(server_handle->list_mutex));
		client_count = list_size(server_handle->clientList_Hndl);
		OSA_mutexUnlock(&(server_handle->list_mutex));
		usleep(100000);
		if (0 == client_count)
		{
			server_handle->init_param.logFun("we end exit server!!!*************************\n");
			break;
		}
	}
	
	while(1)
	{
		status = get_server_status(server_handle);
		if (SERVER_ABOR == status)
		{
			server_handle->init_param.logFun("SERVER_ABOR\n");
			break;
		}
		usleep(100000);
	}
	
	OSA_mutexDelete(&(server_handle->mutex_handle));
	OSA_mutexDelete(&(server_handle->list_mutex));
	if (NULL != server_handle->clientList_Hndl)
	{
		OSA_memFree(server_handle->clientList_Hndl);
		server_handle->clientList_Hndl = NULL;
	}
	OSA_memFree(server_handle);
	server_handle = NULL;
	return 0;
}

#endif
/*******************************************************************************/

static int set_reClient_status(OSA_ClientHandle_t *handle, Client_Status_t status)
{
	if (handle == NULL)
	{
		return -1;
	}
	
	OSA_mutexLock(&(handle->mutex_handle));
	handle->client_status = status;
	OSA_mutexUnlock(&(handle->mutex_handle));
	return 0;
}

static int get_reClient_socket(OSA_ClientHandle_t *handle)
{
	if (handle == NULL)
	{
		return -1;
	}

	int socket = -1;
	OSA_mutexLock(&(handle->mutex_handle));
	socket = handle->client_socket;
	OSA_mutexUnlock(&(handle->mutex_handle));
	return socket;
}

static int set_reClient_socket(OSA_ClientHandle_t *handle, int socket)
{
	if (handle == NULL)
	{
		return -1;
	}
	
	OSA_mutexLock(&(handle->mutex_handle));
	handle->client_socket = socket;
	OSA_mutexUnlock(&(handle->mutex_handle));
	return 0;
}


static Client_Status_t get_reClient_status(OSA_ClientHandle_t *handle)
{
	Client_Status_t status = CLIENT_DISCONNECT;
	if (handle == NULL)
	{
		return status;
	}
	
	OSA_mutexLock(&(handle->mutex_handle));
	status = handle->client_status;
	OSA_mutexUnlock(&(handle->mutex_handle));
	
	return status;
}

int OSA_client_sendMsg(OSA_ClientHandle_t *handle, char* message, int nLen, int code)
{
	Com_MsgHead_t head_s;
	int ret = 0;
	int sendlen = 0;
	int socket = 0;

	if (handle == NULL || message == NULL || nLen <= 0)
	{
		OSA_printf("pNetHandle is NULL, communication_send_msg failed\n");
		return -1;
	}

	socket = get_reClient_socket(handle);
	if (socket < 3)
	{
		return -1;
	}

	if (handle->init_param.client_type == 1)
	{
		
	}

	net_init_msgHead(&head_s);
	head_s.msg_code = code;
	head_s.load_len = nLen;

	//handle->init_param.logFun("head_s.load_len = nLen:%d\n", nLen);
	sendlen = sizeof(Com_MsgHead_t);
	
	ret =  OSA_tcpSndNonblockFd(socket, (char *)(&head_s), &sendlen, 2, handle->init_param.logFun);
	if (ret != OSA_SOK)
	{
		handle->init_param.logFun("1 RH_TcpSndNonblockFd********* \n");
		goto EXIT;
	}

	sendlen = nLen;
	ret = OSA_tcpSndNonblockFd(socket, message, &sendlen, 2, handle->init_param.logFun);
	if (ret != OSA_SOK)
	{
		handle->init_param.logFun("2 RH_TcpSndNonblockFd********* \n");
		goto EXIT;
	}

EXIT:

	return ret;
}


void *OSA_heart_process(void * argv)
{
	sleep(3);//等待连接上服务器
	OSA_ClientHandle_t * client_handle = (OSA_ClientHandle_t *)argv;
	OSA_ClientParam_t init_param;
	memset(&init_param, 0, sizeof(OSA_ClientParam_t));
	memcpy(&init_param, &(client_handle->init_param), sizeof(OSA_ClientParam_t));
	
	if (NULL == init_param.logFun || NULL == init_param.msgFun || strlen(init_param.server_ip) < 0 || \
		NULL == init_param.statusFun || init_param.server_port <= 0)
	{
		printf("OSA_client_init is err server_ip=%s client_param.server_port=%d !!!\n",init_param.server_ip, init_param.server_port);
		return NULL;
	}

	Heart_Beart_t heart_beart;
	memset(&heart_beart, 0 , sizeof(Heart_Beart_t));
	heart_beart.count = 0;
	while(CLIENT_CONNECT == get_reClient_status(client_handle))
	{
		sleep(3);
		heart_beart.count++;
		OSA_client_sendMsg(client_handle, (char*)&(heart_beart), sizeof(Heart_Beart_t), CMD_MSG_HEART);
	}

	return NULL;
}

void *OSA_client_processThr(void * argv)
{
	OSA_ClientHandle_t * client_handle = (OSA_ClientHandle_t *)argv;
	OSA_ClientParam_t init_param;
	memset(&init_param, 0, sizeof(OSA_ClientParam_t));
	memcpy(&init_param, &(client_handle->init_param), sizeof(OSA_ClientParam_t));
	
	if (NULL == init_param.logFun || NULL == init_param.msgFun || strlen(init_param.server_ip) < 0 || \
		NULL == init_param.statusFun || init_param.server_port <= 0)
	{
		OSA_printf("OSA_client_init is err server_ip=%s client_param.server_port=%d !!!\n",init_param.server_ip, init_param.server_port);
		return NULL;
	}
		
	int client_socket = -1;
	int ret = 0;

	if (MSG_TYPE == init_param.client_type)
	{
		ret = OSA_thrCreate(&(client_handle->heartThr_handle), OSA_heart_process, OSA_DETACH, 0, (void *)(client_handle));
		if (0 != ret)
		{
			init_param.logFun("crate OSA_thrCreate OSA_client_process thread failed\n");
			OSA_mutexDelete(&(client_handle->mutex_handle));
			OSA_memFree(client_handle);
			client_handle = NULL;
			return client_handle;
		}
	}
	else
	{
		if (NULL == init_param.get_emptybuf)
		{
			init_param.logFun("crate OSA_thrCreate OSA_client_process thread failed\n");
			OSA_mutexDelete(&(client_handle->mutex_handle));
			OSA_memFree(client_handle);
			client_handle = NULL;
			return client_handle;
		}
	}
REPEAT_CONNECT:
	init_param.logFun("client will connet to server [%s:%d]\n", init_param.server_ip, init_param.server_port);
	
	client_handle->client_socket = -1;
	
	if (client_socket > 0) 
	{
		shutdown(client_socket, SHUT_RDWR);
		close(client_socket);
		client_socket = -1;
	}

	ret = 0;
	client_socket = socket(AF_INET, SOCK_STREAM, 0);
	if (client_socket < 3 && 1 == init_param.recon_flag)
	{
		init_param.logFun("connet server fd is failed, socket is [%d]\n", client_socket);
		sleep(3);
		goto REPEAT_CONNECT;
	}
	
	ret = OSA_connetBlockFd(client_socket, init_param.server_port, init_param.server_ip, init_param.logFun);
	if (ret != OSA_SOK)
	{
		init_param.logFun("connet server fd is failed,the pot is [%d]\n", init_param.server_port);
		sleep(1);
		if (1 == init_param.recon_flag)
		{
			goto REPEAT_CONNECT;
		}
	}
	
	init_param.logFun("connet [%s:%d] is ok\n", init_param.server_ip,  init_param.server_port);
	client_handle->client_socket = client_socket;

	OSA_setRevTimeoutFd(client_socket, 4, 0, init_param.logFun);

	Com_MsgHead_t msg_head;
	Frame_Head_t stream_head;
	int recv_len = 0;
	int read_len = 0;
	
	set_reClient_status(client_handle, CLIENT_CONNECT);
	set_reClient_socket(client_handle, client_socket);
	while (CLIENT_CONNECT == get_reClient_status(client_handle))
	{	
		char *tmp_msg = NULL;
		char msg_l[MAX_RECV] = {0};
		int flag = 0;
		char *head_buf = NULL;
		
		if (MSG_TYPE == init_param.client_type)
		{
			memset(&msg_head, 0, sizeof(Com_MsgHead_t));
			recv_len = sizeof(Com_MsgHead_t);
			head_buf = (char *)&msg_head;
		}
		else
		{
			memset(&stream_head, 0, sizeof(Frame_Head_t));
			recv_len = sizeof(Frame_Head_t);
			head_buf = (char *)&stream_head;
		}
		
		ret =  OSA_tcpRcvBlockFd(client_socket, head_buf, recv_len, &read_len, init_param.logFun);
		if (ret != OSA_SOK)
		{
			init_param.logFun("OSA_tcpRcvBlockFd head is err!!!\n");
			goto EXIT;
		}

		if (MSG_TYPE == init_param.client_type)
		{
			if (client_check_msgHead((Com_MsgHead_t*)head_buf) != 0)
			{
				init_param.logFun("communtication_check_head is failed\n");
				goto EXIT;
			}

			recv_len = ((Com_MsgHead_t*)head_buf)->load_len;
			if (recv_len > MAX_RECV)
			{
				tmp_msg = (char *)OSA_memAlloc(recv_len);
				flag = 1;
			}
			else
			{
				tmp_msg = msg_l;
			}
		}
		else
		{
			if (client_check_streamHead((Frame_Head_t*)head_buf) != 0)
			{
				init_param.logFun("buf communtication_check_head is failed\n");
				goto EXIT;
			}
			
			recv_len = ((Frame_Head_t*)(head_buf))->nFrameLength;
			tmp_msg = init_param.get_emptybuf(recv_len, init_param.app_data);
			if (NULL == tmp_msg)
			{
				init_param.logFun("get_emptybuf is failed\n");
			}
			OSA_assert(tmp_msg != NULL);
			flag = 0;
		}

		ret = OSA_tcpRcvBlockFd(client_socket, tmp_msg, recv_len, &read_len, init_param.logFun);
		if (ret != OSA_SOK)
		{
			if (NULL != tmp_msg && flag != 0)
			{
				OSA_memFree(tmp_msg);
				tmp_msg = NULL;
			}
			
			init_param.logFun("OSA_tcpRcvBlockFd msg is failed\n");
			goto EXIT;
			
		}

		//处理函数
		Commu_CallbackMsg_t dealmsg;
		dealmsg.client_type = init_param.client_type;
		if (MSG_TYPE == dealmsg.client_type)
		{
			memcpy((char *)&(dealmsg.msg_head), head_buf, sizeof(Com_MsgHead_t));
		}
		else
		{
			memcpy((char *)&(dealmsg.stream_head), head_buf, sizeof(Frame_Head_t));
		}
		
		dealmsg.sOperHandle = (Commu_Handle_t*)client_handle;
		dealmsg.recv_value = tmp_msg;
		dealmsg.app_data = init_param.app_data;
		if (init_param.msgFun(&dealmsg) != 0)
		{
			init_param.logFun("dealCmd is failed,the msg_code is [%d],\n", msg_head.msg_code);
		}
		
		EXIT:
		if (NULL != tmp_msg && flag != 0)
		{
			OSA_memFree(tmp_msg);
			tmp_msg = NULL;
		}

		if (ret != 0 && 1 == init_param.recon_flag)
		{
			//set_reClient_status(client_handle, CLIENT_DISCONNECT);
			set_reClient_socket(client_handle, -1);
			
			init_param.logFun("OSA_Client is err  we shoudl reconnect!!!\n");
			sleep(3);
			goto REPEAT_CONNECT;
		}
		
	}
	set_reClient_socket(client_handle, -1);
	shutdown(client_socket, SHUT_RDWR);
	close(client_socket);
	set_reClient_status(client_handle, CLIENT_ABOR);
	return NULL;
}


OSA_ClientHandle_t *OSA_client_init(OSA_ClientParam_t client_param)
{
	if (NULL == client_param.logFun || NULL == client_param.msgFun || strlen(client_param.server_ip) < 0 || \
		NULL == client_param.statusFun || client_param.server_port <= 0)
	{
		printf("OSA_client_init is err server_ip=%s client_param.server_port=%d !!!\n",client_param.server_ip, client_param.server_port);
		return NULL;
	}
		
	int ret = -1;
		
	OSA_ClientHandle_t * client_handle = (OSA_ClientHandle_t *)OSA_memAlloc(sizeof(OSA_ClientHandle_t));
	if (NULL == client_handle)
	{
		client_param.logFun("OSA_client_init  OSA_memAlloc client_handle is err!!!\n");
		return NULL;
	}

	memset(client_handle, 0 ,sizeof(OSA_ClientHandle_t));
	memcpy(&(client_handle->init_param), &client_param, sizeof(OSA_ClientParam_t));
	ret = OSA_mutexCreate(&(client_handle->mutex_handle));
	if (OSA_SOK != ret)
	{
		client_param.logFun("OSA_mutexCreate is  fail\n");
		OSA_memFree(client_handle);
		client_handle = NULL;
		return client_handle;
	}
	
	ret = OSA_thrCreate(&(client_handle->thr_handle), OSA_client_processThr, OSA_DETACH, 0, (void *)(client_handle));
	if (0 != ret)
	{
		client_param.logFun("crate OSA_client_init OSA_thrCreate thread failed\n");
		OSA_mutexDelete(&(client_handle->mutex_handle));
		OSA_memFree(client_handle);
		client_handle = NULL;
		return client_handle;
	}

	return client_handle;
}

int OSA_client_destory(OSA_ClientHandle_t *client_handle)
{
	if (NULL == client_handle)
	{
		printf("OSA_client_destory param is err!!!\n");
		return OSA_EFAIL;
	}
	
	client_handle->init_param.logFun("OSA_client_destory\n");
	set_reClient_status(client_handle, CLIENT_DISCONNECT);
	int client_status = 0;
	while (1)
	{
		client_status = get_reClient_status(client_handle);
		usleep(10000);
		if (CLIENT_ABOR == client_status)
		{
			break;
		}
	}
	OSA_mutexDelete(&(client_handle->mutex_handle));
	OSA_memFree(client_handle);
	client_handle = NULL;
	
	return OSA_SOK;
}


