#include "CTimer.h"
#include <cassert>

CTimer * g_Timer = NULL;

CTimer::CTimer()
{
}

CTimerDataObject* CTimer::CreateTimer(unsigned int elapse,CTimerCallback* callback,HWND hWnd,void *pUserData)
{
    SetTimer(hWnd,m_timerMap.size(),elapse,&CTimer::TimerProc);

    CTimerDataObject * pObj = new CTimerDataObject();
    pObj->m_callback = callback;
    pObj->m_elapse = elapse;
    pObj->m_hWnd = hWnd;
    pObj->m_pUserData = pUserData;
    pObj->m_TimerID = m_timerMap.size();

    m_timerMap[m_timerMap.size()] = pObj;
    return pObj;
}

CTimer::~CTimer()
{

}

void CTimer::KillTimer(CTimerDataObject * pObj)
{
    assert(pObj);
    ::KillTimer(pObj->m_hWnd,pObj->m_TimerID);
    this->m_timerMap.erase(pObj->m_TimerID);
    delete pObj;
}

int CTimer::Release()
{
    std::map<UINT_PTR,CTimerDataObject*>::iterator iter = this->m_timerMap.begin();
    while(iter != m_timerMap.end())
    {
        CTimerDataObject * pObj = iter->second;
        ::KillTimer(pObj->m_hWnd,pObj->m_TimerID);
        delete pObj;
        iter++;
    }
    return 0;
}

CTimer* CTimer::GetTimerInstance()
{
    if(!g_Timer)
    {
        g_Timer = new CTimer();
    }
    return g_Timer;
}

void CALLBACK CTimer::TimerProc ( HWND hwnd, UINT message, UINT_PTR iTimerID, DWORD dwTime)
{
    if(g_Timer->m_timerMap.size() == 0)
    {
        return;
    }

    CTimerDataObject* pObj = (CTimerDataObject*)g_Timer->m_timerMap[iTimerID];

    if(pObj)
    {
        pObj->m_callback->operator()(pObj);
    }
}
