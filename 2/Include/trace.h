#ifndef	__TRACE_H
#define	__TRACE_H

#ifdef TRACE_GLOBALS
#define TRACE_EXT
#else
#define TRACE_EXT extern
#endif

////////////////////////////////////////////////////////////////////////
// Define
////////////////////////////////////////////////////////////////////////
#define TRACE_CNT_ITER							1
#define Trace_Data_Buffer_Size					750001
#define MTrace_Data_Buffer_Size					80001
#define Trace_Data_Count_M						25
#define Trace_Data_Transfer_Buffer_Size_C		61
#define Trace_Data_Transfer_Buffer_Size_M		51

typedef struct _Queue
{
	volatile int front;
	volatile int rear;
	volatile int queArr[Trace_Data_Buffer_Size];
} Queue;

typedef struct _MQueue
{
	volatile int front;
	volatile int rear;
	volatile int queArr[MTrace_Data_Buffer_Size];
} MQueue;

////////////////////////////////////////////////////////////////////////
// Variable
////////////////////////////////////////////////////////////////////////
TRACE_EXT int trace_cnt_iteration;
TRACE_EXT int trace_on_flag[JPC_AXIS];
TRACE_EXT int Trace_Update_Status;

////////////////////////////////////////////////////////////////////////
// Function
////////////////////////////////////////////////////////////////////////
TRACE_EXT void QueueInit(Queue * pq);
TRACE_EXT int QIsEmpty(Queue * pq);
TRACE_EXT int NextPosIdx(int pos);
TRACE_EXT void Enqueue(Queue * pq, int data);
TRACE_EXT int Dequeue(Queue * pq);

TRACE_EXT void MQueueInit(MQueue * pq);
TRACE_EXT int MQIsEmpty(MQueue * pq);
TRACE_EXT int MNextPosIdx(int pos);
TRACE_EXT void MEnqueue(MQueue * pq, int data);
TRACE_EXT int MDequeue(MQueue * pq);

#endif

