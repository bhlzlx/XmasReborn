#ifndef CTIMER_H
#define CTIMER_H

#include "IWindowEngine.h"
#include <windows.h>
#include <map>
#include <utility>

class CTimer;

struct CTimerCallback;

struct CTimerDataObject
{
    UINT_PTR                    m_TimerID;
    UINT                        m_elapse;
    HWND                        m_hWnd;
    CTimerCallback *            m_callback;
    void*                       m_pUserData;
};

struct CTimerCallback
{
    void * m_this_ptr;
    virtual size_t operator()(CTimerDataObject *) = 0;
};

class CTimer
{
private:
    CTimer();
public:
    int Release();
    virtual ~CTimer();
public:
    CTimerDataObject* CreateTimer(unsigned int elapse,CTimerCallback* callback,HWND hWnd,void *pUserData);
    void              KillTimer(CTimerDataObject *);

    static CTimer* GetTimerInstance();
    static void CALLBACK TimerProc ( HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime);
protected:

private:
    std::map<UINT_PTR ,CTimerDataObject* > m_timerMap;
};

#endif // CTIMER_H
