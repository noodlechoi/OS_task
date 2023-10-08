#include <iostream>
#include<windows.h>

int main(int argc, char* argv[])
{
    STARTUPINFO si[2];
    // 이전 프로세스 정보를 저장하는 구조체
    PROCESS_INFORMATION pi[2];
    HANDLE hProc[2];
    DWORD exit = 0, ret = 0;

    // 구조체 초기화
    for (int i = 0; i < 2; ++i) {
        ZeroMemory(&si[i], sizeof(si[i]));
        ZeroMemory(&pi[i], sizeof(pi[i]));
        si[i].cb = sizeof(si[i]);
    }

    TCHAR name1[] = TEXT("notepad 1");
    TCHAR name2[] = TEXT("notepad 2");

    // 프로세스 생성
    if (!CreateProcess(NULL, name1, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[0], &pi[0])) {
        std::cout << "fail create process" << std::endl;
        return -1;
    }
    if (!CreateProcess(NULL, name2, NULL, NULL, FALSE, CREATE_NEW_CONSOLE, NULL, NULL, &si[1], &pi[1])) {
        std::cout << "fail create process" << std::endl;
        return -1;
    }

    // 핸들 값
    for (int i = 0; i < 2; ++i)
        hProc[i] = pi[i].hProcess;

    while (1) {
        // 두 프로세스 모두 종료할 때까지 기다리기
        ret = WaitForMultipleObjects(2, hProc, TRUE, INFINITE);

        if (ret == WAIT_OBJECT_0) {
            if (GetExitCodeProcess(hProc[0], &exit))
                std::cout << "exit code : " << exit << std::endl;
            else
                std::cout << "error code : " << exit << std::endl;
            break;
        }
    }

    for (int i = 0; i < 2; ++i) {
        CloseHandle(pi[i].hProcess);
        CloseHandle(pi[i].hThread);
    }

    return 0;
}