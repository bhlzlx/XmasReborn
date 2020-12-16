#include "IWindowEngine.h"
#include <windows.h>
#include <map>
#include <tr1/memory>
#include <tr1/shared_ptr.h>


#define TRANSPARENT_LAYERED_WINDOW  "Shin!!!Phantom!!!"
#define MESSAGE_BOX(hwnd,message) MessageBox(hwnd,message,"Prompt Message",MB_OK)

namespace Phantom
{
    struct WindowEngine;

    WindowEngine * g_WindowEngine = NULL;

    struct TransparentWnd:public ITransparentWnd
    {
        HWND            m_hWnd;
        IProcObject *   m_procObj;
        SIZE            m_size;

        TransparentWnd(IProcObject * pProc)
        {
            m_procObj = pProc;
        }

        LRESULT SetTransparent(BOOL value = TRUE)
        {
            long style = GetWindowLong(m_hWnd,GWL_EXSTYLE);
            if(value)
            {
                style^=WS_EX_TRANSPARENT;
            }
            else
            {
                style&=~WS_EX_TRANSPARENT;
            }
            SetWindowLong(m_hWnd,GWL_EXSTYLE,style);
            return TRUE;
        }

        HWND GetSafeHWND()
        {
            return m_hWnd;
        }

        operator HWND()
        {
            return m_hWnd;
        }

        LRESULT WindowProcedure(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
        {
            if(m_procObj)
            {
                return m_procObj->operator()(hWnd,message,wParam,lParam);
            }
            else
            {
                return DefWindowProc(hWnd,message,wParam,lParam);
            }
        }

        LRESULT Resize(int cx,int cy)
        {
            m_size.cx = cx;
            m_size.cy = cy;
            return ::SetWindowPos(m_hWnd,NULL,0,0,cx,cy,SWP_NOMOVE|SWP_NOZORDER);
        }

        SIZE    GetWindowSize()
        {
            return m_size;
        }

        int GetType()
        {
            return IBase::ObjType::TRANSPARENT_WND;
        }

        int Release()
        {
            DestroyWindow(this->m_hWnd);
            delete this;
            return 0;
        }
    };



    struct WindowEngine:public IWindowEngine
    {
        WNDCLASSEX                  m_wndClass;
        BLENDFUNCTION               m_blendFunction;
        UPDATELAYEREDWINDOWFUNCTION m_updateLayeredWindowProc;
        HMODULE                     m_hUser32;
        ULONG_PTR                   m_gdiplusStartupToken;

        std::map<HWND,ITransparentWnd*>\
        m_WndMap;


        virtual ITransparentWnd* CreateTransparentWnd(
            IProcObject* proc,
            const int & width,
            const int & height,
            const int & x,
            const int & y,
            LPCTSTR resType,
            LPCTSTR resName
        )
        {
            TransparentWnd * pTranspWnd = new TransparentWnd(proc);
            pTranspWnd->m_size.cx = width;
            pTranspWnd->m_size.cy = height;
            HWND& hWnd = pTranspWnd->m_hWnd = CreateWindowEx (
                WS_EX_LAYERED|WS_EX_TOOLWINDOW,//|WS_EX_TRANSPARENT,                   /* Extended possibilites for variation */
                TRANSPARENT_LAYERED_WINDOW,         /* structname */
                "TransparentWindow",       /* Title Text */
                WS_OVERLAPPEDWINDOW, /* default window */
                x,       /* Windows decides the position */
                y,       /* where the window ends up on the screen */
                width,                 /* The programs width */
                height,                 /* and height in pixels */
                HWND_DESKTOP,        /* The window is a child-window to desktop */
                NULL,                /* No menu */
                ::GetModuleHandle(0),       /* Program Instance handler */
                NULL                 /* No Window Creation data */
            );

            if(!hWnd)
            {
                MESSAGE_BOX(NULL,"Can not create PhantomWindow.");
                return NULL;
            }
            m_WndMap[hWnd] = pTranspWnd;

            LONG style = ::GetWindowLong(hWnd,GWL_STYLE);

            if(style&WS_CAPTION)
                style^=WS_CAPTION;
            if(style&WS_THICKFRAME)
                style^=WS_THICKFRAME;
            if(style&WS_SYSMENU)
                style^=WS_SYSMENU;
            ::SetWindowLong(hWnd,GWL_STYLE,style);

            style = ::GetWindowLong(hWnd,GWL_EXSTYLE);
            if(style&WS_EX_APPWINDOW)
                style^=WS_EX_APPWINDOW;
            ::SetWindowLong(hWnd,GWL_EXSTYLE,style);

            // get screen dc
            POINT ptSrc = {0,0};
            SIZE wndSize = {width,height};

            //BOOL result = g_UpdateLayeredWindow(hWnd,NULL,NULL,&wndSize,imageCacheDCArray[dcIndex],&ptSrc,0,&blendFunction,2);
            std::shared_ptr<CGdiPlusBitmapResource> pBitmap;
            pBitmap = std::shared_ptr<CGdiPlusBitmapResource>(new CGdiPlusBitmapResource);
            if(!pBitmap->Load(resName,resType))
            {
                MESSAGE_BOX(NULL,"Failed loading the gdiplus resource!");
                return NULL;
            }
            HRESULT hr = 0;
            HDC hdc = ::GetDC(hWnd);
            HDC memDC = CreateCompatibleDC(hdc);
            HBITMAP memBitmap = ::CreateCompatibleBitmap(hdc,width,height);
            HBITMAP oldBitmap = (HBITMAP)::SelectObject(memDC,memBitmap);
            ::ReleaseDC(hWnd,hdc);
            Gdiplus::Graphics graphics(memDC);
            graphics.DrawImage(*pBitmap,0,0,width,height);
            hr = m_updateLayeredWindowProc(hWnd,NULL,NULL,&wndSize,memDC,&ptSrc,0,&m_blendFunction,2);
            ::ShowWindow(hWnd,SW_SHOW);
            ::SetWindowPos(hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
            if(!hr)
            {
                MESSAGE_BOX(NULL,"Can't Invoke UpdateLayeredWindow Function!");
            }
            // Clean up staffs
            ::SelectObject(memDC,oldBitmap);
            ::DeleteObject(memBitmap);
            ::DeleteDC(memDC);
            return pTranspWnd;
        }


        static LRESULT CALLBACK WindowProcedure(HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam)
        {
            TransparentWnd * pTransWnd = (TransparentWnd*)g_WindowEngine->m_WndMap[hWnd];
            if(pTransWnd)
            {
                return pTransWnd->WindowProcedure(hWnd,message,wParam,lParam);
            }
            else
            {
                return DefWindowProc(hWnd,message,wParam,lParam);
            }
        }

        int GetType()
        {
            IBase::ObjType::ENGINE;
            return 0;
        }

        int Release()
        {
            Gdiplus::GdiplusShutdown(m_gdiplusStartupToken);
            ::FreeLibrary(this->m_hUser32);
            delete this;
            return 0;
        }

        LRESULT RefreshWndWithMemDC(ITransparentWnd *pWnd,HDC &memDC)
        {
            HWND hWnd = pWnd->GetSafeHWND();
            static POINT ptSrc = {0};
            static HDC screenDC = ::GetDC(NULL);
            LRESULT ret = m_updateLayeredWindowProc(hWnd,NULL,NULL,&((TransparentWnd*)pWnd)->m_size,memDC,&ptSrc,0,&m_blendFunction,2);
            if(ret == 0)
            {
                printf("failed...");
                DWORD errorCode = ::GetLastError();
                printf("error code: %d",errorCode);
            }

            ReleaseDC(NULL,screenDC);
            return ret;
        }

        LRESULT CreateBitCacheForRotation(LPCTSTR resType,LPCTSTR resName,int width,int height,char num,HBITMAP *pHBITMAP)
        {
            assert(resType && resName && pHBITMAP && width && height && num);
            // Create CGdiPlusBitmapResource Object to load Embed resource.
            std::shared_ptr<CGdiPlusBitmapResource> pBitmap;
            pBitmap = std::shared_ptr<CGdiPlusBitmapResource>(new CGdiPlusBitmapResource);
            if(!pBitmap->Load(resName,resType))
            {
                return FALSE;
            }
            float sliceAngle = 360.0f / num;
            HDC screenDC = ::GetDC(NULL);
            HDC compatibleDC = ::CreateCompatibleDC(screenDC);
            HBITMAP oldBitmap;
            for(int i = 0;i< num ;i++)
            {
                pHBITMAP[i] = ::CreateCompatibleBitmap(screenDC,width,height);
                oldBitmap = (HBITMAP)::SelectObject(compatibleDC,pHBITMAP[i]);
                Gdiplus::Graphics graphics(compatibleDC);
                Gdiplus::Matrix matrix;
                Gdiplus::PointF pt = Gdiplus::PointF(width/2,height/2);
                matrix.RotateAt(i*sliceAngle,pt,Gdiplus::MatrixOrderAppend);
                graphics.SetTransform(&matrix);
                graphics.DrawImage(*pBitmap,0,0,width,height);
                ::SelectObject(compatibleDC,oldBitmap);
            }
            ::ReleaseDC(NULL,screenDC);
            ::DeleteDC(compatibleDC);
            return TRUE;
        }

        ITransparentWnd* GetTransparentWndForHWND(HWND hwnd)
        {
            std::map<HWND,ITransparentWnd*>::iterator iter = this->m_WndMap.find(hwnd);
            if(iter == this->m_WndMap.end())
            {
                return NULL;
            }
            else
            {
                return (iter->second);
            }
        }

        LRESULT DestroyBitmaps(char num,HBITMAP * pBitmaps)
        {
            char i = 0;
            while( i < num )
            {
                ::DeleteObject(pBitmaps[i]);
            }
            return TRUE;
        }
    };

    IWindowEngine* GetWindowEngine()
    {
        if(NULL == g_WindowEngine)
        {
            g_WindowEngine = new WindowEngine();
            // ��ʼ��gdi+
            Gdiplus::GdiplusStartupInput gdiInput;
            Gdiplus::GdiplusStartup(&g_WindowEngine->m_gdiplusStartupToken,&gdiInput,NULL);

            WNDCLASSEX& wndstruct = g_WindowEngine->m_wndClass;
            wndstruct.hInstance = ::GetModuleHandle(0);
            wndstruct.lpszClassName = TRANSPARENT_LAYERED_WINDOW;
            wndstruct.lpfnWndProc = &WindowEngine::WindowProcedure;      /* This function is called by windows */
            wndstruct.style = CS_DBLCLKS;                 /* Catch double-clicks */
            wndstruct.cbSize = sizeof (WNDCLASSEX);
            /* Use default icon and mouse-pointer */
            wndstruct.hIcon = LoadIcon (NULL, IDI_APPLICATION);
            wndstruct.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
            wndstruct.hCursor = LoadCursor (NULL, IDC_ARROW);
            wndstruct.lpszMenuName = NULL;                 /* No menu */
            wndstruct.cbClsExtra = 0;                      /* No extra bytes after the window struct */
            wndstruct.cbWndExtra = 0;                      /* structure or the window instance */
            /* Use Windows's default colour as the background of the window */
            wndstruct.hbrBackground = (HBRUSH) COLOR_BACKGROUND;
            if(!RegisterClassEx(&wndstruct))
            {
                MESSAGE_BOX(NULL,"Register Window Class Failed!");
                return NULL;
            }

            // Construct Blend Function Object
            BLENDFUNCTION &blendFunction = g_WindowEngine->m_blendFunction;
            blendFunction.AlphaFormat = AC_SRC_ALPHA;
            blendFunction.BlendFlags = 0;
            blendFunction.BlendOp = AC_SRC_OVER;
            blendFunction.SourceConstantAlpha = 255;

            HMODULE& hUser32 = g_WindowEngine->m_hUser32 = ::LoadLibrary("User32.dll");
            if(!hUser32)
            {
                return NULL;
            }

            g_WindowEngine->m_updateLayeredWindowProc = (UPDATELAYEREDWINDOWFUNCTION)::GetProcAddress(hUser32,"UpdateLayeredWindow");

            if(!g_WindowEngine->m_updateLayeredWindowProc)
            {
                return NULL;
            }
            else
            {
                return g_WindowEngine;
            }
        }
        else
        {
            return g_WindowEngine;
        }

    }

    LRESULT SmoothStretchBlt(
        HDC hdcDest,      // handle to destination DC
        int nXOriginDest, // x-coord of destination upper-left corner
        int nYOriginDest, // y-coord of destination upper-left corner
        int nWidthDest,   // width of destination rectangle
        int nHeightDest,  // height of destination rectangle
        HDC hdcSrc,       // handle to source DC
        int nXOriginSrc,  // x-coord of source upper-left corner
        int nYOriginSrc,  // y-coord of source upper-left corner
        int nWidthSrc,    // width of source rectangle
        int nHeightSrc,   // height of source rectangle
        DWORD dwRop       // raster operation code
    )
    {
        ::SetStretchBltMode (hdcDest, COLORONCOLOR);
        ::SetBrushOrgEx(hdcDest, 0, 0, NULL);
        return ::StretchBlt(
            hdcDest,
            nXOriginDest,
            nYOriginDest,
            nWidthDest,
            nHeightDest,
            hdcSrc,
            nXOriginSrc,
            nYOriginSrc,
            nWidthSrc,
            nHeightSrc,
            dwRop
        );
    }

    LRESULT StretchRefreshWnd(HBITMAP *pBitmap,ITransparentWnd* pWnd)
    {
        BITMAP info;
        ::GetObject(pBitmap[0],sizeof(info),&info);
        TransparentWnd * pTransWnd = (TransparentWnd *)pWnd;
        HDC destDC = ::GetDC(pWnd->GetSafeHWND());
        HDC memDC  = ::CreateCompatibleDC(destDC);
        HDC srcDC = ::CreateCompatibleDC(destDC);
        HBITMAP memBitmap = ::CreateCompatibleBitmap(destDC,pTransWnd->m_size.cx,pTransWnd->m_size.cy);
        HBITMAP oldBitmapSrc,oldBitmapMem;
        oldBitmapSrc = (HBITMAP)::SelectObject(srcDC,*pBitmap);
        oldBitmapMem = (HBITMAP)::SelectObject(memDC,memBitmap);
        BOOL result = SmoothStretchBlt(
                         memDC,
                         0,
                         0,
                         pTransWnd->m_size.cx,
                         pTransWnd->m_size.cy,
                         srcDC,
                         0,0,
                         info.bmWidth,
                         info.bmHeight,
                         SRCCOPY
                         );
        g_WindowEngine->RefreshWndWithMemDC(pWnd,memDC);
        ::SelectObject(memDC,oldBitmapMem);
        ::SelectObject(srcDC,oldBitmapSrc);
        ::DeleteObject(memBitmap);
        ::DeleteDC(memDC);
        ::DeleteDC(srcDC);
        ::ReleaseDC(*pWnd,destDC);

        return TRUE;
    }

}

