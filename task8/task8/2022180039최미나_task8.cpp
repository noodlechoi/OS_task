#include <thread>
#include <iostream>
#include <atomic>
#include <mutex>
#include <chrono>

#define BUCKETS 1000

using namespace std;
using namespace chrono;

static const int NUMCPUS = 8;

thread_local static int local; 	// local count (per cpu)

// sloppy counter
class sloppy_counter_t {
	atomic_int value;
	int th;
public:
	sloppy_counter_t(int s) : value(0), th(s) { local = 0; }

	void increment() 
	{ 
		local++;
		if (local >= th) flush();
	}
	int get() { return value; }
	void flush()
	{
		value += local;
		local = 0;
	}
};

class NODE {
public:
	int key;
	NODE* next;

	NODE() : key(0), next(nullptr) {}
};

// 연결리스트
class con_linked_list {
	NODE* head;
	std::mutex lock;
	atomic_int insert_sum;
	atomic_int exe_sum;
	int th;
public:
	con_linked_list() : head{ nullptr }, insert_sum{ 0 }, exe_sum{ 0 }, th{ 0 } {}
	con_linked_list(int s) : head{ nullptr }, insert_sum{ 0 }, exe_sum{ 0 }, th{ s } {}

	bool insert(int key)
	{
		lock.lock();
		NODE* p = new NODE;
		if (p == nullptr) {
			cout << "Can’t allocate memory." << endl;
			lock.unlock();
			return false; // fail
		}
		p->key = key;

		// 삽입할 때 더하기
		local += p->key;
		if (local >= th) flush();

		p->next = head;
		head = p;
		lock.unlock();
		return true; // success
	}

	bool lookup(int key) {
		lock.lock();
		NODE* curr = head;
		while (nullptr != curr) {
			if (curr->key == key) {
				lock.unlock();
				return true;
				break;
			}
			curr = curr->next;
		}
		lock.unlock();
		return false; // fail
	}

	void increment()
	{
		insert(1);
	}

	int get()
	{
		return insert_sum;
	}

	int execution_get()
	{
		lock.lock();
		NODE* curr = head;
		while (nullptr != curr) {
			exe_sum += curr->key;
			curr = curr->next;
		}
		lock.unlock();

		return exe_sum;
	}

	void flush()
	{
		insert_sum += local;
		local = 0;
	}

};


// 개선 병행 연결리스트
class improve_linked_list {
	NODE* head;
	std::mutex lock;
	atomic_int insert_sum;
	atomic_int exe_sum;
	int th;
public:
	improve_linked_list() : head{ nullptr }, insert_sum{ 0 }, exe_sum{ 0 }, th{ 0 } {}
	improve_linked_list(int s) : head{ nullptr }, insert_sum{ 0 }, exe_sum{ 0 }, th{ s } {}

	bool insert(int key)
	{
		NODE* p = new NODE;
		if (p == nullptr) {
			cout << "Can’t allocate memory." << endl;
			return false; // fail
		}
		p->key = key;

		// 삽입할 때 더하기
		local += p->key;
		if (local >= th) flush();

		lock.lock();
		p->next = head;
		head = p;
		lock.unlock();
		return true; // success
	}

	bool lookup(int key) {
		bool rv = false;
		lock.lock();
		NODE* curr = head;
		while (nullptr != curr) {
			if (curr->key == key) {
				rv = true;
				break;
			}
			curr = curr->next;
		}
		lock.unlock();
		return rv;
	}

	void increment()
	{
		insert(1);
	}

	int get()
	{
		return insert_sum;
	}

	int execution_get()
	{
		lock.lock();
		NODE* curr = head;
		while (nullptr != curr) {
			exe_sum += curr->key;
			curr = curr->next;
		}
		lock.unlock();

		return exe_sum;
	}

	void flush()
	{
		insert_sum += local;
		local = 0;
	}

};


class NODE_Lock {
public:
	int key;
	NODE_Lock* next;
	std::mutex lock;

	NODE_Lock() : key(0), next(nullptr) {}
};

// Hand-over-hand locking
class HOHL_linked_list {
	NODE_Lock* head;
	atomic_int insert_sum;
	atomic_int exe_sum;
	int th;
public:
	HOHL_linked_list() : head{ nullptr }, insert_sum{ 0 }, exe_sum{ 0 }, th{ 0 } {}
	HOHL_linked_list(int s) : head{ nullptr }, insert_sum{ 0 }, exe_sum{ 0 }, th{ s } {}

	bool insert(int key)
	{
		NODE_Lock* p = new NODE_Lock;
		if (p == nullptr) {
			cout << "Can’t allocate memory." << endl;
			return false; // fail
		}
		p->key = key;

		local += p->key;
		if (local >= th) flush();

		p->lock.lock();
		p->next = head;
		head = p;
		p->lock.unlock();
		return true; // success
	}

	bool lookup(int key) {
		bool rv = false;
		NODE_Lock* curr = head;
		curr->lock.lock();
		while (nullptr != curr) {
			if (curr->key == key) {
				rv = true;
				break;
			}
			curr->lock.unlock();
			curr = curr->next;
			curr->lock.lock();
		}
		curr->lock.unlock();
		return rv;
	}

	void increment()
	{
		insert(1);
	}

	int get()
	{
		return insert_sum;
	}

	int execution_get()
	{
		NODE_Lock* curr = head;
		curr->lock.lock();
		while (nullptr != curr) {
			curr->lock.unlock();
			exe_sum += curr->key;
			curr = curr->next;
			// 현재 널 포인터면 탈출
			if (curr == nullptr) break;
			curr->lock.lock();
		}

		return exe_sum;
	}

	void flush()
	{
		insert_sum += local;
		local = 0;
	}

};

// 해쉬 테이블 A를 위한 mutex 없는 연결리스트
class linked_list {
	NODE* head;
	atomic_int insert_sum;
	atomic_int exe_sum;
	int th;
public:
	linked_list() : head{ nullptr }, insert_sum{ 0 }, exe_sum{ 0 }, th{ 0 } {}
	linked_list(int s) : head{ nullptr }, insert_sum{ 0 }, exe_sum{ 0 }, th{ s } {}

	bool insert(int key)
	{
		NODE* p = new NODE;
		if (p == nullptr) {
			cout << "Can’t allocate memory." << endl;
			return false; // fail
		}
		p->key = key;

		// 삽입할 때 더하기
		local += p->key;
		if (local >= th) flush();

		p->next = head;
		head = p;
		return true; // success
	}

	bool lookup(int key) {
		bool rv = false;
		NODE* curr = head;
		while (nullptr != curr) {
			if (curr->key == key) {
				rv = true;
				break;
			}
			curr = curr->next;
		}
		return rv;
	}

	void increment()
	{
		insert(1);
	}

	int get()
	{
		return insert_sum;
	}

	int execution_get()
	{
		NODE* curr = head;
		while (nullptr != curr) {
			exe_sum += curr->key;
			curr = curr->next;
		}

		return exe_sum;
	}

	void flush()
	{
		insert_sum += local;
		local = 0;
	}
};

class HASH_TABLE_A {
	linked_list lists[BUCKETS];
public:
	HASH_TABLE_A() {}
	int insert(int key)
	{
		int bucket = key % BUCKETS;
		return lists[bucket].insert(key);

	}
	int lookup(int key)
	{
		int bucket = key % BUCKETS;
		return lists[bucket].lookup(key);
	}

	int get(int key)
	{
		int bucket = key % BUCKETS;
		return lists[bucket].get();
	}

	int execution_get(int key)
	{
		int bucket = key % BUCKETS;
		return lists[bucket].execution_get();
	}
};


class HASH_TABLE_B {
	improve_linked_list lists[BUCKETS];
public:
	HASH_TABLE_B() {}
	int insert(int key)
	{
		int bucket = key % BUCKETS;
		return lists[bucket].insert(key);

	}
	int lookup(int key)
	{
		int bucket = key % BUCKETS;
		return lists[bucket].lookup(key);
	}

	int get(int key)
	{
		int bucket = key % BUCKETS;
		return lists[bucket].get();
	}

	int execution_get(int key)
	{
		int bucket = key % BUCKETS;
		return lists[bucket].execution_get();
	}
};

// counter_t c{ 10000 };
//sloppy_counter_t c{ 10000 };
con_linked_list c1{ 10000 };
improve_linked_list c2{ 10000 };
HOHL_linked_list c3{ 10000 };
HASH_TABLE_A A;
HASH_TABLE_B B;

void thread_func1(int loops)
{
	for (int i = 0; i < loops; ++i) {
		c1.increment();
	}
	c1.flush();
}

void thread_func2(int loops)
{
	for (int i = 0; i < loops; ++i) {
		c2.increment();
	}
	c2.flush();
}

void thread_func3(int loops)
{
	for (int i = 0; i < loops; ++i) {
		c3.increment();
	}
	c3.flush();
}

void bench_mark_A(int loops)
{
	for(int i = 0; i < loops; ++i)
	{
		if (rand() % 2) {
			A.insert(rand() % 10000);
		}
		else
			A.lookup(rand() % 10000);
	}
}


void bench_mark_B(int loops)
{
	for (int i = 0; i < loops; ++i)
	{
		if (rand() % 2) {
			B.insert(rand() % 10000);
		}
		else
			B.lookup(rand() % 10000);
	}
}

int main()
{
	thread th[NUMCPUS];
	cout << "병행 연결리스트" << endl;
	for (int num_th = 1; num_th <= NUMCPUS; ++num_th) {
		auto st_t = high_resolution_clock::now();
		for (int j = 0; j < num_th; ++j)
			if (j==0) th[j] = thread{ thread_func1, 10000000 - (10000000 / num_th * (num_th - 1))};
			else th[j] = thread{ thread_func1, 10000000 / num_th };
		for (int j = 0; j < num_th; ++j) th[j].join();
		auto end_t = high_resolution_clock::now();
		auto t = end_t - st_t;
		cout << num_th << " Threads, Time = " << duration_cast<milliseconds>(t).count();
		cout << "   삽입 값 더하기 : Result = " << c1.get();
		cout << ",  실행 후 더하기 : Result = " << c1.execution_get() << endl;
	}

	cout << endl;
	cout << "개선 병행 연결리스트" << endl;
	for (int num_th = 1; num_th <= NUMCPUS; ++num_th) {
		auto st_t = high_resolution_clock::now();
		for (int j = 0; j < num_th; ++j)
			if (j == 0) th[j] = thread{ thread_func2, 10000000 - (10000000 / num_th * (num_th - 1)) };
			else th[j] = thread{ thread_func2, 10000000 / num_th };
		for (int j = 0; j < num_th; ++j) th[j].join();
		auto end_t = high_resolution_clock::now();
		auto t = end_t - st_t;
		cout << num_th << " Threads, Time = " << duration_cast<milliseconds>(t).count();
		cout << "   삽입 값 더하기 : Result = " << c2.get();
		cout << ",  실행 후 더하기 : Result = " << c2.execution_get() << endl;
	}

	cout << endl;
	cout << "Hand-over-hand locking" << endl;
	for (int num_th = 1; num_th <= NUMCPUS; ++num_th) {
		auto st_t = high_resolution_clock::now();
		for (int j = 0; j < num_th; ++j)
			if (j == 0) th[j] = thread{ thread_func3, 10000000 - (10000000 / num_th * (num_th - 1)) };
			else th[j] = thread{ thread_func3, 10000000 / num_th };
		for (int j = 0; j < num_th; ++j) th[j].join();
		auto end_t = high_resolution_clock::now();
		auto t = end_t - st_t;
		cout << num_th << " Threads, Time = " << duration_cast<milliseconds>(t).count();
		cout << "   삽입 값 더하기 : Result = " << c3.get();
		cout << ",  실행 후 더하기 : Result = " << c3.execution_get() << endl;
	}

	cout << endl;
	cout << "hash table A" << endl;
	for (int num_th = 1; num_th <= NUMCPUS; ++num_th) {
		auto st_t = high_resolution_clock::now();
		for (int j = 0; j < num_th; ++j)
			if (j == 0) th[j] = thread{ bench_mark_A, 10000000 - (10000000 / num_th * (num_th - 1)) };
			else th[j] = thread{ bench_mark_A, 10000000 / num_th };
		for (int j = 0; j < num_th; ++j) th[j].join();
		auto end_t = high_resolution_clock::now();
		auto t = end_t - st_t;
		cout << num_th << " Threads, Time = " << duration_cast<milliseconds>(t).count();
		cout << "   삽입 값 더하기 : Result = " << A.get(rand() % 2);
		cout << ",  실행 후 더하기 : Result = " << A.execution_get(rand() % 2) << endl;
	}

	cout << endl;
	cout << "hash table B" << endl;
	for (int num_th = 1; num_th <= NUMCPUS; ++num_th) {
		auto st_t = high_resolution_clock::now();
		for (int j = 0; j < num_th; ++j)
			if (j == 0) th[j] = thread{ bench_mark_B, 10000000 - (10000000 / num_th * (num_th - 1)) };
			else th[j] = thread{ bench_mark_B, 10000000 / num_th };
		for (int j = 0; j < num_th; ++j) th[j].join();
		auto end_t = high_resolution_clock::now();
		auto t = end_t - st_t;
		cout << num_th << " Threads, Time = " << duration_cast<milliseconds>(t).count();
		cout << "   삽입 값 더하기 : Result = " << B.get(rand() % 2);
		cout << ",  실행 후 더하기 : Result = " << B.execution_get(rand() % 2) << endl;
	}

	system("pause");
}
