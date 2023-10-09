#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

const int NumberOfCPU = 4;

struct PCB {
public:
	int PID;
	int arrive_time;
	int length;
	int executed_Length;
};

class ProcessInput {
private:
	bool opened = false;
	int number_of_process = 0;
	bool end_of_process = false;
	bool have_process = false;
	int arrive_time = 0;
	int run_length = 0;
	ifstream fin;
public:
	ProcessInput() : opened(false), number_of_process(0), end_of_process(false), have_process(false), arrive_time(0), run_length(0), fin("data4.txt", ios::in)
	{
	}
	bool GetProcess(int current_time, int* size, bool* end_of_schedule)
	{
		if (false == opened) {
			if (!fin) {
				cerr << "Failed to open data4.txt!\n";
				exit(1);
			}
		}

		if (true == end_of_process) {
			*size = 0;
			*end_of_schedule = true;
			return false;
		}

		if (false == have_process) {
			fin >> arrive_time >> run_length;
			have_process = true;
		}

		if (arrive_time > current_time) {
			*size = 0;
			*end_of_schedule = false;
			return false;
		}

		if (-1 == arrive_time) {
			end_of_process = true;
			*end_of_schedule = true;
			return false;
		}

		*size = run_length;
		*end_of_schedule = false;
		have_process = false;
		return true;
	}
};

// 우선 순위 비교 구조체
struct CompareLength {
	bool operator()(const PCB* left, const PCB* right) {
		return left->length > right->length;
	}
};

// 잔여시간 비교 구조체
struct CompareRemainingTime {
	bool operator()(const PCB* left, const PCB* right) {
		return left->length - left->executed_Length > right->length - right->executed_Length;
	}
};

int current_time;


void FIFO()
{
	int current_time = 0;
	//queue <PCB*> ready_queue;
	// CPU queue
	queue <PCB*> CPU[NumberOfCPU];
	int cpu_idx = 0;
	PCB* current_process = nullptr;
	int total_return_time = 0;
	int total_process = 0;
	int total_response_time = 0;
	int pid = 1;

	ProcessInput p_in;

	for (;; current_time++) {
		int length;
		bool end_of_schedule;

		// 현재 시간에 도착한 프로세스들을 큐에 추가
		while (true == p_in.GetProcess(current_time, &length, &end_of_schedule))
		{
			CPU[cpu_idx % NumberOfCPU].push(new PCB{pid++, current_time, length, 0});
			cpu_idx++;
		}

		// 현재 실행 중인 프로세스가 완료되었을 경우 처리
		if (nullptr != current_process)
		{
			if (current_process->executed_Length >= current_process->length) {
				total_return_time += current_time - current_process->arrive_time;
				total_process++;
				delete current_process;
				current_process = nullptr;
			}
		}

		// 만약 현재 실행 중인 프로세스가 없고, 준비 큐가 비어있지 않다면, 새로운 프로세스를 실행 대기 상태
		if (nullptr == current_process)
		{
			for (int i = 0; i < NumberOfCPU; ++i) {
				if (false == CPU[i].empty())
				{
					current_process = CPU[i].front();
					CPU[i].pop();
					if (0 == current_process->executed_Length)
						total_response_time += current_time - current_process->arrive_time;
				}
			}
		}

		// 스케줄링이 종료되었을 경우 반복문을 탈출
		if ((true == end_of_schedule) && (nullptr == current_process)) {
			// CPU 하나라도 비어있지 않으면 탈출 X
			for (int i = 0; i < NumberOfCPU; ++i) {
				if (!CPU[i].empty())	end_of_schedule = false;
			}
			if (true == end_of_schedule)
				break;
		}

		if (nullptr != current_process) {
			current_process->executed_Length++;
		}
	}

	// 출력
	cout << "\n\n===  Scheduling Algorithm : FIFO  ===\n";
	cout << "Number of CPU = " << NumberOfCPU << endl;
	cout << "Total Execution Time = " << current_time << endl;
	cout << "Number of Processes Executed = " << total_process << endl;
	cout << "Average Return Time = " << total_return_time / total_process << endl;
	cout << "Average Response Time = " << total_response_time / total_process << endl;
}

void SJF()	// 가장 짧은 것 부터
{
	int current_time = 0;
	// 우선순위 큐
	// priority_queue<PCB*, vector<PCB*>, CompareLength> ready_queue;
	priority_queue<PCB*, vector<PCB*>, CompareLength> CPU[NumberOfCPU];
	int cpu_idx = 0;
	PCB* current_process = nullptr;
	int total_return_time = 0;
	int total_process = 0;
	int total_response_time = 0;
	int pid = 1;

	ProcessInput p_in;

	for (;; current_time++) {
		int length;
		bool end_of_schedule;

		// 현재 시간에 도착한 프로세스들을 큐에 추가
		while (true == p_in.GetProcess(current_time, &length, &end_of_schedule))
		{
			CPU[cpu_idx % NumberOfCPU].push(new PCB{pid++, current_time, length, 0});
			cpu_idx++;
		}

		// 현재 실행 중인 프로세스가 완료되었을 경우 처리
		if (nullptr != current_process)
		{
			if (current_process->executed_Length >= current_process->length) {
				total_return_time += current_time - current_process->arrive_time;
				total_process++;
				delete current_process;
				current_process = nullptr;
			}
		}

		// 만약 현재 실행 중인 프로세스가 없고, 준비 큐가 비어있지 않다면, 새로운 프로세스를 실행 대기 상태 (SJF 알고리즘)
		if (nullptr == current_process)
		{
			for (int i = 0; i < NumberOfCPU; ++i) {
				if (false == CPU[i].empty())
				{
					// front -> top
					current_process = CPU[i].top();
					CPU[i].pop();
					if (0 == current_process->executed_Length)
						total_response_time += current_time - current_process->arrive_time;
				}
			}
		}

		// 스케줄링이 종료되었을 경우 반복문을 탈출
		if ((true == end_of_schedule) && (nullptr == current_process)) {
			// CPU 하나라도 비어있지 않으면 탈출 X
			for (int i = 0; i < NumberOfCPU; ++i) {
				if (!CPU[i].empty())	end_of_schedule = false;
			}
			if (true == end_of_schedule)
				break;
		}

		if (nullptr != current_process) {
			current_process->executed_Length++;
		}
	}

	cout << "\n\n===  스케줄링 알고리즘 : SJF (Shortest Job First)  ===\n";
	cout << "Number of CPU = " << NumberOfCPU << endl;
	cout << "Total Execution Time = " << current_time << endl;
	cout << "Number of Processes Executed = " << total_process << endl;
	cout << "Average Return Time = " << total_return_time / total_process << endl;
	cout << "Average Response Time = " << total_response_time / total_process << endl;
}

void STCF()	// SJF, 잔여시간을 조사하여 가장 작은 작업 스케쥴, 중간에 작업이 중단됨
{
	int current_time = 0;
	// 우선순위 큐
	//priority_queue<PCB*, vector<PCB*>, CompareRemainingTime> ready_queue;
	priority_queue<PCB*, vector<PCB*>, CompareLength> CPU[NumberOfCPU];
	int cpu_idx = 0;
	PCB* current_process = nullptr;
	int total_return_time = 0;
	int total_process = 0;
	int total_response_time = 0;
	int pid = 1;

	ProcessInput p_in;

	for (;; current_time++) {
		int length;
		bool end_of_schedule;

		// 현재 시간에 도착한 프로세스들을 큐에 추가
		while (true == p_in.GetProcess(current_time, &length, &end_of_schedule))
		{
			// 중단
			// 현재 실행 중인 프로세스의 잔여 시간이 새로 도착한 프로세스의 실행 시간보다 더 클 경우
			if (nullptr != current_process && current_process->length - current_process->executed_Length > length) {
				// 큐에 추가
				CPU[cpu_idx % NumberOfCPU].push(new PCB{current_process->PID, current_time, current_process->length - current_process->executed_Length, current_process->executed_Length});
				// 현재 실행 중인 프로세스를 중지
				delete current_process;
				current_process = nullptr;
			}
			CPU[cpu_idx % 4].push(new PCB{ pid++, current_time, length, 0 });
			cpu_idx++;
		}

		// 현재 실행 중인 프로세스가 완료되었을 경우 처리
		if (nullptr != current_process)
		{
			if (current_process->executed_Length >= current_process->length) {
				total_return_time += current_time - current_process->arrive_time;
				total_process++;
				delete current_process;
				current_process = nullptr;
			}
		}

		// 만약 현재 실행 중인 프로세스가 없고, 준비 큐가 비어있지 않다면, 새로운 프로세스를 실행 대기 상태
		if (nullptr == current_process)
		{
			for (int i = 0; i < NumberOfCPU; ++i) {
				if (false == CPU[i].empty())
				{
					// front -> top
					current_process = CPU[i].top();
					CPU[i].pop();
					if (0 == current_process->executed_Length)
						total_response_time += current_time - current_process->arrive_time;
				}
			}
		}

		// 스케줄링이 종료되었을 경우 반복문을 탈출
		if ((true == end_of_schedule) && (nullptr == current_process)) {
			// CPU 하나라도 비어있지 않으면 탈출 X
			for (int i = 0; i < NumberOfCPU; ++i) {
				if (!CPU[i].empty())	end_of_schedule = false;
			}
			if (true == end_of_schedule)
				break;
		}

		if (nullptr != current_process) {
			current_process->executed_Length++;
		}
	}

	cout << "\n\n===  스케줄링 알고리즘 : STCF  ===\n";
	cout << "Number of CPU = " << NumberOfCPU << endl;
	cout << "Total Execution Time = " << current_time << endl;
	cout << "Number of Processes Executed = " << total_process << endl;
	cout << "Average Return Time = " << total_return_time / total_process << endl;
	cout << "Average Response Time = " << total_response_time / total_process << endl;
}

void RR()	// timeslice = 2의 시간이 지나면 다음 작업으로 문맥 교환
{
	const int time_slice = 2;
	int current_time = 0;
	// queue <PCB*> ready_queue;
	queue <PCB*> CPU[NumberOfCPU];
	int cpu_idx = 0;
	PCB* current_process = nullptr;
	int total_return_time = 0;
	int total_process = 0;
	int total_response_time = 0;
	int pid = 1;

	ProcessInput p_in;

	for (;; current_time++) {
		int length;
		bool end_of_schedule;

		// 현재 시간에 도착한 프로세스들을 큐에 추가
		while (true == p_in.GetProcess(current_time, &length, &end_of_schedule))
		{
			// 실행된 시간이 timeslice가 되면 중단
			if (nullptr != current_process && current_process->executed_Length >= time_slice) {
				// 큐에 추가
				CPU[cpu_idx % NumberOfCPU].push(new PCB{current_process->PID, current_time, current_process->length - current_process->executed_Length, current_process->executed_Length});
				// 현재 실행 중인 프로세스를 중지
				delete current_process;
				current_process = nullptr;
			}
			CPU[cpu_idx % NumberOfCPU].push(new PCB{ pid++, current_time, length, 0 });
			cpu_idx++;
		}

		// 현재 실행 중인 프로세스가 완료되었을 경우 처리
		if (nullptr != current_process)
		{
			if (current_process->executed_Length >= current_process->length) {
				total_return_time += current_time - current_process->arrive_time;
				total_process++;
				delete current_process;
				current_process = nullptr;
			}
		}

		// 만약 현재 실행 중인 프로세스가 없고, 준비 큐가 비어있지 않다면, 새로운 프로세스를 실행 대기 상태
		if (nullptr == current_process)
		{
			for (int i = 0; i < NumberOfCPU; ++i) {
				if (false == CPU[i].empty())
				{
					current_process = CPU[i].front();
					CPU[i].pop();
					if (0 == current_process->executed_Length)
						total_response_time += current_time - current_process->arrive_time;
				}
			}
		}

		// 스케줄링이 종료되었을 경우 반복문을 탈출
		if ((true == end_of_schedule) && (nullptr == current_process)) {
			// CPU 하나라도 비어있지 않으면 탈출 X
			for (int i = 0; i < NumberOfCPU; ++i) {
				if (!CPU[i].empty())	end_of_schedule = false;
			}
			if (true == end_of_schedule)
				break;
		}

		if (nullptr != current_process) {
			current_process->executed_Length++;
		}
	}

	// 출력
	cout << "\n\n===  Scheduling Algorithm : RR  ===\n";
	cout << "Number of CPU = " << NumberOfCPU << endl;
	cout << "Total Execution Time = " << current_time << endl;
	cout << "Number of Processes Executed = " << total_process << endl;
	cout << "Average Return Time = " << total_return_time / total_process << endl;
	cout << "Average Response Time = " << total_response_time / total_process << endl;
}

int main()
{
	FIFO();
	SJF();
	STCF();
	RR();
}
