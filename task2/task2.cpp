#include<stdio.h>
#include<windows.h>
#include<tchar.h>

int main(int argc, TCHAR* argv[])
{
    // 구조체 초기화
    STARTUPINFO si = { 0, };
    // 이전 프로세스 정보를 저장하는 구조체
    PROCESS_INFORMATION pi;

    si.cb = sizeof(si);
    si.dwFlags = STARTF_USEPOSITION | STARTF_USESIZE;
    si.dwX = 100;
    si.dwY = 100;
    si.dwXSize = 300;
    si.dwYSize = 300;

    TCHAR command[] = _T("notepad 1");

    BOOL state;

    state = CreateProcess(
        NULL,
        command,
        NULL, NULL,
        TRUE,
        CREATE_NEW_CONSOLE,
        NULL, NULL,
        &si,
        &pi
    );

    if (state != 0) {
        _fputts(_T("Creation OK! \n"), stdout);
    }
    else {
        _fputts(_T("Creation Fail! \n"), stdout);
    }

    return 0;
}