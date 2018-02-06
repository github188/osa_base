#include <stdio.h>
#include <stdlib.h>
#include<string.h>
#include "osa_list.h"

typedef struct _HeadNode
{
	void* pData;
	int nSize;
	DataNode* next;
}HeadNode;

typedef DataNode* ptrDataNode;
typedef HeadNode* ptrHeadNode;

List_Handle_t list_create()
{
	ptrHeadNode pHead = (ptrHeadNode)malloc(sizeof(*pHead));
	if(NULL == pHead)
	{
		return NULL;
	}

	memset(pHead, 0 ,sizeof(*pHead));
	return pHead;
}

static int ListIsEmpty(ptrHeadNode pHead)
{
	return pHead->nSize;
}


static ptrDataNode FindNodeTail(ptrHeadNode pHead)

{
	ptrDataNode pNode;
	if(pHead == NULL)
	{
		return NULL;
	}
	pNode = pHead->next;
	for(;pNode->next != NULL; pNode = pNode->next);
	return pNode;

}
static ptrDataNode ListInsert(ptrDataNode prev, void *data)
{
	ptrDataNode pCurNode = (ptrDataNode)malloc(sizeof(*pCurNode));
	if(NULL == pCurNode)
	{
		return NULL;
	}
	pCurNode->pData = data;

	pCurNode->next = prev->next;
	prev->next = pCurNode;
	return pCurNode;
}
List_CurNode_t list_push_front(List_Handle_t pHeadHandle, void* pData)
{
	ptrHeadNode pHead =(ptrHeadNode)pHeadHandle;
	ptrDataNode pCurNode;
	if(pHead == NULL)
	{
		return NULL;
	}
	pCurNode = (ptrDataNode)malloc(sizeof(*pCurNode));
	if(pCurNode == NULL)
	{
		return NULL;
	}
	pCurNode->pData = pData;

	pCurNode->next = pHead->next;
	pHead->next = pCurNode;
	(pHead->nSize)++;
	return pCurNode;
}
List_CurNode_t list_push_back(List_Handle_t pHeadHandle, void* pData)
{
	ptrHeadNode pHead =(ptrHeadNode)pHeadHandle;
	if(pHead == NULL)
	{
		return NULL;
	}
	if(ListIsEmpty(pHead) == 0)
	{
		return list_push_front(pHead,  pData);
	}
	ptrDataNode pdataNode = FindNodeTail(pHead);
	(pHead->nSize)++;
	return ListInsert(pdataNode, pData);
}
List_NodeData_t list_pop_front(List_Handle_t pHeadHandle)
{
	ptrHeadNode pHead =(ptrHeadNode)pHeadHandle;
	void *pdata = NULL;
	if(pHead == NULL)
	{
		return NULL;
	}
	if(ListIsEmpty(pHead) != 0)
	{
		ptrDataNode pdataNode = pHead->next;
		pdata = pdataNode->pData;
		pHead->next = pdataNode->next;

		free(pdataNode);
		pdataNode = NULL;
		(pHead->nSize)--;
	}
	return pdata;
}

List_NodeData_t list_pop_back(List_Handle_t pHeadHandle)
{
	ptrHeadNode pHead =(ptrHeadNode)pHeadHandle;
	void *pdata = NULL;
	if(pHead == NULL)
	{
		return NULL;
	}
	if(ListIsEmpty(pHead) != 0)
	{
		ptrDataNode pdataNode = FindNodeTail(pHead);
		pdata = pdataNode->pData;
		free(pdataNode);
		pdataNode = NULL;
		(pHead->nSize)--;
	}
	return pdata;
}

List_CurNode_t list_begin(List_Handle_t pHeadHandle)
{
	ptrHeadNode pHead =(ptrHeadNode)pHeadHandle;

	if(pHead == NULL)
	{
		return NULL;
	}
	List_CurNode_t pNode= pHead->next;
	return pNode;
}

List_CurNode_t list_next(List_Handle_t pHeadHandle, List_CurNode_t pCurNode)
{
	ptrDataNode pdataNode = pCurNode;
	if(pdataNode == NULL)
	{
		return NULL;
	}
	return pdataNode->next;
}

List_CurNode_t list_end(List_Handle_t pHeadHandle)
{
	return NULL;
}


int list_destory(List_Handle_t pListHandle)
{
	ptrHeadNode pHead =(ptrHeadNode)pListHandle;
	if(pHead == NULL)
	{
		return -1;
	}
	ptrDataNode pNode = pHead->next;
	ptrDataNode pNodeBak;
	while(pNode !=  NULL)
	{
		pNodeBak = pNode;
		pNode = pNode->next;
		free(pNodeBak);//销毁节点
	}
	free(pHead);//销毁头
	return 0;
}

int list_earse_data(List_Handle_t pHeadHandle, void* pData)
{
	ptrHeadNode pHead =(ptrHeadNode)pHeadHandle;
	if(pHead == NULL)
	{
		return -1;
	}
	ptrDataNode pNode = pHead->next;
	ptrDataNode pNodeBak;
	ptrDataNode ptrPrvNode;
	int nCount = 0;
 	while(pNode !=  NULL)
	{
		pNodeBak = pNode;
		//删除特定节点
		if(pNode->pData == pData)
		{
			pNode = pNode->next;
			if(pNodeBak == pHead->next)
			{
				pHead->next = pNode;
			}
			else
			{
				ptrPrvNode->next = pNode;
			}
			printf("list_earse_data nCount:%d\n",nCount);
			free(pNodeBak);//销毁节点
			pHead->nSize--;
			break;
		}
		nCount++;
		ptrPrvNode = pNode;
		pNode = pNode->next;
	}
 	return 0;
}


int list_clear(List_Handle_t pListHandle)
{
	ptrHeadNode pHead =(ptrHeadNode)pListHandle;
	if(pHead == NULL)
	{
		return -1;
	}
	ptrDataNode pNode = pHead->next;
	ptrDataNode pNodeBak;
	while(pNode !=  NULL)
	{
		pNodeBak = pNode;
		pNode = pNode->next;
		free(pNodeBak);//销毁节点
	}
	memset(pHead, 0 ,sizeof(*pHead));
	//ListMakeEmpty(pHead);
	return 0;
}
int list_size(List_Handle_t pHeadHandle)
{
	ptrHeadNode pHead =(ptrHeadNode)pHeadHandle;
	if(pHead == NULL)
	{
		return -1;
	}
	return pHead->nSize;
}


List_LockHandle_t* list_lockAndCreate()
{

	List_LockHandle_t* pList_Handle = malloc(sizeof(*pList_Handle));
	if(pList_Handle == NULL)
	{
		printf("list_lockAndCreate is fai\n");
		return NULL;
	}
	memset(pList_Handle, 0, sizeof(*pList_Handle));
	pList_Handle->pList_Handle = list_create();
	if(pList_Handle->pList_Handle == NULL)
	{
		printf("list_create is fai\n");
		return NULL;
	}
	pthread_mutex_init(&(pList_Handle->lock), NULL);
	return pList_Handle;
}

List_NodeData_t list_lockAndPop_front(List_LockHandle_t* pHeadHandle)
{
	List_NodeData_t pNodeData = NULL;
	if(pHeadHandle == NULL)
	{
		return NULL;
	}
	pthread_mutex_lock(&(pHeadHandle->lock));
	pNodeData = list_pop_front(pHeadHandle->pList_Handle);
	pthread_mutex_unlock(&(pHeadHandle->lock));
	return pNodeData;
}

List_CurNode_t list_lockAndPush_back(List_LockHandle_t* pHeadHandle, void* pData)
{
	List_CurNode_t pNodeData = NULL;
	if(pHeadHandle == NULL)
	{
		return NULL;
	}
	pthread_mutex_lock(&(pHeadHandle->lock));
	pNodeData = list_push_back(pHeadHandle->pList_Handle, pData);
	pthread_mutex_unlock(&(pHeadHandle->lock));
	return pNodeData;
}

int list_lockAndGet_size(List_LockHandle_t* pHeadHandle)
{
	int size = 0;
	if(pHeadHandle == NULL)
	{
		return -1;
	}
	pthread_mutex_lock(&(pHeadHandle->lock));
	size = list_size(pHeadHandle->pList_Handle);
	pthread_mutex_unlock(&(pHeadHandle->lock));
	return size;
}


