

#include <osa_buf.h>
#include <string.h>

int OSA_bufDelete(OSA_BufHndl *hndl)
{
	int status=OSA_SOK;

	if(hndl==NULL)
	return OSA_EFAIL;

	status = OSA_queDelete(&hndl->emptyQue);
	status |= OSA_queDelete(&hndl->fullQue);

	return status;
}
int OSA_bufCreate(OSA_BufHndl *hndl, int buf_num)
{
  int status = OSA_SOK;
  int i;

  if(hndl==NULL)
    return OSA_EFAIL;

  if (buf_num > OSA_BUF_NUM_MAX)
    return OSA_EFAIL;

	memset(hndl, 0, sizeof(OSA_BufHndl));

	status = OSA_queCreate(&hndl->emptyQue, buf_num);
	if(status!=OSA_SOK) 
	{
		OSA_ERROR("OSA_bufCreate() = %d \r\n", status);
		return status;
	}

	status = OSA_queCreate(&hndl->fullQue, buf_num);

	if (status!=OSA_SOK) 
	{
		OSA_queDelete(&hndl->emptyQue);
		OSA_ERROR("OSA_bufCreate() = %d \r\n", status);
		return status;
	}

	hndl->numBuf = buf_num;

	for (i = 0; i < hndl->numBuf; i++)
	{
		memset(&(hndl->bufInfo[i]), 0, sizeof(OSA_BufInfo));
		OSA_quePut(&hndl->emptyQue, i, OSA_TIMEOUT_FOREVER);
	}

	return status;
}

int OSA_bufGetEmpty(OSA_BufHndl *hndl, int *bufId, Uint32 timeout)
{
	int status;

	if(hndl==NULL || bufId==NULL)
	return OSA_EFAIL;

	status = OSA_queGet(&hndl->emptyQue, bufId, timeout);

	if (status!=OSA_SOK)
	{
		*bufId = OSA_BUF_ID_INVALID;
	}

	return status;
}

int OSA_bufPutFull (OSA_BufHndl *hndl, int bufId)
{
	int status;

	if(hndl==NULL)
		return OSA_EFAIL;

	if (bufId >= hndl->numBuf || bufId < 0)
		return OSA_EFAIL;

	status = OSA_quePut(&hndl->fullQue, bufId, OSA_TIMEOUT_FOREVER);

	return status;
}

int OSA_bufGetFull(OSA_BufHndl *hndl, int *bufId, Uint32 timeout)
{
	int status;

	if (hndl==NULL || bufId==NULL)
		return OSA_EFAIL;

	status = OSA_queGet(&hndl->fullQue, bufId, timeout);

	if (status!=OSA_SOK)
	{
		*bufId = OSA_BUF_ID_INVALID;
	}

	return status;
}

int OSA_bufPutEmpty(OSA_BufHndl *hndl, int bufId)
{
  	int status;

	if(hndl==NULL)
		return OSA_EFAIL;

 	if (bufId >= hndl->numBuf || bufId < 0)
    	return OSA_EFAIL;

  	status = OSA_quePut(&hndl->emptyQue, bufId, OSA_TIMEOUT_FOREVER);

  	return status;
}


int OSA_bufSwitchFull(OSA_BufHndl *hndl, int *bufId)
{
	int status;
	int newBufId;

	status = OSA_bufGetEmpty(hndl, &newBufId, OSA_TIMEOUT_NONE);

	if (status==OSA_SOK)
	{

		if(*bufId!=OSA_BUF_ID_INVALID)
		{
	  		OSA_bufPutFull(hndl, *bufId);
		}

		*bufId = newBufId;
	}

  	return status;
}

int OSA_bufSwitchEmpty(OSA_BufHndl *hndl, int *bufId)
{
	int status;
	int newBufId;

	status = OSA_bufGetFull(hndl, &newBufId, OSA_TIMEOUT_NONE);

	if (status==OSA_SOK)
	{

		if (*bufId!=OSA_BUF_ID_INVALID)
		{
			OSA_bufPutEmpty(hndl, *bufId);
		}

		*bufId = newBufId;
	}

	return status;
}

OSA_BufInfo *OSA_bufGetBufInfo(OSA_BufHndl *hndl, int bufId)
{
	if (hndl==NULL)
		return NULL;

	if (bufId>=hndl->numBuf)
		return NULL;

	return &hndl->bufInfo[bufId];
}


