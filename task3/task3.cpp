#include <stdio.h>
#include <thread>
#include <chrono>

int main() {
    const int num = 100;
    long long total = 0;

    for (int i = 0; i < num; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        printf("Hello World.\n");
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        total += elapsed_time;
    }

    double average_time = static_cast<double>(total) / num;
    printf("평균 실행 시간: %.1lf nsec\n", average_time);

    return 0;
}
