// Search_terms.cpp : 이 파일에는 'main' 함수가 포함됩니다. 거기서 프로그램 실행이 시작되고 종료됩니다.
//
#include "Search_terms.h"
#include <iostream>
#include <time.h>

// 주의 : main 함수는 수정하지 마세요. 
int main()
{
	clock_t tStart = clock();

	Search_terms srch_kw("input_search_terms.txt");
	string word = "놀이동산";

	srch_kw.find_most_frequent_search_terms(5);
	srch_kw.find_frequent_search_terms(100, 200);
	srch_kw.is_word_in_keyword_list(word);
	srch_kw.print_search_terms();

	// 실행 시간을 측정합니다. 
	cout << "Time taken: " << (double)(clock() - tStart) / CLOCKS_PER_SEC << " s" << endl;

	return 0;
}





