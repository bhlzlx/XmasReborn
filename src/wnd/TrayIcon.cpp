#include "TrayIcon.h"

UINT TrayIcon::TRAYICON_MESSAGE = ID_TRAYICON_MESSAGE;

TrayIcon::TrayIcon(
                   HWND hWnd,
                   HICON hIcon,
                   LPSTR lpsTip
                   )
{
    //ctor
    ZeroMemory(&this->data,sizeof(NOTIFYICONDATA));
    data.cbSize = sizeof(NOTIFYICONDATA);
    data.hIcon = hIcon;
    data.hWnd = hWnd;
    strcpy(data.szTip,lpsTip);
    data.uCallbackMessage = TrayIcon::TRAYICON_MESSAGE;
    data.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    data.uID = ID_TRAYICON_UID;
}

BOOL TrayIcon::Create()
{
    return Shell_NotifyIcon(NIM_ADD,&this->data);
}

BOOL TrayIcon::ShowMessage(LPCSTR msg)
{
//    NOTIFYICONDATA infoData;
//    ZeroMemory(&infoData,sizeof(NOTIFYICONDATA));
//    strcpy(infoData.szTip,msg);
//    infoData.cbSize = sizeof(NOTIFYICONDATA);
//    infoData.uFlags = NIF_INFO;
//    strcpy(infoData.szInfo,msg);
//    strcpy(infoData.szInfoTitle,"ίχ!");
    return TRUE;
}

TrayIcon::~TrayIcon()
{
    //dtor
    Shell_NotifyIcon(NIM_DELETE,&this->data);
}
