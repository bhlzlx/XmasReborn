#ifndef IPHANTOMUTIL_H
#define IPHANTOMUTIL_H

#include <windows.h>
#include "CGdiPlusBitmap.h"
#include <cassert>

namespace Phantom
{
    typedef BOOL(*UPDATELAYEREDWINDOWFUNCTION)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);

    struct IBase
    {
        enum ObjType
        {
            ENGINE,
            TRANSPARENT_WND
        };
    public:
        virtual int GetType() = 0;
        virtual int Release() = 0;
    };

    struct ITransparentWnd:public IBase
    {
        virtual HWND GetSafeHWND() = 0;
        virtual operator HWND () = 0;
        virtual LRESULT SetTransparent(BOOL value = TRUE) = 0;
        virtual LRESULT Resize(int cx,int cy) = 0;
        virtual SIZE    GetWindowSize() = 0;
    };

    struct IProcObject
    {
    public:
        void * m_this_ptr;
        virtual LRESULT operator()(HWND,UINT,WPARAM,LPARAM) = 0;
    };

    struct IWindowEngine:public IBase
    {
        virtual ITransparentWnd* CreateTransparentWnd(
                                 IProcObject* proc,
                                 const int & width,
                                 const int & height,
                                 const int & x,
                                 const int & y,
                                 LPCTSTR resType,
                                 LPCTSTR resName
                                 ) = 0;
       virtual LRESULT RefreshWndWithMemDC(ITransparentWnd *pWnd,HDC &memDC) = 0;
       virtual LRESULT CreateBitCacheForRotation(LPCTSTR resType,LPCTSTR resName,int width,int height,char num,HBITMAP *pHBITMAP) = 0;
       virtual ITransparentWnd* GetTransparentWndForHWND(HWND hwnd) = 0;
       virtual LRESULT DestroyBitmaps(char num,HBITMAP * pBitmaps)=0;
    };

    IWindowEngine* GetWindowEngine();

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
                            );
    LRESULT StretchRefreshWnd(HBITMAP *pBitmap,ITransparentWnd* pWnd);

}

#endif // IPHANTOMUTIL_H
