#ifndef _LIST_H__
#define _LIST_H__
typedef  void* List_Handle_t;//����������
typedef void* List_NodeData_t;//�û�����
typedef void* List_CurNode_t;//����ڵ�
#include <pthread.h>

typedef struct node
{
	void* pData;
	struct node* next;
}DataNode;

typedef struct _List_LockHandle
{
	List_Handle_t pList_Handle;
	pthread_mutex_t lock;
}List_LockHandle_t;


/*@
 *
 */
List_Handle_t list_create();

List_NodeData_t list_pop_back(List_Handle_t pHeadHandle);

List_NodeData_t list_pop_front(List_Handle_t pHeadHandle);

List_CurNode_t list_push_back(List_Handle_t pHeadHandle, void* pData);

List_CurNode_t list_push_front(List_Handle_t pHeadHandle, void* pData);

List_CurNode_t list_begin(List_Handle_t pHeadHandle);

List_CurNode_t list_next(List_Handle_t pHeadHandle, List_CurNode_t pCurNode);

List_CurNode_t list_end(List_Handle_t pHeadHandle);

int list_earse_data(List_Handle_t pHeadHandle, void* pData);

int list_size(List_Handle_t pHeadHandle);

int list_destory(List_Handle_t pList_handle);

int list_clear(List_Handle_t pList_handle);

#endif
