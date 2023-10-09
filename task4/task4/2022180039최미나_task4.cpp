#include <iostream>
#include <fstream>
#include <queue>

using namespace std;

const int NumberOfCPU = 1;

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
	ProcessInput() : opened(false), number_of_process(0), end_of_process(false), have_process(false), arrive_time(0), run_length(0), fin("data.txt", ios::in)
	{
	}
	bool GetProcess(int current_time, int* size,  bool* end_of_schedule)
	{
		if (false == opened) {
			if (!fin) {
				cerr << "Failed to open data.txt!\n";
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

// �켱 ���� �� ����ü
struct CompareLength {
	bool operator()(const PCB* left, const PCB* right) {
		return left->length > right->length;
	}
};

// �ܿ��ð� �� ����ü
struct CompareRemainingTime {
	bool operator()(const PCB* left, const PCB* right) {
		return left->length - left->executed_Length > right->length - right->executed_Length;
	}
};

int current_time;


void FIFO()
{
	int current_time = 0;
	queue <PCB*> ready_queue;
	PCB* current_process = nullptr;
	int total_return_time = 0;
	int total_process = 0;
	int total_response_time = 0;
	int pid = 1;

	ProcessInput p_in;

	for (;; current_time++) {
		int length;
		bool end_of_schedule;

		// ���� �ð��� ������ ���μ������� ť�� �߰�
		while (true == p_in.GetProcess(current_time, &length, &end_of_schedule))
		{
			ready_queue.push(new PCB{ pid++, current_time, length, 0 });
		}

		// ���� ���� ���� ���μ����� �Ϸ�Ǿ��� ��� ó��
		if (nullptr != current_process)
		{
			if (current_process->executed_Length >= current_process->length) {
				total_return_time += current_time - current_process->arrive_time;
				total_process++;
				delete current_process;
				current_process = nullptr;
			}
		}

		// ���� ���� ���� ���� ���μ����� ����, �غ� ť�� ������� �ʴٸ�, ���ο� ���μ����� ���� ��� ����
		if (nullptr == current_process)
		{
			if (false == ready_queue.empty())
			{
				current_process = ready_queue.front();
				ready_queue.pop();
				if (0 == current_process->executed_Length)
					total_response_time += current_time - current_process->arrive_time;
			}
		}

		// �����ٸ��� ����Ǿ��� ��� �ݺ����� Ż��
		if ((true == end_of_schedule) && ready_queue.empty() && (nullptr == current_process))  break;

		if (nullptr != current_process) {
			current_process->executed_Length++;
		}
	}

	// ���
	cout << "\n\n===  Scheduling Algorithm : FIFO  ===\n";
	cout << "Number of CPU = " << NumberOfCPU << endl;
	cout << "Total Execution Time = " << current_time << endl;
	cout << "Number of Processes Executed = " << total_process << endl;
	cout << "Average Return Time = " << total_return_time / total_process << endl;
	cout << "Average Response Time = " << total_response_time / total_process << endl;
}

void SJF()	// ���� ª�� �� ����
{
	int current_time = 0;
	// �켱���� ť
	priority_queue<PCB*, vector<PCB*>, CompareLength> ready_queue;
	PCB* current_process = nullptr;
	int total_return_time = 0;
	int total_process = 0;
	int total_response_time = 0;
	int pid = 1;

	ProcessInput p_in;

	for (;; current_time++) {
		int length;
		bool end_of_schedule;

		// ���� �ð��� ������ ���μ������� ť�� �߰�
		while (true == p_in.GetProcess(current_time, &length, &end_of_schedule))
		{
			ready_queue.push(new PCB{ pid++, current_time, length, 0 });
		}

		// ���� ���� ���� ���μ����� �Ϸ�Ǿ��� ��� ó��
		if (nullptr != current_process)
		{
			if (current_process->executed_Length >= current_process->length) {
				total_return_time += current_time - current_process->arrive_time;
				total_process++;
				delete current_process;
				current_process = nullptr;
			}
		}

		// ���� ���� ���� ���� ���μ����� ����, �غ� ť�� ������� �ʴٸ�, ���ο� ���μ����� ���� ��� ���� (SJF �˰���)
		if (nullptr == current_process)
		{
			if (false == ready_queue.empty())
			{
				// front -> top
				current_process = ready_queue.top();
				ready_queue.pop();
				if (0 == current_process->executed_Length)
					total_response_time += current_time - current_process->arrive_time;
			}
		}

		// �����ٸ��� ����Ǿ��� ��� �ݺ����� Ż��
		if ((true == end_of_schedule) && ready_queue.empty() && (nullptr == current_process))  break;

		if (nullptr != current_process) {
			current_process->executed_Length++;
		}
	}

	cout << "\n\n===  �����ٸ� �˰��� : SJF (Shortest Job First)  ===\n";
	cout << "Number of CPU = " << NumberOfCPU << endl;
	cout << "Total Execution Time = " << current_time << endl;
	cout << "Number of Processes Executed = " << total_process << endl;
	cout << "Average Return Time = " << total_return_time / total_process << endl;
	cout << "Average Response Time = " << total_response_time / total_process << endl;
}

void STCF()	// SJF, �ܿ��ð��� �����Ͽ� ���� ���� �۾� ������, �߰��� �۾��� �ߴܵ�
{
	int current_time = 0;
	// �켱���� ť
	priority_queue<PCB*, vector<PCB*>, CompareRemainingTime> ready_queue;
	PCB* current_process = nullptr;
	int total_return_time = 0;
	int total_process = 0;
	int total_response_time = 0;
	int pid = 1;

	ProcessInput p_in;

	for (;; current_time++) {
		int length;
		bool end_of_schedule;

		// ���� �ð��� ������ ���μ������� ť�� �߰�
		while (true == p_in.GetProcess(current_time, &length, &end_of_schedule))
		{
			// �ߴ�
			// ���� ���� ���� ���μ����� �ܿ� �ð��� ���� ������ ���μ����� ���� �ð����� �� Ŭ ���
			if (nullptr != current_process && current_process->length - current_process->executed_Length > length) {
				// ť�� �߰�
				ready_queue.push(new PCB{ current_process->PID, current_time, current_process->length - current_process->executed_Length, current_process->executed_Length });
				// ���� ���� ���� ���μ����� ����
				delete current_process;
				current_process = nullptr;
			}
			ready_queue.push(new PCB{ pid++, current_time, length, 0 });
		}

		// ���� ���� ���� ���μ����� �Ϸ�Ǿ��� ��� ó��
		if (nullptr != current_process)
		{
			if (current_process->executed_Length >= current_process->length) {
				total_return_time += current_time - current_process->arrive_time;
				total_process++;
				delete current_process;
				current_process = nullptr;
			}
		}

		// ���� ���� ���� ���� ���μ����� ����, �غ� ť�� ������� �ʴٸ�, ���ο� ���μ����� ���� ��� ����
		if (nullptr == current_process)
		{
			if (false == ready_queue.empty())
			{
				current_process = ready_queue.top();
				ready_queue.pop();
				if (0 == current_process->executed_Length)
					total_response_time += current_time - current_process->arrive_time;
			}
		}

		// �����ٸ��� ����Ǿ��� ��� �ݺ����� Ż��
		if ((true == end_of_schedule) && ready_queue.empty() && (nullptr == current_process))  break;

		if (nullptr != current_process) {
			current_process->executed_Length++;
		}
	}

	cout << "\n\n===  �����ٸ� �˰��� : STCF  ===\n";
	cout << "Number of CPU = " << NumberOfCPU << endl;
	cout << "Total Execution Time = " << current_time << endl;
	cout << "Number of Processes Executed = " << total_process << endl;
	cout << "Average Return Time = " << total_return_time / total_process << endl;
	cout << "Average Response Time = " << total_response_time / total_process << endl;
}

void RR()	// timeslice = 2�� �ð��� ������ ���� �۾����� ���� ��ȯ
{
	const int time_slice = 2;
	int current_time = 0;
	queue <PCB*> ready_queue;
	PCB* current_process = nullptr;
	int total_return_time = 0;
	int total_process = 0;
	int total_response_time = 0;
	int pid = 1;

	ProcessInput p_in;

	for (;; current_time++) {
		int length;
		bool end_of_schedule;

		// ���� �ð��� ������ ���μ������� ť�� �߰�
		while (true == p_in.GetProcess(current_time, &length, &end_of_schedule))
		{
			// ����� �ð��� timeslice�� �Ǹ� �ߴ�
			if (nullptr != current_process && current_process->executed_Length >= time_slice) {
				// ť�� �߰�
				ready_queue.push(new PCB{ current_process->PID, current_time, current_process->length - current_process->executed_Length, current_process->executed_Length });
				// ���� ���� ���� ���μ����� ����
				delete current_process;
				current_process = nullptr;
			}
			ready_queue.push(new PCB{ pid++, current_time, length, 0 });
		}

		// ���� ���� ���� ���μ����� �Ϸ�Ǿ��� ��� ó��
		if (nullptr != current_process)
		{
			if (current_process->executed_Length >= current_process->length) {
				total_return_time += current_time - current_process->arrive_time;
				total_process++;
				delete current_process;
				current_process = nullptr;
			}
		}

		// ���� ���� ���� ���� ���μ����� ����, �غ� ť�� ������� �ʴٸ�, ���ο� ���μ����� ���� ��� ����
		if (nullptr == current_process)
		{
			if (false == ready_queue.empty())
			{
				current_process = ready_queue.front();
				ready_queue.pop();
				if (0 == current_process->executed_Length)
					total_response_time += current_time - current_process->arrive_time;
			}
		}

		// �����ٸ��� ����Ǿ��� ��� �ݺ����� Ż��
		if ((true == end_of_schedule) && ready_queue.empty() && (nullptr == current_process))  break;

		if (nullptr != current_process) {
			current_process->executed_Length++;
		}
	}

	// ���
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
