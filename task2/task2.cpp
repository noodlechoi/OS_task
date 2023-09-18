#include<stdio.h>
#include<windows.h>

int CreateNotePad(TCHAR name[])
{
    STARTUPINFO si;
    // 이전 프로세스 정보를 저장하는 구조체
    PROCESS_INFORMATION pi;
    HANDLE hProc;
    DWORD exit = 0, ret = 0;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);

    // 프로세스 생성
    if (!CreateProcess(NULL, name, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        printf("fail create process\n");
        return -1;
    }

    hProc = pi.hProcess;
    // 종료할 때까지 기다리기
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

int main(int argc, char* argv[])
{
    TCHAR name1[] = TEXT("notepad 1");
    TCHAR name2[] = TEXT("notepad 2");
    CreateNotePad(name1);
    CreateNotePad(name2);
}