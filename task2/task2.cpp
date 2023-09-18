#include<stdio.h>
#include<windows.h>

int main(int argc, char* argv[])
{
    STARTUPINFO si;
    // 이전 프로세스 정보를 저장하는 구조체
    PROCESS_INFORMATION pi;
    HANDLE hProc;
    DWORD exit = 0, ret = 0;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    TCHAR command[] = TEXT("notepad 1");

    // 프로세스 생성
    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        printf("fail create process\n");
        return -1;
    }

    // 종료했는지 확인
    if (GetExitCodeProcess(pi.hProcess, &exit)) {
        if (exit == STILL_ACTIVE)
            printf("process is alive\n");
        else
            printf("exit status(code) : %d\n", exit);
    }
    else {
        printf("errcode : %d\n", GetLastError());
    }

    hProc = pi.hProcess;
    ret = WaitForSingleObject(hProc, INFINITE);

    if (ret == WAIT_OBJECT_0) {
        if (GetExitCodeProcess(hProc, &exit))
            printf("exit status(code) : %d\n", exit);
        else
            printf("errcode : %d\n", GetLastError());
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return 0;
}