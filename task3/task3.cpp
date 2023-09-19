#include <iostream>
#include <thread>
#include <chrono>

int main() {
    const int num = 100;
    long long total = 0;

    for (int i = 0; i < num; ++i) {
        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::yield();
        auto end = std::chrono::high_resolution_clock::now();

        auto elapsed_time = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();
        total += elapsed_time;
    }

    double average_time = static_cast<double>(total) / num;
    std::cout << "평균 실행 시간: " << average_time << " nsec" << std::endl;

    return 0;
}
