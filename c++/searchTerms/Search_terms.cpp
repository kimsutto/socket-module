#include "Search_terms.h"
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

//빈도수에 따라 정렬 template struct
template<template <typename> class P = std::less >
struct compare_pair_second {
	template<class T1, class T2> bool operator()(const std::pair<T1, T2>& left, const std::pair<T1, T2>& right) {
		return P<T2>()(left.second, right.second);
	}
};


void Search_terms::find_most_frequent_search_terms(int k)
{
    ofstream out_file;
    out_file.open("result_1.txt");
    //------------------------------------------------------------//
	map<string, int> hash;
	//hash에 단어와 등장횟수 저장 
	for (int i = 0;i < keyword_list.size();i++) {
		hash[keyword_list[i]]++;
	}
	//빈도수로 정렬하기 위해서 다음과 같이 vector로 만들어줌 
	vector<pair<string, int>> vectorData;
	for (auto iter = hash.begin();iter != hash.end();iter++) {
		vectorData.emplace_back(make_pair(iter->first, iter->second));
	}
	//정렬 
	sort(vectorData.begin(), vectorData.end(), compare_pair_second<greater>());

	//out_file에 k번까지 입력
	int num = k;
	for (int j = 0; j < num; j++) {
		string dummy_word = vectorData[j].first;
		int dummy_word_freq = vectorData[j].second;
		out_file << dummy_word << "\t" << dummy_word_freq;
		out_file << "\n";
	}
    
    //------------------------------------------------------------//
    
    out_file.close();

}

void Search_terms::find_frequent_search_terms(int min_freq, int max_freq)
{
    ofstream out_file;
    out_file.open("result_2.txt");
    //------------------------------------------------------------//

	map<string, int> hash;
	//hash에 단어와 등장횟수 저장 
	for (int i = 0;i < keyword_list.size();i++) {
		hash[keyword_list[i]]++;
	}
	//빈도수로 정렬하기 위해서 다음과 같이 vector로 만들어줌 
	vector<pair<string, int>> vectorData;
	for (auto iter = hash.begin();iter != hash.end();iter++) {
		vectorData.emplace_back(make_pair(iter->first, iter->second));
	}
	//정렬
	sort(vectorData.begin(), vectorData.end(), compare_pair_second<greater>());

	int min = min_freq;
	int max = max_freq;

	//out_file에 min에서 max까지 입력
	for(int j = min; j<max; j++) {
		string dummy_word = vectorData[j].first;
		int dummy_word_freq = vectorData[j].second;
		out_file << dummy_word << "\t" << dummy_word_freq;
		out_file << "\n";
	}

    //------------------------------------------------------------//
    
    out_file.close();

}


bool Search_terms::is_word_in_keyword_list(string word)
{
    bool ret_val = true;

    ofstream out_file;
    out_file.open("result_3.txt");

	for (int i = 0; i < keyword_list.size(); i++) {
		if (word == keyword_list[i]) {
			//bool 이라 숫자로 찍힘.
			out_file << word << "\t" << ret_val;
			out_file.close();
			return true;
		}
	}
	ret_val = false;
	out_file << word << "\t" << ret_val;
	out_file.close();
	return false;
 

}

void Search_terms::print_search_terms()
{
    ofstream out_file;
    out_file.open("result_4.txt");
    //------------------------------------------------------------//

	sort(keyword_list.begin(), keyword_list.end()); // keyword_list를 오름차순 정렬
	vector<pair<string, int>> key;
	key.push_back(make_pair(keyword_list[0], 1)); // 첫 단어, 1 초기화

	for (int i = 1; i < keyword_list.size(); i++) {
		if (keyword_list[i - 1] == keyword_list[i]) key.back().second++; // 이전과 같으면 +1
		else {
			key.push_back(make_pair(keyword_list[i], 1));   // 이전과 다르면 새로운 단어로 추가
		}
	}

	for (int j = 0; j <key.size(); j++) {
		string dummy_word = key[j].first;
		int dummy_word_freq = key[j].second;
		out_file << dummy_word << "\t" << dummy_word_freq;
		out_file << "\n";
	}
    
    out_file.close();

}


Search_terms::Search_terms(string kw_filename)
{
    extern string trim(const string & s);
    string line;
    ifstream kw_file(kw_filename);

    if (!kw_file.is_open())
    {
        cout << "Unable to open file " << kw_filename;
        exit(-1);
    }

    // 키워드 파일에서 한줄씩 읽어서 keyword_list에 저장한다. 
    while (getline(kw_file, line))
    {
        line = trim(line);

        if (line.empty()) { continue;  }

        keyword_list.push_back(line);


    }
    kw_file.close();

}



//--------------------------------------------------------------------------//
// 참고하세요. 키워드 양쪽 끝에 공백 문자를 제거합니다. 
const string WHITESPACE = " \n\r\t\f\v";

string ltrim(const string& s)
{
    size_t start = s.find_first_not_of(WHITESPACE);
    return (start == string::npos) ? "" : s.substr(start);
}

string rtrim(const string& s)
{
    size_t end = s.find_last_not_of(WHITESPACE);
    return (end == string::npos) ? "" : s.substr(0, end + 1);
}

string trim(const string& s)
{
    return rtrim(ltrim(s));
}
//--------------------------------------------------------------------------//