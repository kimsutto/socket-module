#include "WoeidImporter.h"
#include "Woeid.h"
#include <Windows.h>
#include<mutex>
#include<thread>
#include<string>
#include<stdlib.h>
#include <map>

struct WoeidManager
{
	std::map<std::string, Woeid> Manager;
	std::mutex lock;

	void add(Woeid data)
	{
		std::unique_lock<std::mutex> l(lock);
		std::string key = data.getCity();
		Manager[key] = data;
		printf("Add Complete : %s \n", key.c_str());
		l.unlock();
	}

	void find(std::string city)
	{
		std::unique_lock<std::mutex> l(lock);
		Woeid w = Manager[city];
		w.toString();
		l.unlock();
	}
};

void csvInsert(int id, WoeidManager& manager, std::string filename) {
	std::vector<Woeid> WoeidList=WoeidImporter().loadCSV(filename);
	for (int i = 0; i < WoeidList.size(); ++i) {
		manager.add(WoeidList[i]);
	}
}

int main(int argc, char** argv)
{
	WoeidManager manager; 

	std::thread th[3];
	th[0] = std::thread(csvInsert, 0, std::ref(manager), "WOEIDLIST1.csv");
	th[1] = std::thread(csvInsert, 1, std::ref(manager), "WOEIDLIST2.csv");
	th[2] = std::thread(csvInsert, 2, std::ref(manager), "WOEIDLIST3.csv");

	Sleep(5000);

	th[0].join();
	th[1].join();
	th[2].join();

	return 0;
}