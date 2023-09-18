#include <iostream>
#include<windows.h>

int main(int argc, char* argv[])
{
    STARTUPINFO si;
    // 이전 프로세스 정보를 저장하는 구조체
    PROCESS_INFORMATION pi;
    HANDLE hProc[2];
    DWORD exit = 0, ret = 0;

    // 구조체 초기화
    ZeroMemory(&si, sizeof(si));
    ZeroMemory(&pi, sizeof(pi));
    si.cb = sizeof(si);

    TCHAR name1[] = TEXT("notepad 1");
    TCHAR name2[] = TEXT("notepad 2");

    // 프로세스 생성
    if (!CreateProcess(NULL, name1, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        std::cout << "fail create process" << std::endl;
        return -1;
    }
    if (!CreateProcess(NULL, name2, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        std::cout << "fail create process" << std::endl;
        return -1;
    }

    for (int i = 0; i < 2; ++i)
        hProc[i] = pi.hProcess;
    // 종료할 때까지 기다리기
    ret = WaitForMultipleObjects(2, hProc, FALSE, INFINITE);

    if (ret == WAIT_OBJECT_0) {
        if (GetExitCodeProcess(hProc[0], &exit))
            std::cout << "exit status(code) : " << exit << std::endl;
        else
            std::cout << "errcode : " << exit << std::endl;
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}