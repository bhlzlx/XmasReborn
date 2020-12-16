#include <windows.h>
#include <gdiplus.h>
#include <tchar.h>
#include <thread>
#include "resource.h"

#include "./wnd/WindowController.h"
// audio player
#include <audioStream.h>
#include <oalStreamSource.h>
#include <io/archieve.h>


char TrayIconMessage[] = "Merry Christmas!";

#define BGMFILE "BGM.OGG"

HINSTANCE g_hInstance;

std::shared_ptr<Nix::openal::Device> device = nullptr;
std::shared_ptr< Nix::openal::Context> context = nullptr;
//Nix::IFile * file = archive->open("pdh_dh_0001.ogg");
Nix::openal::IAudioStream* audioStream = nullptr;
Nix::openal::StreamAudioSource* audioSource = nullptr;
Nix::openal::AudioDecoderService* decodeService = nullptr;


CTimer *pBGMTimer;

struct AudioProc:public CTimerCallback
{
    size_t operator()(CTimerDataObject * pObj)
    {
        Nix::openal::StreamAudioSource* audioSource = (Nix::openal::StreamAudioSource*)pObj->m_pUserData;
        audioSource->tick();
        return 0;
    }
} ogg_callback;

CTimerDataObject *timerDataObj;

//extern HDC g_ImageCacheDC[18];
/*  GDI+ startup token */

int WINAPI WinMain (
    HINSTANCE hThisInstance,
    HINSTANCE hPrevInstance,
    LPSTR lpszArgument,
    int nCmdShow)
{
    /**/
    HANDLE hMutex = CreateMutex(NULL,TRUE,"PhantomMerry");
    if(hMutex)
    {
        if(ERROR_ALREADY_EXISTS == GetLastError())
        {
            MessageBoxW(NULL,L"Only one instance of the application was allowed.",L"CLANNAD after story ~",MB_OK);
            return 0;
        }
    }

//    pthread_win32_process_attach_np();

    g_hInstance = hThisInstance;
    MSG messages;            /* Here messages to the application are saved */

    WindowController controller;
    controller.InitWindowController();

    // unpack embed audio file
    HANDLE hFile = ::CreateFile(BGMFILE, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_TEMPORARY, NULL);
	if (hFile == INVALID_HANDLE_VALUE)
		return false;
	HRSRC hRes =  FindResource(hThisInstance,"IDR_OGG_AUDIO","OGG");
	HGLOBAL musicFileHandle = ::LoadResource(hThisInstance,hRes);
	DWORD	resSize = ::SizeofResource(hThisInstance,hRes);
    char * str = (char *)LockResource(musicFileHandle);
    // DWORD bytesWriten;
	// WriteFile(hFile,musicFileHandle,resSize,&bytesWriten,NULL);
    auto oggBuffer = Nix::CreateMemoryBuffer(resSize);
    oggBuffer->write(resSize,str);
    oggBuffer->seek( Nix::SeekFlag::SeekSet, 0 );
    UnlockResource(musicFileHandle);
	CloseHandle(hFile);

    device = Nix::openal::GetDevice(nullptr);
	context = device->CreateContext(nullptr);
	context->MakeCurrent();
	//Nix::IFile * file = archive->open("pdh_dh_0001.ogg");
	audioStream = Nix::openal::IAudioStream::FromOGG(oggBuffer);
	audioSource = new Nix::openal::StreamAudioSource();
	decodeService = new Nix::openal::AudioDecoderService();
	decodeService->initialize();
	audioSource->initialize(audioStream, decodeService);
	audioSource->play();
	// while (true) {
	// 	audioSource->tick();
	// 	auto mTime = std::chrono::milliseconds(2);
	// 	std::this_thread::sleep_for(mTime);
	// }
    timerDataObj = CTimer::GetTimerInstance()->CreateTimer(10,&ogg_callback,*controller.GetThemeWindow(),audioSource);

    /* Run the message loop. It will run until GetMessage() returns 0 */
    while (GetMessage (&messages, NULL, 0, 0))
    {
        /* Translate virtual-key messages into character messages */
        TranslateMessage(&messages);
        /* Send message to WindowProcedure */
        DispatchMessage(&messages);
    }
    /* The program return-value is 0 - The value that PostQuitMessage() gave */
    CTimer::GetTimerInstance()->KillTimer(timerDataObj);
    // deinit audio resources

    Sleep(20);


    return messages.wParam;
}
