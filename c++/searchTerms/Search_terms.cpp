#include "Search_terms.h"
#include <fstream>
#include <string>
#include <iostream>
#include <string>
#include <algorithm>
#include <map>

using namespace std;

//�󵵼��� ���� ���� template struct
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
	//hash�� �ܾ�� ����Ƚ�� ���� 
	for (int i = 0;i < keyword_list.size();i++) {
		hash[keyword_list[i]]++;
	}
	//�󵵼��� �����ϱ� ���ؼ� ������ ���� vector�� ������� 
	vector<pair<string, int>> vectorData;
	for (auto iter = hash.begin();iter != hash.end();iter++) {
		vectorData.emplace_back(make_pair(iter->first, iter->second));
	}
	//���� 
	sort(vectorData.begin(), vectorData.end(), compare_pair_second<greater>());

	//out_file�� k������ �Է�
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
	//hash�� �ܾ�� ����Ƚ�� ���� 
	for (int i = 0;i < keyword_list.size();i++) {
		hash[keyword_list[i]]++;
	}
	//�󵵼��� �����ϱ� ���ؼ� ������ ���� vector�� ������� 
	vector<pair<string, int>> vectorData;
	for (auto iter = hash.begin();iter != hash.end();iter++) {
		vectorData.emplace_back(make_pair(iter->first, iter->second));
	}
	//����
	sort(vectorData.begin(), vectorData.end(), compare_pair_second<greater>());

	int min = min_freq;
	int max = max_freq;

	//out_file�� min���� max���� �Է�
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
			//bool �̶� ���ڷ� ����.
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

	sort(keyword_list.begin(), keyword_list.end()); // keyword_list�� �������� ����
	vector<pair<string, int>> key;
	key.push_back(make_pair(keyword_list[0], 1)); // ù �ܾ�, 1 �ʱ�ȭ

	for (int i = 1; i < keyword_list.size(); i++) {
		if (keyword_list[i - 1] == keyword_list[i]) key.back().second++; // ������ ������ +1
		else {
			key.push_back(make_pair(keyword_list[i], 1));   // ������ �ٸ��� ���ο� �ܾ�� �߰�
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

    // Ű���� ���Ͽ��� ���پ� �о keyword_list�� �����Ѵ�. 
    while (getline(kw_file, line))
    {
        line = trim(line);

        if (line.empty()) { continue;  }

        keyword_list.push_back(line);


    }
    kw_file.close();

}



//--------------------------------------------------------------------------//
// �����ϼ���. Ű���� ���� ���� ���� ���ڸ� �����մϴ�. 
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