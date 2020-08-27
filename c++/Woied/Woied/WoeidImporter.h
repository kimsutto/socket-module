#pragma once
#ifndef __WOEID_IMPORTER_H__
#define __WOEID_IMPORTER_H__

#include <vector>
#include "Woeid.h"

class WoeidImporter 
{
public:
	// constructor
	WoeidImporter() {}
	~WoeidImporter() {}

	// import
	std::vector<Woeid> loadCSV(std::string filename);
};

#endif