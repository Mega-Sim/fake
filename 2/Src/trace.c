#define TRACE_GLOBALS

#include "all.h"

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : QueueInit
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
void QueueInit(Queue * pq)
{
	pq->front = 0;
	pq->rear = 0;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : QIsEmpty
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
int QIsEmpty(Queue * pq)
{
	if(pq->front == pq->rear)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : NextPosIdx
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
int NextPosIdx(int pos)
{
	if(pos == (Trace_Data_Buffer_Size-1))
	{
		return 0;
	}
	else
	{
		return pos+1;
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : Enqueue
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
void Enqueue(Queue * pq, int data)
{
	if(NextPosIdx(pq->rear) == pq->front)
	{
		Dequeue(pq);
	}

	pq->rear = NextPosIdx(pq->rear);
	pq->queArr[pq->rear] = data;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : Dequeue
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
int Dequeue(Queue * pq)
{
	pq->front = NextPosIdx(pq->front);
	return pq->queArr[pq->front];
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : MQueueInit
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
void MQueueInit(MQueue * pq)
{
	pq->front = 0;
	pq->rear = 0;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : MQIsEmpty
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
int MQIsEmpty(MQueue * pq)
{
	if(pq->front == pq->rear)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : MNextPosIdx
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
int MNextPosIdx(int pos)
{
	if(pos == (MTrace_Data_Buffer_Size-1))
	{
		return 0;
	}
	else
	{
		return pos+1;
	}
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : MEnqueue
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
void MEnqueue(MQueue * pq, int data)
{
	if(MNextPosIdx(pq->rear) == pq->front)
	{
		MDequeue(pq);
	}

	pq->rear = MNextPosIdx(pq->rear);
	pq->queArr[pq->rear] = data;
}

#pragma CODE_SECTION(".user_int");
////////////////////////////////////////////////////////////////////////
// Function : MDequeue
// Brief    : 
// Input    :
// Return   :
// History  :
// Date     :
////////////////////////////////////////////////////////////////////////
int MDequeue(MQueue * pq)
{
	pq->front = MNextPosIdx(pq->front);
	return pq->queArr[pq->front];
}

