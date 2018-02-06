

#include <osa_prf.h>

#ifdef OSA_PRF_ENABLE

int OSA_prfFps(OSA_FrameHndl_t * hndl)
{
	if (NULL == hndl)
	{
		OSA_printf("it is err OSA_prfFps param=NULL\n");
		return OSA_EFAIL;
	}
	
	if (hndl->flag != 1)
	{
		hndl->flag = 1;
		hndl->pre_time = OSA_getCurTimeInMsec();
	}
	
	hndl->count++;
	hndl->cur_time = OSA_getCurTimeInMsec();

	if (hndl->cur_time - hndl->pre_time >= 3000)
	{
		hndl->pre_time = hndl->cur_time;
		OSA_printf("%s=%d\n",hndl->msg, hndl->count);
		hndl->count = 0;
	}
	
	return OSA_SOK;
}


int OSA_prfBegin(OSA_PrfHndl *hndl)
{
	hndl->curTime = OSA_getCurTimeInMsec();
	return OSA_SOK;
}

int OSA_prfEnd(OSA_PrfHndl *hndl, Uint32 value)
{
	hndl->curTime = OSA_getCurTimeInMsec() - hndl->curTime;

	hndl->count++;

	hndl->totalTime += hndl->curTime;
		if(hndl->curTime > hndl->maxTime)
	hndl->maxTime = hndl->curTime;
		if(hndl->curTime < hndl->minTime)
	hndl->minTime = hndl->curTime;

	hndl->totalValue += value;
		if (value > hndl->maxValue)
	hndl->maxValue = value;
		if (value < hndl->minTime)
	hndl->minValue = value;

	return OSA_SOK;
}

int OSA_prfReset(OSA_PrfHndl *hndl)
{
	hndl->count=0;
	hndl->totalTime=0;
	hndl->maxTime=0;
	hndl->maxValue=0;
	hndl->minTime=0;
	hndl->minValue=0;
	hndl->totalValue=0;
	  
	return OSA_SOK;
}

int OSA_prfPrint(OSA_PrfHndl *hndl, char *name, Uint32 flags)
{
	OSA_printf(" \n");
	OSA_printf(" Profile Info  : %s \n", name);
	OSA_printf(" ======================\n");

	if (flags == 0)
	{
		flags = OSA_PRF_PRINT_DEFAULT;
	}
	
	if (flags & OSA_PRF_PRINT_COUNT ) 
	{
		OSA_printf(" Iterations    : %d \n", hndl->count);
	}

	if (flags & OSA_PRF_PRINT_TIME ) 
	{
		OSA_printf(" Avg Time (ms) : %9.2f \n", (float)hndl->totalTime/hndl->count);

		if (flags & OSA_PRF_PRINT_MIN_MAX )
		{
	  	OSA_printf(" Min Time (ms) : %d \n", hndl->minTime);
		}

		if (flags & OSA_PRF_PRINT_MIN_MAX )
		{
	  		OSA_printf(" Max Time (ms) : %d \n", hndl->maxTime);
		}
	}

	if (flags & OSA_PRF_PRINT_VALUE )
	{

		OSA_printf(" Avg Value     : %9.2f \n", (float)hndl->totalValue/hndl->count);
		OSA_printf(" Avg Value/sec : %9.2f \n", (float)(hndl->totalValue*1000)/hndl->totalTime);

		if (flags & OSA_PRF_PRINT_MIN_MAX)
		{
	  		OSA_printf(" Min Value     : %d \n", hndl->minValue);
		}

		if (flags & OSA_PRF_PRINT_MIN_MAX)
		{
	  		OSA_printf(" Max Value     : %d \n", hndl->maxValue);
		}
	}
	OSA_printf(" \n");  

	return OSA_SOK;
}




#endif
