#ifndef SEARCH_TERMS_H
#define SEARCH_TERMS_H

#include <string>
#include <vector>

using namespace std;

//------------------------------------------------------------//
// 힌트: STL 라이브러리를 사용하세요.
//------------------------------------------------------------//   

class Search_terms
{
public:
	Search_terms(string input_file);

	// 문제1: 검색 빈도가 가장 높은 k개 검색어를 출력한다. 검색빈도가 높은 단어부터 순서대로 출력한다.
	// 출력 방법: 검색키워드와 검색빈도의 순서쌍으로 출력한다. 한 줄에는 검색키워드 한 개만 출력한다. 
	// 예: 근린공원 412	
	//     영주어린이공원 199
	void find_most_frequent_search_terms(int k); 
	
	// 문제2: 검색 빈도 freq가  min_freq과 max_freq 사이에 있는 검색어를 출력한다.  min_freq <= freq <= max_freq
	// 검색빈도가 높은 단어부터 순서대로 출력한다.
	// 출력 방법: 검색키워드와 검색빈도의 순서쌍으로 출력한다. 한 줄에는 검색키워드 한 개만 출력한다. 
	// 예: 근린공원 412	
	//     영주어린이공원 199
	void find_frequent_search_terms(int min_freq, int max_freq);
	
	// 문제3: 사용자 입력 word가 keyword_list에 있는지 검사한다. 
	// word가 keyword_list에 없으면 false, 있으면 true를 리턴한다.
	bool is_word_in_keyword_list(string word);
	
	// 문제4: keyword_list에 있는 모든 단어들을 오름차순(사전순서, string::compare 사용)으로 출력한다.  
	// 출력 방법: 검색키워드와 검색빈도의 순서쌍으로 출력한다. 한 줄에는 검색키워드 한 개만 출력한다. 
	// 예: 근린공원 412	
	//     영주어린이공원 199
	void print_search_terms();

	//------------------------------------------------------------//

	// 여기에 코드를 추가하세요.

	//------------------------------------------------------------//   


private:
	vector<string> keyword_list;
	//------------------------------------------------------------//

	// 여기에 코드를 추가하세요.

	//------------------------------------------------------------//   
};

#endif /* SEARCH_TERMS_H */