#ifndef WINDOWCONTROLLER_H
#define WINDOWCONTROLLER_H

#include "IWindowEngine.h"
#include "CTimer.h"
#include "CRandGen.h"
#include "TrayIcon.h"
#include "../resource.h"
#include <cstdio>
#include <memory>

#define SNOW_COUNT  20
#define SNOW_CACHE_SIZE   96
#define THEME_WND_WIDTH   500
#define THEME_WND_HEIGHT  500

using namespace Phantom;

struct DeviceInformation
{
    static int screenWidth;
    static int screenHeigth;
    static int GetScreenSize(SIZE &size);
};

struct TransformObject
{
    int x;
    int y;
    int rotation;
    short offsetX;
    short offsetY;
    // Ҫ����yˮƽ�˶�����ļ���
    char  maxStateCount;
    char  stateCount;
    char  state;

    ITransparentWnd * pWnd;
    TransformObject(ITransparentWnd * pWnd,int wndSize)
    {
        SIZE size;
        DeviceInformation::GetScreenSize(size);
        x = CRandGen::Rand()%size.cx;
        y = -CRandGen::Rand()%size.cy;
        maxStateCount = CRandGen::Rand()%4;
        offsetX = 2 + (wndSize - 48) / 8;
        offsetY = offsetX*1.2;
        rotation = CRandGen::Rand()%36 * 10;
        this->pWnd = pWnd;
    }

    void ReinitSizeInfo(int wndSize)
    {
        maxStateCount = CRandGen::Rand()%4;
        offsetX = 2 + (wndSize - 48) / 8;
        offsetY = offsetX*1.2;
        rotation = CRandGen::Rand()%36 * 10;
    }

    BOOL TransformState()
    {
        y+=offsetY;
        if(stateCount >= maxStateCount)
        {
            state = CRandGen::Rand()%2;
            stateCount = 0;
            maxStateCount = CRandGen::Rand()%4;
        }
        else
        {
            stateCount++;
        }
        state?x+=offsetX:x-=offsetX;
        rotation+=10;
        if(rotation>=360)
        {
            rotation = 0;
        }
        SIZE screenSize;
        DeviceInformation::GetScreenSize(screenSize);
        if(x < 0 || y > screenSize.cy || x > screenSize.cx)
        {
            // Խ������
            x = CRandGen::Rand()%screenSize.cx;
            y = -(CRandGen::Rand()%screenSize.cy);

            return TRUE;
        }
        return FALSE;
    }

    operator ITransparentWnd*()
    {
        return pWnd;
    }
};

class WindowController
{
    struct SnowWndProc:public IProcObject
    {
        LRESULT operator()(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
        {
            switch(message)
            {
            case WM_DESTROY:
                printf("snow window : %l destroied.\n",hWnd);
            default:
                return DefWindowProc(hWnd,message,wParam,lParam);
            }
        }
    };

    struct ThemeWndProc:public IProcObject
    {
        static HWND            hWndAbout;
        static LRESULT AboutWndProc(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam)
        {
            HINSTANCE hInst = GetModuleHandle(0);
            switch (message)
            {
            case WM_INITDIALOG:
                {
                    CHAR  buffer[256];
                    ::LoadString(hInst,IDS_AUTHOR,buffer,256);
                    SetDlgItemTextA(hWnd,IDC_AUTHOR,buffer);
                    ::LoadString(hInst,IDS_AUTHOR_INFO,buffer,256);
                    SetDlgItemTextA(hWnd,IDC_AUTHOR_INFO,buffer);
                    ::LoadString(hInst,IDS_COPYRIGHT,buffer,256);
                    SetDlgItemTextA(hWnd,IDC_GIT,buffer);
                    ::LoadString(hInst,IDS_BGM,buffer,256);
                    SetDlgItemText(hWnd,IDC_BGM,buffer);
                    break;
                }
            case WM_CLOSE:
                {
                    ::EndDialog(hWnd,0);
                    ::DestroyWindow(hWnd);
                    hWndAbout = NULL;
                    return TRUE;
                    break;
                }
            case WM_COMMAND:
                {
                    switch(LOWORD(wParam))
                    {
                    case IDOK:
                        {
                            ::EndDialog(hWnd,0);
                            ::DestroyWindow(hWnd);
                            hWndAbout = NULL;
                            return TRUE;
                            break;
                        }
                    }
                    break;
                }
            default:
                return DefWindowProc (hWnd, message, wParam, lParam);
            }
            return TRUE;
        }

        LRESULT operator()(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
        {
            WindowController * controller = (WindowController*)m_this_ptr;
            switch (message)                  /* handle the messages */
            {
            case WM_LBUTTONDBLCLK:
                break;
                case WM_DESTROY:
                    break;
                case WM_TIMER:
                    break;
                case ID_TRAYICON_MESSAGE:
                    {
                        switch(lParam)
                        {
                        case WM_RBUTTONUP:
                            {
                                LPPOINT lpPoint = new tagPOINT();
                                ::GetCursorPos(lpPoint);
                                HMENU hMenu = ::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU));
                                hMenu = GetSubMenu(hMenu, 0);
                                ::SetForegroundWindow(hWnd);
                                ::TrackPopupMenu(hMenu,TPM_RIGHTBUTTON,lpPoint->x,lpPoint->y,0,hWnd,NULL);
                                delete lpPoint;
                                break;
                            }
                        }
                    }
                    break;
                case WM_COMMAND:
                    {
                        switch(LOWORD(wParam))
                        {
                        case IDM_EXIT1:
                            {
                                PostQuitMessage(0);
                                exit(0);
                                break;
                            }
                        case IDM_ABOUT:
                            {
                                if(!hWndAbout)
                                {
                                    hWndAbout = ::CreateDialog(GetModuleHandle(0),MAKEINTRESOURCE(IDD_ABOUT),hWnd,(DLGPROC)&AboutWndProc);
                                }
                                break;
                            }
                        }
                    }
                default:                      /* for messages that we don't deal with */
                    return DefWindowProc (hWnd, message, wParam, lParam);
            }
            return 0;
        }
    };

    struct SnowTimerProc:public CTimerCallback
    {
        size_t operator()(CTimerDataObject * pObj)
        {
            // get the hWnd find the ITransparent window object.
            WindowController * controller = (WindowController*)(this->m_this_ptr);
            ITransparentWnd * pWnd = controller->m_engine->GetTransparentWndForHWND(pObj->m_hWnd);
            TransformObject* pTransObj = controller->m_transformMap[pWnd];
            return controller->TransformForTransformObj(pTransObj);
        }
    };

    public:
        WindowController();
        LRESULT InitWindowController();
        virtual ~WindowController();
        ITransparentWnd* GetThemeWindow();
    protected:
        LRESULT TransformForTransformObj(TransformObject *pTransObj);


        static void * CreateSnowThread(void *param);
    private:
        // bitmap cache
        HBITMAP m_bitmaps[36];
        // window engine
        IWindowEngine * m_engine;
        // snow windows
        TransformObject* m_snow_array[SNOW_COUNT];
        // theme window
        ITransparentWnd* m_theme_wnd;

        // window proc
        SnowWndProc     m_snowWndProc;
        ThemeWndProc    m_themeWndProc;
        //
        CTimerDataObject* m_timerDataObj[SNOW_COUNT];
        SnowTimerProc   m_snowTimerProc;


        std::map<ITransparentWnd*,TransformObject*> m_transformMap;
        std::shared_ptr<TrayIcon> m_trayIcon;


};

#endif // WINDOWCONTROLLER_H
