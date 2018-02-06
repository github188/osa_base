

#ifndef _OSA_BUF_H_
#define _OSA_BUF_H_

#include <osa_que.h>
#include <osa_mutex.h>

#define OSA_BUF_NUM_MAX       (120)

#define OSA_BUF_ID_INVALID    (-1)

#define	MAKEFOURCC(ch0, ch1, ch2, ch3)                      \
		((Uint32)(Uint8)(ch0) | ((Uint32)(Uint8)(ch1) << 8) |   \
		((Uint32)(Uint8)(ch2) << 16) | ((Uint32)(Uint8)(ch3) << 24 ))
		

#define	H264_CODEC_TYPE   			MAKEFOURCC('H','2','6','4')
#define	AAC_CODEC_TYPE    			MAKEFOURCC('A','D','T','S')



typedef struct RH_FRAMEHEAD_ 
{
	unsigned char  check_start[4]; /*,标示符*/
	//Fream_Info_t frame;
	
	unsigned int ID;							//=mmioFOURCC('4','D','S','P');
	unsigned int nChannel;
	unsigned int nPTimeTick;    					//time
	unsigned int nDTimeTick;    					//time
	unsigned int nFrameLength; 					//length of frame
	unsigned int nDataCodec;   					//encode type
	unsigned int s_blue;						// 1 blue， 0 no

	//video:mmioFOURCC('H','2','6','4');
	//audio:mmioFOURCC('A','D','T','S');
	union {
		unsigned int nFrameRate;   				//video:framerate
		unsigned int nSamplerate;   				//aduio:samplerate (default:44100)
	};
	union {
		unsigned int nWidth;       					//video:width
		unsigned int nAudioChannel;					//audio:channel (default:2)
	};
	union {
		unsigned int nHight;       					//video:height
		unsigned int nSamplebit;					//audio:samplebit (default:16)
	};
	union {
		unsigned int nColors;      					//video:colors
		unsigned int nBandwidth;				//audio:bandwidth (default:64000)
	};
	union {
		unsigned int nIframe;					//video: I frame
		unsigned int nReserve;					//audio:  reserve
	};
	struct timeval presentationTime;
	unsigned int nPacketNumber; 					//packages serial number
	unsigned int nOthers;      						//reserve	
	unsigned char  check_end[4]; /*,标示符*/
} Frame_Head_t;


typedef struct
{
	Frame_Head_t head;
	char *data;
} OSA_BufInfo;

typedef struct {

  OSA_BufInfo bufInfo[OSA_BUF_NUM_MAX];
  
  OSA_QueHndl emptyQue;
  OSA_QueHndl fullQue;
      
  int numBuf;

} OSA_BufHndl;


int  OSA_bufCreate(OSA_BufHndl *hndl, int buf_num);
int  OSA_bufDelete(OSA_BufHndl *hndl);

int  OSA_bufGetFull(OSA_BufHndl *hndl, int *bufId, Uint32 timeout);
int  OSA_bufPutEmpty(OSA_BufHndl *hndl, int bufId);

int  OSA_bufGetEmpty(OSA_BufHndl *hndl, int *bufId, Uint32 timeout);
int  OSA_bufPutFull(OSA_BufHndl *hndl, int bufId);

int  OSA_bufSwitchFull (OSA_BufHndl *hndl, int *bufId);
int  OSA_bufSwitchEmpty(OSA_BufHndl *hndl, int *bufId);

OSA_BufInfo *OSA_bufGetBufInfo(OSA_BufHndl *hndl, int bufId);

#endif /* _OSA_BUF_H_ */



