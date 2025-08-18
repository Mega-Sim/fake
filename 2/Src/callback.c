#include "callback.h"
#include "axisdef.h"
#include "6xSystem.h"



void (*__cb[CALLBACK_CALL_END + 1]) () =
{
	NULL,
};



void __dsplib_callback_init()
{
	int i;
	for (i = 0; i <= CALLBACK_CALL_END; i ++)
		__dsplib_set_callback(i, NULL);
}

void * __dsplib_set_callback(int ofs, void (*func)())
{
    void (*ptr)();
    
    if (ofs < 0 || ofs >= CALLBACK_CALL_END) return NULL;
    ptr = __cb[ofs];
    __cb[ofs] = func;
    return ptr;
}


void * __dsplib_set_callback_param4(int ofs, void (*func)(void*prcv, int len, void *prtn, int *))
{
	void (*ptr)();
	ptr = (void (*)()) func;
	return __dsplib_set_callback(ofs, ptr);
}


void __dsplib_call_callback(int ofs)
{
	if (ofs < 0 || ofs >= CALLBACK_CALL_END) return;
	if (__cb[ofs] != NULL) __cb[ofs]();
}

void __dsplib_call_callback_param4(int ofs, void *p1, int nrcv, void *p2, int *psend)
{
	void (*ptr)(void *p1, int, void *p2, int *p);

	if (ofs < 0 || ofs >= CALLBACK_CALL_END-1) return;
	
	ptr = (void (*)(void *, int, void *, int *)) __cb[ofs];
	if (ptr != NULL) ptr(p1, nrcv, p2, psend);
}





