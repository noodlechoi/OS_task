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

// 시계 알고리즘을 위한 페이지 구조체
struct Page {
	int value;
	bool reference_bit;
};

void FIFO(ACCESS_PATTERN & ap)
{
	// frame 큐와 집합
	queue <int> curr_frame_queue;
	set <int> curr_frame;

	int hit = 0;

	for (auto pn : ap) {
		// 이미 있으면 hit
		if (curr_frame.count(pn)) hit++;
		// 없으면  miss
		else
			// 작을 때 그냥 넣기
			if (curr_frame.size() < NUMBER_OF_FRAME) {
				curr_frame_queue.push(pn);
				curr_frame.insert(pn);
			}
			else {
				int victim = curr_frame_queue.front();
				// 큐에서 뺀 다음 넣기
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

	// 랜덤 엔진 설정
	random_device rd;
	mt19937 gen(rd());
	uniform_int_distribution<> dist(0, NUMBER_OF_FRAME - 1); // 랜덤 인덱스 생성을 위한 분포

	for (auto pn : ap) {
		if (curr_frame.count(pn)) {
			hit++;
		}
		else {
			if (curr_frame.size() < NUMBER_OF_FRAME) {
				curr_frame.insert(pn);
			}
			else {
				// 랜덤하게 선택된 페이지를 교체
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
			// 페이지가 이미 리스트에 있으면 (hit)
			hit++;
			// 페이지를 리스트의 맨 앞으로 옮김 (가장 최근 사용된 것으로 표시)
			curr_frame_list.erase(it);
			curr_frame_list.push_front(pn);
		}
		else {
			// 페이지가 리스트에 없으면 (miss)
			if (curr_frame_list.size() >= NUMBER_OF_FRAME) {
				// 페이지 프레임이 다 차있으면 맨 뒤 페이지 제거 (오래 사용 되지 않은 페이지)
				curr_frame_list.pop_back();
			}
			// 새 페이지를 리스트의 맨 앞에 추가 (가장 최근 사용된 것으로 표시)
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
		// 있으면 hit
		if (curr_frame.count(pn)) {
			hit++;
		}
		else {
			int max_distance = -1;
			int page_replaced = -1;
			//  미래에 참조될 페이지 중에서 현재 페이지보다 먼저 나오는 페이지를 찾음
			for (auto frame : curr_frame) {
				auto it = find(ap.begin(), ap.end(), frame);
				// 더 이상 참조되지 않는 페이지일 때
				if (it == ap.end()) {
					page_replaced = frame;
					break;
				}
				else {
					// 현재 페이지와 해당 프레임의 참조까지의 거리를 계산
					int distance = it - ap.begin();
					if (distance > max_distance) {
						max_distance = distance;
						page_replaced = frame;
					}
				}
			}

			// 찾은 페이지를 프레임에서 교체
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
	// 128개 배열 생성
	vector<Page> curr_frame(NUMBER_OF_FRAME, { 0, false });
	int hand = 0; // 시계 알고리즘의 포인터

	int hit = 0;

	for (auto pn : ap) {
		bool page_found = false;
		while (!page_found) {
			// 현재 포인터가 가리키는 페이지의 참조 비트를 확인
			if (!curr_frame[hand].reference_bit) {
				// 참조 비트가 0인 경우 페이지 교체
				curr_frame[hand] = { pn, true };
				page_found = true;
			}
			else {
				// 참조 비트가 1인 경우 참조 비트를 0으로 변경하고 포인터를 이동
				curr_frame[hand].reference_bit = false;
			}

			// 포인터 이동
			hand = (hand + 1) % NUMBER_OF_FRAME;

			// 페이지가 이미 프레임에 있는지 확인
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