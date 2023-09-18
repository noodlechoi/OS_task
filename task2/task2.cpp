#include <stdio.h>
#include <windows.h>

int main(int argc, char* argv[])
{
    DWORD dwExitCode = 0, dwRet = 0;
    HANDLE hProc[2];
    STARTUPINFO si;
    PROCESS_INFORMATION pi;

    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    //  Child Process 실행
    TCHAR command[] = TEXT("notepad 1");
    if (!CreateProcess(NULL, command, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        printf("fail to execute process \n");
        return-1;
    }

    if (GetExitCodeProcess(pi.hProcess, &dwExitCode))
    {
        //   process가 살아 있는 경우, dwExitCode의 값은 STILL_ACTIVE 입니다.

        if (dwExitCode == STILL_ACTIVE)
            printf("process is alive\n");
        else
            printf("exit status(code) : %d\n", dwExitCode);
    }
    else
    {
        printf("errcode : %d\n", GetLastError());
    }

    hProc[0] = pi.hProcess;
    dwRet = WaitForMultipleObjects(1, hProc, FALSE, INFINITE);

    //  WaitForMultipleObjects()의 반환값은 시그널된 객체(Process, Thread)의 ID 값이며,
    // 다음과 같은 형태로 어느 객체가 시그널 되어 있는지 확인이 가능합니다.

    if (dwRet == WAIT_OBJECT_0 + 0)
    {

        // 자~~ 이제 Child Process가 종료시에 반환한 값을 확인 !!

        if (GetExitCodeProcess(*(hProc + 0), &dwExitCode))
        {
            printf("exit status(code) : %d\n", dwExitCode);
        }
        else
        {
            printf("errcode : %d\n", GetLastError());
        }
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    return 0;
}