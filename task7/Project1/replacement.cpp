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

// �ð� �˰����� ���� ������ ����ü
struct Page {
	int value;
	bool reference_bit;
};

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
	set<int> curr_frame;

	int hit = 0;

	// ���� ���� ����
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(0, NUMBER_OF_FRAME - 1); // ���� �ε��� ������ ���� ����

	for (auto pn : ap) {
		if (curr_frame.count(pn)) {
			hit++;
		}
		else {
			if (curr_frame.size() < NUMBER_OF_FRAME) {
				curr_frame.insert(pn);
			}
			else {
				// �����ϰ� ���õ� �������� ��ü
				int random_idx = dist(gen);
				auto it = curr_frame.begin();
				advance(it, random_idx);
				curr_frame.erase(it);
				curr_frame.insert(pn);
			}
		}
	}
	cout << "==================\n";
	cout << "RANDOM scheduling\n";
	cout << "Number of HIT : " << hit << endl;
	cout << "Hit Ratio : " << hit * 100 / ap.size() << endl;
}

void LRU(ACCESS_PATTERN& ap)
{
	list<int> curr_frame_list;

	int hit = 0;

	for (auto pn : ap) {
		auto it = find(curr_frame_list.begin(), curr_frame_list.end(), pn);

		if (it != curr_frame_list.end()) {
			// �������� �̹� ����Ʈ�� ������ (hit)
			hit++;
			// �������� ����Ʈ�� �� ������ �ű� (���� �ֱ� ���� ������ ǥ��)
			curr_frame_list.erase(it);
			curr_frame_list.push_front(pn);
		}
		else {
			// �������� ����Ʈ�� ������ (miss)
			if (curr_frame_list.size() >= NUMBER_OF_FRAME) {
				// ������ �������� �� �������� �� �� ������ ���� (���� ��� ���� ���� ������)
				curr_frame_list.pop_back();
			}
			// �� �������� ����Ʈ�� �� �տ� �߰� (���� �ֱ� ���� ������ ǥ��)
			curr_frame_list.push_front(pn);
		}
	}

	cout << "==================\n";
	cout << "LRU scheduling\n";
	cout << "Number of HIT : " << hit << endl;
	cout << "Hit Ratio : " << hit * 100 / ap.size() << endl;
}

void OPT(ACCESS_PATTERN& ap)
{
	set<int> curr_frame;

	int hit = 0;

	for (auto pn : ap) {
		// ������ hit
		if (curr_frame.count(pn)) {
			hit++;
		}
		else {
			int max_distance = -1;
			int page_replaced = -1;
			//  �̷��� ������ ������ �߿��� ���� ���������� ���� ������ �������� ã��
			for (auto frame : curr_frame) {
				auto it = find(ap.begin(), ap.end(), frame);
				// �� �̻� �������� �ʴ� �������� ��
				if (it == ap.end()) {
					page_replaced = frame;
					break;
				}
				else {
					// ���� �������� �ش� �������� ���������� �Ÿ��� ���
					int distance = it - ap.begin();
					if (distance > max_distance) {
						max_distance = distance;
						page_replaced = frame;
					}
				}
			}

			// ã�� �������� �����ӿ��� ��ü
			curr_frame.erase(page_replaced);
			curr_frame.insert(pn);
		}
	}

	cout << "==================\n";
	cout << "OPT scheduling\n";
	cout << "Number of HIT : " << hit << endl;
	cout << "Hit Ratio : " << hit * 100 / ap.size() << endl;
}

void CLOCK(ACCESS_PATTERN& ap)
{
	// 128�� �迭 ����
	vector<Page> curr_frame(NUMBER_OF_FRAME, { 0, false });
	int hand = 0; // �ð� �˰����� ������

	int hit = 0;

	for (auto pn : ap) {
		bool page_found = false;
		while (!page_found) {
			// ���� �����Ͱ� ����Ű�� �������� ���� ��Ʈ�� Ȯ��
			if (!curr_frame[hand].reference_bit) {
				// ���� ��Ʈ�� 0�� ��� ������ ��ü
				curr_frame[hand] = { pn, true };
				page_found = true;
			}
			else {
				// ���� ��Ʈ�� 1�� ��� ���� ��Ʈ�� 0���� �����ϰ� �����͸� �̵�
				curr_frame[hand].reference_bit = false;
			}

			// ������ �̵�
			hand = (hand + 1) % NUMBER_OF_FRAME;

			// �������� �̹� �����ӿ� �ִ��� Ȯ��
			if (curr_frame[hand].value == pn) {
				page_found = true;
				hit++;
			}
		}
	}

	cout << "==================\n";
	cout << "Clock scheduling\n";
	cout << "Number of HIT : " << hit << endl;
	cout << "Hit Ratio : " << hit * 100 / ap.size() << endl;
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