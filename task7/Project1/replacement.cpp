#include <iostream>
#include <fstream>
#include <vector>
#include <queue>
#include <set>
#include <map>
#include <random>
#include <unordered_map>
using namespace std;

#include "definition.h"

typedef vector <int> ACCESS_PATTERN;

void FIFO(ACCESS_PATTERN & ap)
{
	// frame ť�� ����
	queue <int> curr_frame_queue;
	set <int> curr_frame;

	int hit = 0;

	for (auto pn : ap) {
		// �̹� ������ hit
		if (curr_frame.count(pn)) hit++;
		// ������  miss
		else
			// ���� �� �׳� �ֱ�
			if (curr_frame.size() < NUMBER_OF_FRAME) {
				curr_frame_queue.push(pn);
				curr_frame.insert(pn);
			}
			else {
				int victim = curr_frame_queue.front();
				// ť���� �� ���� �ֱ�
				curr_frame_queue.pop();
				curr_frame_queue.push(pn);
				curr_frame.erase(victim);
				curr_frame.insert(pn);
			}
	}
	cout << "==================\n";
	cout << "FIFO scheduling\n";
	cout << "Number of HIT : " << hit << endl;
	cout << "Hit Ratio : " << hit * 100 / ap.size() << endl;
}

void RANDOM(ACCESS_PATTERN& ap)
{
	// frame ť�� ����
	queue <int> curr_frame_queue;
	set <int> curr_frame;

	int hit = 0;

	for (auto pn : ap) {
		// �̹� ������ hit
		if (curr_frame.count(pn)) hit++;
		// ������  miss
		else
			// ���� �� �׳� �ֱ�
			if (curr_frame.size() < NUMBER_OF_FRAME) {
				curr_frame_queue.push(pn);
				curr_frame.insert(pn);
			}
			else {
				int victim = curr_frame_queue.front();
				// ť���� �� ���� �ֱ�
				curr_frame_queue.pop();
				curr_frame_queue.push(pn);
				curr_frame.erase(victim);
				curr_frame.insert(pn);
			}
	}
	cout << "==================\n";
	cout << "RANDOM scheduling\n";
	cout << "Number of HIT : " << hit << endl;
	cout << "Hit Ratio : " << hit * 100 / ap.size() << endl;
}

void LRU(ACCESS_PATTERN& ap)
{
	
}

void OPT(ACCESS_PATTERN& ap)
{
	
}

void CLOCK(ACCESS_PATTERN& ap)
{

}

int main()
{
	// input_generation();

	cout << "Loading Page Access Pattern : ";
	ACCESS_PATTERN access_pattern;
	ifstream input("page_data.txt");
	for (;;) {
		int page_num;
		input >> page_num;
		if (-1 == page_num)
			break;
		else
			access_pattern.push_back(page_num);
	}
	input.close();
	cout << "DONE.\n";

	OPT(access_pattern);
	FIFO(access_pattern);
	LRU(access_pattern);
	RANDOM(access_pattern);
	CLOCK(access_pattern);

}