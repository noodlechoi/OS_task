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

// best fit ��� ã��
auto try_alloc_BestFit(list <FREE_NODE>& free_list, int size)
{
	int min = MEMORY_SIZE + 1;
	auto best_fit_block = free_list.end();

	for (auto i = free_list.begin(); i != free_list.end();) {
		// �䱸�� �ͺ��ٴ� Ŀ����
		if (i->size >= size) {
			int diff = i->size - size;
			// ���� ���� �� ã��
			if (diff < min) {
				min = diff;
				best_fit_block = i;
			}
		}
		++i;
	}

	// ����
	if (min != MEMORY_SIZE + 1)	return best_fit_block;
	else return free_list.end();
}

// worst fit  ��� ã��
auto try_alloc_WorstFit(list <FREE_NODE>& free_list, int size)
{
	int max = -1;
	auto worst_fit_block = free_list.end();

	for (auto i = free_list.begin(); i != free_list.end();) {
		if (i->size >= size) {
			int diff = i->size - size;
			// best fit���� �ݴ�� ũ�Ⱑ ū �� ã��
			if (diff > max) {
				max = diff;
				worst_fit_block = i;
			}
		}
		++i;
	}

	// ����
	if (max != -1)	return worst_fit_block;
	else return free_list.end();
}

// next fit
auto try_alloc_NextFit(list <FREE_NODE>& free_list, int size, list <FREE_NODE>::iterator& next)
{
	for (auto i = next; i != free_list.end();) {
		if (i->size >= size) {
			int start_address = i->start_address;
			
			return i;
		}
		++i;
	}
	return free_list.end();
}

void Return_Memory(list <FREE_NODE>& free_list, int address, int size)
{
	auto i = free_list.begin();

	// ��ȯ�� ��ġ �˻�)
	while (i != free_list.end()) {
		if (i->start_address > address) break;
		else ++i;
	}
	free_list.insert(i, FREE_NODE{ address, size });
	auto next_node = i;
	auto new_node = --i;

	// ���� �ּ��� �� ���� ����
	if (next_node != free_list.end()) {
		if (next_node->start_address == (address + size)) {
			new_node->size += next_node->size;
			free_list.erase(next_node);
		}
	}

	// �Ʒ��� �ּ��� �� ���� ����
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
		// ���ð� ���� �޸� ��ȯ
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
		// ���ο� �޸� ��û �ޱ�
		if (false == input_end)
			while (true == mr.GetMemoryRequest(clock, &req_size, &use_time, &input_end)) {
				request_queue.push(UNALLOCATED_REQUEST{ clock, req_size, use_time });
				num_of_requests++;
				total_request_size += req_size;
				total_request_timespace += req_size * use_time;
			}

		// ���� ���� �˻�
		if ((true == input_end) && (request_queue.size() == 0) && (alloc_list.empty() == true))
			break;

		// ��û ť �˻�, �޸� �Ҵ� ���ɿ��� �˻�
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
	wcout << L"��ü ���� �ð� = " << clock << endl;
	wcout << L"��� ��� �ð� = " << total_wait_time / num_of_requests << endl;
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
		// ���ð� ���� �޸� ��ȯ
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
		// ���ο� �޸� ��û �ޱ�
		if (false == input_end)
			while (true == mr.GetMemoryRequest(clock, &req_size, &use_time, &input_end)) {
				request_queue.push(UNALLOCATED_REQUEST{ clock, req_size, use_time });
				num_of_requests++;
				total_request_size += req_size;
				total_request_timespace += req_size * use_time;
			}

		// ���� ���� �˻�
		if ((true == input_end) && (request_queue.size() == 0) && (alloc_list.empty() == true))
			break;

		// ��û ť �˻�, �޸� �Ҵ� ���ɿ��� �˻�	(Best Fit �˰���)
		while (request_queue.empty() == false) {
			auto& req = request_queue.front();
			// free list ã��
			auto best_fit_block = try_alloc_BestFit(free_list, req.size);

			// �Ҵ�� ��� ������ �κ� free list�� �߰�
			if (free_list.end() != best_fit_block) {
				int start_address = best_fit_block->start_address;
				int block_size = best_fit_block->size;

				if (block_size == req.size)
					free_list.erase(best_fit_block);
				else {
					best_fit_block->size = block_size - req.size;
					best_fit_block->start_address = start_address + req.size;
				}

				// �ð� ���
				total_wait_time += clock - req.arrive_time;
				alloc_list.push_back(ALLOCATED_MEMORY{ clock, start_address, req.size, clock + req.use_time, req.arrive_time });
				request_queue.pop();
			}
			// �Ҵ� ������ ��� �� ã���� ����
			else break;

		}
	}

	cout << "Scheduling Algorithm : BestFit\n";
	cout << "Memory Size = " << MEMORY_SIZE << endl;
	cout << "Number of Memory Requests = " << num_of_requests << endl;
	cout << "Total size of memory reqeusted = " << total_request_size << endl;	
	wcout << L"��ü ���� �ð� = " << clock << endl;
	wcout << L"��� ��� �ð� = " << total_wait_time / num_of_requests << endl;
}



void WorstFit()
{
	list <FREE_NODE> free_list;
	list <ALLOCATED_MEMORY> alloc_list;
	free_list.push_back(FREE_NODE{ 0, MEMORY_SIZE });
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
		// ���ð� ���� �޸� ��ȯ
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
		// ���ο� �޸� ��û �ޱ�
		if (false == input_end)
			while (true == mr.GetMemoryRequest(clock, &req_size, &use_time, &input_end)) {
				request_queue.push(UNALLOCATED_REQUEST{ clock, req_size, use_time });
				num_of_requests++;
				total_request_size += req_size;
				total_request_timespace += req_size * use_time;
			}

		// ���� ���� �˻�
		if ((true == input_end) && (request_queue.size() == 0) && (alloc_list.empty() == true))
			break;

		// ��û ť �˻�, �޸� �Ҵ� ���ɿ��� �˻�	(Worst Fit �˰���)
		while (request_queue.empty() == false) {
			auto& req = request_queue.front();
			// free list ã��
			auto worst_fit_block = try_alloc_WorstFit(free_list, req.size);

			// �Ҵ�� ����� ������ �κ� free list�� �߰�
			if (free_list.end() != worst_fit_block) {
				int start_address = worst_fit_block->start_address;
				int block_size = worst_fit_block->size;

				// ũ�� ������ �׳� �����
				if (block_size == req.size)
					free_list.erase(worst_fit_block);
				else {
					worst_fit_block->size = block_size - req.size;
					worst_fit_block->start_address = start_address + req.size;
				}

				// �ð� ���
				total_wait_time += clock - req.arrive_time;
				alloc_list.push_back(ALLOCATED_MEMORY{ clock, start_address, req.size, clock + req.use_time, req.arrive_time });
				request_queue.pop();
			}
			// �Ҵ� ������ ��� �� ã���� ����
			else break;

		}
	}

	cout << "Scheduling Algorithm : WorstFit\n";
	cout << "Memory Size = " << MEMORY_SIZE << endl;
	cout << "Number of Memory Requests = " << num_of_requests << endl;
	cout << "Total size of memory reqeusted = " << total_request_size << endl;
	wcout << L"��ü ���� �ð� = " << clock << endl;
	wcout << L"��� ��� �ð� = " << total_wait_time / num_of_requests << endl;
}

void NextFit()
{
	// first fit���� ������ ��ġ�� ���� �� �� �������� ã���� ����
	list <FREE_NODE> free_list;
	list <ALLOCATED_MEMORY> alloc_list;
	free_list.push_back(FREE_NODE{ 0, MEMORY_SIZE });
	queue <UNALLOCATED_REQUEST> request_queue;
	bool input_end = false;
	int clock = 0;
	int num_of_requests = 0;
	int total_return_time = 0;
	int total_wait_time = 0;
	int total_request_size = 0;
	long long total_request_timespace = 0;
	MEMORY_REQUEST mr;

	// ù ��° ��ġ ����
	auto allocated = free_list.begin();

	for (;; ++clock) {
		// ���ð� ���� �޸� ��ȯ
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
		// ���ο� �޸� ��û �ޱ�
		if (false == input_end)
			while (true == mr.GetMemoryRequest(clock, &req_size, &use_time, &input_end)) {
				request_queue.push(UNALLOCATED_REQUEST{ clock, req_size, use_time });
				num_of_requests++;
				total_request_size += req_size;
				total_request_timespace += req_size * use_time;
			}

		// ���� ���� �˻�
		if ((true == input_end) && (request_queue.size() == 0) && (alloc_list.empty() == true))
			break;

		// ��û ť �˻�, �޸� �Ҵ� ���ɿ��� �˻�
		while (request_queue.empty() == false) {
			auto& req = request_queue.front();
			allocated = try_alloc_NextFit(free_list, req.size, allocated);
			if (free_list.end() == allocated) {
				// �ٽ� ó������
				allocated = free_list.begin();
				break;
			}
			// ����
			if (allocated->size == req.size)
				free_list.erase(allocated);
			else {
				allocated->size = allocated->size - req.size;
				allocated->start_address = allocated->start_address + req.size;
			}
			total_wait_time += clock - req.arrive_time;
			alloc_list.push_back(ALLOCATED_MEMORY{ clock, allocated->start_address, req.size, clock + req.use_time, req.arrive_time });
			request_queue.pop();
		}
	}

	cout << "Scheduling Algorithm : NextFit\n";
	cout << "Memory Size = " << MEMORY_SIZE << endl;
	cout << "Number of Memory Requests = " << num_of_requests << endl;
	cout << "Total size of memory reqeusted = " << total_request_size << endl;
	wcout << L"��ü ���� �ð� = " << clock << endl;
	wcout << L"��� ��� �ð� = " << total_wait_time / num_of_requests << endl;
}


void Buddy()
{
	
}


int main()
{
	wcout.imbue(locale("kor"));		// �ѱ� ���
	FirstFit();
	WorstFit();
	BestFit();
	// ���� �߻�
	//NextFit();
	Buddy();
}
