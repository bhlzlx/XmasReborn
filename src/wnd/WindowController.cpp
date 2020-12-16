#include "WindowController.h"
#include <pthread.h>

#define MESSAGE_BOX(hwnd,message) MessageBox(hwnd,message,"Prompt Message",MB_OK)

using namespace Phantom;

HWND WindowController::ThemeWndProc::hWndAbout = NULL;

int DeviceInformation::GetScreenSize(SIZE &size)
{
    if(!(screenHeigth && screenWidth))
    {
        screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
        screenHeigth = ::GetSystemMetrics(SM_CYSCREEN);
    }
    size.cx = screenWidth;
    size.cy = screenHeigth;
    return 0;
}

int DeviceInformation::screenWidth = 0;
int DeviceInformation::screenHeigth = 0;

void * WindowController::CreateSnowThread(void *param)
{
    WindowController * controller = static_cast<WindowController*>(param);
    int i = 0;
    SIZE screenSize;
    DeviceInformation::GetScreenSize(screenSize);
    for(;i<SNOW_COUNT;i++)
    {
        int size = CRandGen::Rand()%48+48;
        ITransparentWnd *pWnd = controller->m_engine->CreateTransparentWnd(&controller->m_snowWndProc,size,size,CRandGen::Rand()%screenSize.cx,-CRandGen::Rand()%screenSize.cy,"PNG","IDR_PNG_SNOW");
        pWnd->SetTransparent();
        controller->m_snow_array[i] = new TransformObject(pWnd,size);
        controller->m_timerDataObj[i] = CTimer::GetTimerInstance()->CreateTimer(300 - size ,&controller->m_snowTimerProc,*pWnd,NULL);
        controller->m_transformMap[pWnd] = controller->m_snow_array[i];
    }

    MSG msg;

    while(GetMessage(&msg,NULL,0,0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return NULL;
}



WindowController::WindowController()
{
    //ctor
    this->m_engine = NULL;
    memset(this->m_snow_array,0,sizeof(m_snow_array));
    this->m_theme_wnd = NULL;
    this->m_snowWndProc.m_this_ptr = this;
    this->m_themeWndProc.m_this_ptr = this;
    this->m_snowTimerProc.m_this_ptr = this;
    memset(this->m_bitmaps,0,sizeof(m_bitmaps));
}

WindowController::~WindowController()
{
    //dtor
}

LRESULT WindowController::InitWindowController()
{
    m_engine = GetWindowEngine();
    assert(m_engine);

    m_engine->CreateBitCacheForRotation("PNG","IDR_PNG_SNOW",SNOW_CACHE_SIZE,SNOW_CACHE_SIZE,36,this->m_bitmaps);
    
    pthread_t t;
    pthread_create(&t,NULL,&CreateSnowThread,this);
   // pthread_join(t,NULL);
    SIZE screenSize;
    DeviceInformation::GetScreenSize(screenSize);
    this->m_theme_wnd = m_engine->CreateTransparentWnd(&m_themeWndProc,THEME_WND_WIDTH,THEME_WND_HEIGHT,(screenSize.cx-THEME_WND_WIDTH)/2,(screenSize.cy-THEME_WND_HEIGHT)/2,"PNG","IDR_PNG_THEME");
    m_theme_wnd->SetTransparent();
    this->m_trayIcon = std::shared_ptr<TrayIcon>(new TrayIcon(*m_theme_wnd,::LoadIcon(GetModuleHandle(NULL),MAKEINTRESOURCE(IDI_ICON)),"Happy Everyday!"));
    this->m_trayIcon->Create();
}

LRESULT WindowController::TransformForTransformObj(TransformObject *pTransObj)
{
    HWND hwnd = pTransObj->pWnd->GetSafeHWND();
    if(pTransObj->TransformState())
    {
        // �������ɴ�С
        int newSize = CRandGen::Rand()%48+48;
        if(!pTransObj->pWnd->Resize(newSize,newSize))
        {
            MESSAGE_BOX(NULL,"Resized failed.");
        }
        pTransObj->ReinitSizeInfo(newSize);
    }
    else
    {
        ::SetWindowPos(hwnd,NULL,pTransObj->x,pTransObj->y,0,0,SWP_NOSIZE|SWP_NOZORDER);
    }

    Phantom::StretchRefreshWnd(m_bitmaps+pTransObj->rotation/10,*pTransObj);

    return TRUE;
}

ITransparentWnd* WindowController::GetThemeWindow()
{
    return this->m_theme_wnd;
}
