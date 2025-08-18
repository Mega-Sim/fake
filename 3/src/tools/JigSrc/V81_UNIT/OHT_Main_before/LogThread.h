//---------------------------------------------------------------------------

#ifndef LogThreadH
#define LogThreadH
//---------------------------------------------------------------------------

#include <Classes.hpp>
#include "ThreadCtrl.h"
#include "LogManager.h"

//---------------------------------------------------------------------------

class LogThread:public ThreadCtrl
{
protected:
	void RealThreadProc(void);	// Execute �� ���� �Լ�
    
public:
	LogThread();
	~LogThread();

private:
	//2013.07.11 Critical Section
	CRITICAL_SECTION	m_CS;

};
//---------------------------------------------------------------------------
#endif
