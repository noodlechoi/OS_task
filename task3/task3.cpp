#include <iostream>
#include <thread>
#include <chrono>

int main()
{
	std::chrono::high_resolution_clock::time_point t1, t2;
	std::chrono::duration<double> time_span;
	for (int i = 0; i < 100; ++i) {
		t1 = std::chrono::high_resolution_clock::now();

		std::this_thread::yield();

		t2 = std::chrono::high_resolution_clock::now();
		 += std::chrono::duration_cast<std::chrono::duration<double>> (t2 - t1);
	}


	std::cout << time_span.count() << std::endl;
}
