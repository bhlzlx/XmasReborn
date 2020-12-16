#ifndef TRAYICON_H
#define TRAYICON_H
#include <windows.h>

#define ID_TRAYICON_UID 0x1001
#define ID_TRAYICON_MESSAGE 0x00f0

class TrayIcon
{
    public:
        static UINT TRAYICON_MESSAGE;
        TrayIcon(
                   HWND hWnd,
                   HICON hIcon,
                   LPSTR lpsTip
                   );
        virtual ~TrayIcon();
        BOOL Create();
        BOOL ShowMessage(LPCSTR);
    protected:
        NOTIFYICONDATA data;
    private:
};

#endif // TRAYICON_H
