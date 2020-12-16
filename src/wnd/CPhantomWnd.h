#ifndef CPHANTOMWND_H_INCLUDED
#define CPHANTOMWND_H_INCLUDED

#include <windows.h>
#include <vector>
#include <tr1/memory>
#include <tr1/shared_ptr.h>
//#include <boost/shared_ptr.hpp>
#include <tchar.h>

#include "CRandGen.h"
#include "../resource.h"
#include "CGdiPlusBitmap.h"

//using namespace boost;
using namespace std;

#define PHANTOM_WND_CLASS_NAME "PhantomWindow"

#define PHANTOM_MESSAGE(hWnd , Message){\
    MessageBox(hWnd,Message,"#Phantom Message#",MB_OK);\
}

#define SNOW_SIZE   128 //这对于一个雪花已经挺大了

#define IMAGE_CACHE_COUNT 36

struct WndData
{
    WndData();
    HWND        m_hWnd;         // Window Handle
    short       x,y,width,height,offsetX,offsetY,rotation;
    char        lastDir,lastDirCount;
    void RefreshWndAnimationState();
    void UpdateWnd();
    void StepMove(bool moveFlag);
    ~WndData();
};

HRESULT InitEnssentialFunction();

std::shared_ptr<WndData> CreateSnowWindow(
                             HDC* imageCacheDCArray
                             );
std::shared_ptr<WndData> CreateThemeWindow(
                             int x,
                             int y,
                             int width,
                             int height,
                             int rotation = 0
                             );

VOID DestroyTransparintWindow(WndData *pWndData);

LRESULT CALLBACK WindowProcedure (HWND hWnd,UINT message,WPARAM wParam,LPARAM lParam);
LRESULT CALLBACK AboudDialogProcedue(HWND hWnd,UINT message, WPARAM wParam, LPARAM lParam);

HRESULT CleanUpSystem();

HDC& GetImageCacheDC(UINT index);

HDC* GetDCArray();

HRESULT RotateSnowWnd(std::shared_ptr<WndData> pWndData);

VOID SnowThreadProc(LPVOID pParam);



#endif // CPHANTOMWND_H_INCLUDED
