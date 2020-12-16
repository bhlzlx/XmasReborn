#include "CPhantomWnd.h"
#include "TrayIcon.h"
#include "CTimer.h"

std::vector<std::shared_ptr<WndData> > g_WndArray;

HDC     g_ImageCacheDC[IMAGE_CACHE_COUNT];
HBITMAP g_OldBitmap[IMAGE_CACHE_COUNT];
HBITMAP g_ImageBitmaps[IMAGE_CACHE_COUNT];

HWND    g_hWndAbout = NULL;

typedef BOOL(*UPDATELAYEREDWINDOWFUNCTION)(HWND,HDC,POINT*,SIZE*,HDC,POINT*,COLORREF,BLENDFUNCTION*,DWORD);
UPDATELAYEREDWINDOWFUNCTION g_UpdateLayeredWindow;

HMODULE hUser32;
BLENDFUNCTION blendFunction;


using namespace Gdiplus;



LRESULT CALLBACK WindowProcedure (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
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
                case IDM_ABOUT_APPLICATION:
                    {
                        if(!g_hWndAbout)
                        {
                            g_hWndAbout = ::CreateDialog(GetModuleHandle(0),MAKEINTRESOURCE(IDD_ABOUT),hWnd,(DLGPROC)&AboudDialogProcedue);
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

LRESULT CALLBACK AboudDialogProcedue(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam)
{
    HINSTANCE hInst = GetModuleHandle(0);
    switch (message)                  /* handle the messages */
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
            g_hWndAbout = NULL;
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
                    g_hWndAbout = NULL;
                    return TRUE;
                    break;
                }
            }
            break;
        }
    default:                      /* for messages that we don't deal with */
        return DefWindowProc (hWnd, message, wParam, lParam);
    }
    return TRUE;
}

std::shared_ptr<WndData> CreateSnowWindow(
                             HDC* imageCacheDCArray
                             )
{
    std::shared_ptr<WndData> pWndData = std::shared_ptr<WndData>(new WndData);

    HWND hWnd = CreateWindowEx (
                                       WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT,                   /* Extended possibilites for variation */
                                       PHANTOM_WND_CLASS_NAME,         /* Classname */
                                       "Are you using the Spy++ inspecting me?!",       /* Title Text */
                                       WS_OVERLAPPEDWINDOW, /* default window */
                                       0,       /* Windows decides the position */
                                       0,       /* where the window ends up on the screen */
                                       0,                 /* The programs width */
                                       0,                 /* and height in pixels */
                                       HWND_DESKTOP,        /* The window is a child-window to desktop */
                                       NULL,                /* No menu */
                                       ::GetModuleHandle(0),       /* Program Instance handler */
                                       NULL                 /* No Window Creation data */
                                       );

    if(!hWnd)
    {
        PHANTOM_MESSAGE(NULL,"Can not create PhantomWindow.");
        return pWndData;
    }

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

    pWndData->m_hWnd = hWnd;
    pWndData->RefreshWndAnimationState();
    pWndData->UpdateWnd();
    g_WndArray.push_back(pWndData);

    return pWndData;
}

std::shared_ptr<WndData> CreateThemeWindow(
                                         int x,
                                         int y,
                                         int width,
                                         int height,
                                         int rotation
                                      )
{
    std::shared_ptr<WndData> pWndData = std::shared_ptr<WndData>(new WndData);
    pWndData->RefreshWndAnimationState();
    HWND hWnd = CreateWindowEx (
                                       WS_EX_LAYERED|WS_EX_TOOLWINDOW|WS_EX_TRANSPARENT,                   /* Extended possibilites for variation */
                                       PHANTOM_WND_CLASS_NAME,         /* Classname */
                                       "ThemeWindow",       /* Title Text */
                                       WS_OVERLAPPEDWINDOW, /* default window */
                                       x,       /* Windows decides the position */
                                       y,       /* where the window ends up on the screen */
                                       width,                 /* The programs width */
                                       height,                 /* and height in pixels */
                                       HWND_DESKTOP,        /* The window is a child-window to desktop */
                                       ::LoadMenu(NULL,MAKEINTRESOURCE(IDR_MENU)),                /* No menu */
                                       ::GetModuleHandle(0),       /* Program Instance handler */
                                       NULL                 /* No Window Creation data */
                                       );

    if(!hWnd)
    {
        PHANTOM_MESSAGE(NULL,"Can not create PhantomWindow.");
        return pWndData;
    }

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
    UINT dcIndex = rotation / 10;
    if(dcIndex == 36)
    {
        dcIndex = 0;
    }
    //BOOL result = g_UpdateLayeredWindow(hWnd,NULL,NULL,&wndSize,imageCacheDCArray[dcIndex],&ptSrc,0,&blendFunction,2);
    pWndData->m_hWnd = hWnd;
    pWndData->x = x;// = {x,y,x+width,y+height};
    pWndData->y = y;
    pWndData->width = width;
    pWndData->height = height;
    pWndData->rotation = rotation;
///
    std::shared_ptr<CGdiPlusBitmapResource> pBitmap;
    pBitmap = std::shared_ptr<CGdiPlusBitmapResource>(new CGdiPlusBitmapResource);
    if(!pBitmap->Load(_T("IDR_PNG_THEME"),_T("PNG")))
    {
        return pWndData;
    }
    HRESULT hr = 0;
    HDC hdc = ::GetDC(pWndData->m_hWnd);
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBitmap = ::CreateCompatibleBitmap(hdc,width,height);
    HBITMAP oldBitmap = (HBITMAP)::SelectObject(memDC,memBitmap);
    ::ReleaseDC(pWndData->m_hWnd,hdc);
    Graphics graphics(memDC);
    graphics.DrawImage(*pBitmap,0,0,width,height);
    hr = g_UpdateLayeredWindow(pWndData->m_hWnd,NULL,NULL,&wndSize,memDC,&ptSrc,0,&blendFunction,2);
    ::SetWindowPos(pWndData->m_hWnd,HWND_TOPMOST,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);
    if(!hr)
    {
        PHANTOM_MESSAGE(NULL,"Can't Invoke UpdateLayeredWindow Function!");
    }
    // Clean up staffs
    ::SelectObject(memDC,oldBitmap);
    ::DeleteObject(memBitmap);
    ::DeleteDC(memDC);

    return pWndData;
}

VOID DestroyTransparintWindow(WndData *pWndData)
{
    DestroyWindow(pWndData->m_hWnd);
}

HRESULT InitEnssentialFunction()
{
    // Register Window Class
    WNDCLASSEX wndClass;
    wndClass.hInstance = ::GetModuleHandle(0);
    wndClass.lpszClassName = PHANTOM_WND_CLASS_NAME;
    wndClass.lpfnWndProc = &WindowProcedure;      /* This function is called by windows */
    wndClass.style = CS_DBLCLKS;                 /* Catch double-clicks */
    wndClass.cbSize = sizeof (WNDCLASSEX);

    /* Use default icon and mouse-pointer */
    wndClass.hIcon = LoadIcon (NULL, IDI_APPLICATION);
    wndClass.hIconSm = LoadIcon (NULL, IDI_APPLICATION);
    wndClass.hCursor = LoadCursor (NULL, IDC_ARROW);
    wndClass.lpszMenuName = NULL;                 /* No menu */
    wndClass.cbClsExtra = 0;                      /* No extra bytes after the window class */
    wndClass.cbWndExtra = 0;                      /* structure or the window instance */
    /* Use Windows's default colour as the background of the window */
    wndClass.hbrBackground = (HBRUSH) COLOR_BACKGROUND;

    if(!RegisterClassEx(&wndClass))
    {
        return S_FALSE;
    }
    // Construct Blend Function Object
    blendFunction.AlphaFormat = AC_SRC_ALPHA;
    blendFunction.BlendFlags = 0;
    blendFunction.BlendOp = AC_SRC_OVER;
    blendFunction.SourceConstantAlpha = 255;
    // LoadLibrary
    hUser32 = ::LoadLibrary("User32.dll");
    if(!hUser32)
    {
        return FALSE;
    }

    g_UpdateLayeredWindow = (UPDATELAYEREDWINDOWFUNCTION)::GetProcAddress(hUser32,"UpdateLayeredWindow");

    if(!g_UpdateLayeredWindow)
    {
        return S_FALSE;
    }
    // Load Image
    HDC hdc = ::GetDC(NULL);
    // Create CGdiPlusBitmapResource Object to load Embed resource.
    std::shared_ptr<CGdiPlusBitmapResource> pBitmap;
    pBitmap = std::shared_ptr<CGdiPlusBitmapResource>(new CGdiPlusBitmapResource);
    if(!pBitmap->Load(_T("IDR_PNG"),_T("PNG")))
    {
        return false;
    }

    for(UINT i = 0;i< IMAGE_CACHE_COUNT;i++)
    {
        g_ImageCacheDC[i] = ::CreateCompatibleDC(hdc);
        g_ImageBitmaps[i] = ::CreateCompatibleBitmap(hdc,SNOW_SIZE,SNOW_SIZE);
        g_OldBitmap[i] = (HBITMAP)::SelectObject(g_ImageCacheDC[i],g_ImageBitmaps[i]);
        Gdiplus::Graphics graphics(g_ImageCacheDC[i]);
        Matrix matrix;
        PointF pt =PointF(SNOW_SIZE/2,SNOW_SIZE/2);
        matrix.RotateAt(i*10,pt,MatrixOrderAppend);
        graphics.SetTransform(&matrix);
        graphics.DrawImage(*pBitmap,0,0,SNOW_SIZE,SNOW_SIZE);
    }
    return S_OK;
}

HDC* GetDCArray()
{
    return g_ImageCacheDC;
}

HRESULT CleanUpSystem()
{
    // �ͷŶ�̬���ӿ�
    ::FreeLibrary(hUser32);
    for(int i = 0;i<IMAGE_CACHE_COUNT;i++)
    {
        ::SelectObject(g_ImageCacheDC[i],g_OldBitmap[i]);
        ::DeleteObject(g_ImageBitmaps[i]);
        ::DeleteDC(g_ImageCacheDC[i]);
    }
    // ���ٴ���
    return S_OK;
}

HDC& GetImageCacheDC(UINT index)
{
    return g_ImageCacheDC[index];
}

WndData::WndData()
{
    m_hWnd = NULL;
}

void WndData::RefreshWndAnimationState()
{
    int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);
    x = CRandGen::Rand()%screenWidth;
    y = -(CRandGen::Rand()%(screenHeight/4));
    printf("x:%d,y:%d ",x,y);
    int size = CRandGen::Rand()%48+48;
    width = this->height = size;
    ::SetWindowPos(m_hWnd,HWND_TOPMOST,x,y,width,height,SWP_SHOWWINDOW);

    this->offsetX = 2 + (size - 48) / 8;
    this->offsetY = offsetX * 1.2;
    this->rotation = (CRandGen::Rand()%36)*10;
}

void WndData::StepMove(bool moveFlag)
{
    if(lastDirCount < 2)
    {
        if(lastDir)
        {
            x+=offsetX;
        }
        else
        {
            x-=offsetX;
        }
        lastDirCount++;
    }
    else
    {
        lastDirCount = 1;
        if(moveFlag)
        {
            x += offsetX;
            lastDir = true;
        }
        else
        {
            x -= offsetX;
            lastDir = false;
        }
    }

    y+=offsetY;
    //printf("%d",offsetY);
    rotation+=10;

    int screenWidth = ::GetSystemMetrics(SM_CXSCREEN);
    int screenHeight = ::GetSystemMetrics(SM_CYSCREEN);
    if( x>screenWidth || x<0 || y>screenHeight)
    {
        this->RefreshWndAnimationState();
        return;
    }
    ::SetWindowPos(m_hWnd,HWND_TOP,x,y,0,0,SWP_NOSIZE|SWP_NOZORDER);
    UpdateWnd();
}

void WndData::UpdateWnd()
{
    HRESULT hr = 0;
    POINT ptSrc = {0,0};
    if(rotation >= 360 || rotation < 0)
    {
        rotation = 0;
    }
    int index = rotation / 10;
    HDC& srcDCRef = g_ImageCacheDC[index];
    HDC memDC = CreateCompatibleDC(srcDCRef);
    SIZE wndSize = {width, height};
    HBITMAP memBitmap = ::CreateCompatibleBitmap(srcDCRef,width,height);
    HBITMAP oldBitmap = (HBITMAP)::SelectObject(memDC,memBitmap);
    ::SetStretchBltMode (memDC, HALFTONE);
    ::SetBrushOrgEx(memDC, 0, 0, NULL);
    hr = ::StretchBlt(memDC,
                 0,
                 0,
                 width,
                 height,
                 srcDCRef,
                 0,
                 0,
                 SNOW_SIZE,
                 SNOW_SIZE,
                 SRCCOPY
                 );
    if(!hr)
    {
        PHANTOM_MESSAGE(NULL,"Can't Invoke StretchBlt!");
    }
    hr = g_UpdateLayeredWindow(m_hWnd,NULL,NULL,&wndSize,memDC,&ptSrc,0,&blendFunction,2);
    if(!hr)
    {
        PHANTOM_MESSAGE(NULL,"Can't Invoke UpdateLayeredWindow Function!");
    }
    // Clean up staffs
    ::SelectObject(memDC,oldBitmap);
    ::DeleteObject(memBitmap);
    ::DeleteDC(memDC);
}

WndData::~WndData()
{
    ShowWindow(m_hWnd,SW_HIDE);
    ::DestroyWindow(m_hWnd);
    m_hWnd = NULL;
}
