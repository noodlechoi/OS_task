#include <iostream>
#include <fstream>
#include <queue>
#include <list>

using namespace std;

constexpr int MEMORY_SIZE = 512;

class MEMORY_REQUEST {

	struct MCB {
	public:
		int id;
		int arrive_time;
		int size;
		int use_time;
	};

	queue<MCB> m_mr;

public:
	MEMORY_REQUEST()
	{
		ifstream fin("memdata.txt", ios::in);
		if (fin.fail()) {
			char buff[200];
			strerror_s(buff, 200, errno);
			cout << "Error Opening file : " << buff;
			exit(-1);
		}
		for (int i = 0;; ++i) {
			if (true == fin.eof()) break;
			MCB mcb;
			mcb.id = i;
			fin >> mcb.arrive_time >> mcb.size >> mcb.use_time;
			if (-1 == mcb.arrive_time) break;
			m_mr.push(mcb);
		}
	}

	bool GetMemoryRequest(int current_time, int* size, int* use_time, bool* end_of_schedule)
	{
		*end_of_schedule = m_mr.empty();
		if (true == *end_of_schedule) return false;

		MCB& top = m_mr.front();
		if (top.arrive_time > current_time) return false;
		*size = top.size;
		*use_time = top.use_time;
		m_mr.pop();
		return true;
	}
};

struct ALLOCATED_MEMORY {
	int alloc_time;
	int start_address;
	int size;
	int end_time;
	int arrive_time;
	bool operator < (const ALLOCATED_MEMORY& rhs) const
	{
		return end_time > rhs.end_time;
	}
};

struct UNALLOCATED_REQUEST {
	int arrive_time;
	int size;
	int use_time;
};

struct FREE_NODE {
	int start_address;
	int size;
};
int try_alloc_FirstFit(list <FREE_NODE>& free_list, int size)
{
	for (auto i = free_list.begin(); i != free_list.end();) {
		if (i->size >= size) {
			int start_address = i->start_address;
			if (i->size == size)
				free_list.erase(i);
			else { // i->size > size 
				i->size = i->size - size;
				i->start_address = i->start_address + size;
			}
			return start_address;
		}
		++i;
	}
	return -1;
}

void Return_Memory(list <FREE_NODE>& free_list, int address, int size)
{
	auto i = free_list.begin();

	// 반환될 위치 검색)
	while (i != free_list.end()) {
		if (i->start_address > address) break;
		else ++i;
	}
	free_list.insert(i, FREE_NODE{ address, size });
	auto next_node = i;
	auto new_node = --i;

	// 위쪽 주소의 빈 공간 병합
	if (next_node != free_list.end()) {
		if (next_node->start_address == (address + size)) {
			new_node->size += next_node->size;
			free_list.erase(next_node);
		}
	}

	// 아래쪽 주소의 빈 공간 병합
	if (new_node != free_list.begin()) {
		auto prev_node = --i;
		if (prev_node->start_address + prev_node->size == address) {
			prev_node->size += new_node->size;
			free_list.erase(new_node);
		}
	}
	if (free_list.begin()->start_address < 0)
	{
		cout << "ERROR";
	}
}

void print_free_list(list<FREE_NODE>& free_list)
{
	cout << "Free List : ";
	for (auto& n : free_list)
		cout << "(" << n.start_address << ", " << n.size << "), ";
	cout << endl;
}

void print_unallocated_memories(queue <UNALLOCATED_REQUEST>& um)
{
	auto am = um;
	cout << "Unallocated Memories : ";
	while (am.size() != 0) {
		auto n = am.front(); am.pop();
		cout << "(" << n.size << "), ";
	}
	cout << endl;
}

void FirstFit()
{
	list <FREE_NODE> free_list;
	list <ALLOCATED_MEMORY> alloc_list;
	free_list.push_back(FREE_NODE{ 0, MEMORY_SIZE});
	queue <UNALLOCATED_REQUEST> request_queue;
	bool input_end = false;
	int clock = 0;
	int num_of_requests = 0;
	int total_return_time = 0;
	int total_wait_time = 0;
	int total_request_size = 0;
	long long total_request_timespace = 0;
	MEMORY_REQUEST mr;

	for (;; ++clock) {
		// 사용시간 끝난 메모리 반환
		for (auto it = alloc_list.begin(); it != alloc_list.end();)
			if (it->end_time > clock)
				it++;
				else {
					Return_Memory(free_list, it->start_address, it->size);
					total_return_time += clock - it->arrive_time;
					it = alloc_list.erase(it);
				}

		int req_size;
		int use_time;
		// 새로운 메모리 요청 받기
		if (false == input_end)
			while (true == mr.GetMemoryRequest(clock, &req_size, &use_time, &input_end)) {
				request_queue.push(UNALLOCATED_REQUEST{ clock, req_size, use_time });
				num_of_requests++;
				total_request_size += req_size;
				total_request_timespace += req_size * use_time;
			}

		// 종료 조건 검사
		if ((true == input_end) && (request_queue.size() == 0) && (alloc_list.empty() == true))
			break;

		// 요청 큐 검사, 메모리 할당 가능여부 검사
		while (request_queue.empty() == false) {
			auto& req = request_queue.front();
			int start_address = try_alloc_FirstFit(free_list, req.size);
			if (-1 == start_address) break;
			total_wait_time += clock - req.arrive_time;
			alloc_list.push_back(ALLOCATED_MEMORY{ clock, start_address, req.size, clock + req.use_time, req.arrive_time });
			request_queue.pop();
		}
	}

	cout << "Scheduling Algorithm : FirstFit\n";
	cout << "Memory Size = " << MEMORY_SIZE << endl;
	cout << "Number of Memory Requests = " << num_of_requests << endl;
	cout << "Total size of memory reqeusted = " << total_request_size << endl;	
	wcout << L"전체 종료 시간 = " << clock << endl;
	wcout << L"평균 대기 시간 = " << total_wait_time / num_of_requests << endl;
}

void BestFit()
{
	list <FREE_NODE> free_list;
	list <ALLOCATED_MEMORY> alloc_list;
	free_list.push_back(FREE_NODE{ 0, MEMORY_SIZE});
	queue <UNALLOCATED_REQUEST> request_queue;
	bool input_end = false;
	int clock = 0;
	int num_of_requests = 0;
	int total_return_time = 0;
	int total_wait_time = 0;
	int total_request_size = 0;
	long long total_request_timespace = 0;
	MEMORY_REQUEST mr;

	for (;; ++clock) {
		// 사용시간 끝난 메모리 반환
		for (auto it = alloc_list.begin(); it != alloc_list.end();)
			if (it->end_time > clock)
				it++;
				else {
					Return_Memory(free_list, it->start_address, it->size);
					total_return_time += clock - it->arrive_time;
					it = alloc_list.erase(it);
				}

		int req_size;
		int use_time;
		// 새로운 메모리 요청 받기
		if (false == input_end)
			while (true == mr.GetMemoryRequest(clock, &req_size, &use_time, &input_end)) {
				request_queue.push(UNALLOCATED_REQUEST{ clock, req_size, use_time });
				num_of_requests++;
				total_request_size += req_size;
				total_request_timespace += req_size * use_time;
			}

		// 종료 조건 검사
		if ((true == input_end) && (request_queue.size() == 0) && (alloc_list.empty() == true))
			break;

		// 요청 큐 검사, 메모리 할당 가능여부 검사
		while (request_queue.empty() == false) {
			auto& req = request_queue.front();
			int start_address = try_alloc_FirstFit(free_list, req.size);
			if (-1 == start_address) break;
			total_wait_time += clock - req.arrive_time;
			alloc_list.push_back(ALLOCATED_MEMORY{ clock, start_address, req.size, clock + req.use_time, req.arrive_time });
			request_queue.pop();
		}
	}

	cout << "Scheduling Algorithm : FirstFit\n";
	cout << "Memory Size = " << MEMORY_SIZE << endl;
	cout << "Number of Memory Requests = " << num_of_requests << endl;
	cout << "Total size of memory reqeusted = " << total_request_size << endl;	
	wcout << L"전체 종료 시간 = " << clock << endl;
	wcout << L"평균 대기 시간 = " << total_wait_time / num_of_requests << endl;
}



void WorstFit()
{

}

void NextFit()
{
	
}

void Buddy()
{

}


int main()
{
	wcout.imbue(locale("kor"));		// 한글 출력
	FirstFit();
	WorstFit();
	BestFit();
	NextFit();
	Buddy();
}
