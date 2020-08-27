#define _CRT_SECURE_NO_WARNINGS
#include "WoeidImporter.h"
#include <vector>
#include <iostream>


std::vector<Woeid> WoeidImporter::loadCSV(std::string filename)
{
	// load file & put data into list
	FILE *fp = NULL;
	char command[256];
	printf("filename: %s\n", filename.c_str());
	std::vector<Woeid> aWoeidList;

	if ((fp = fopen(filename.c_str(), "r")) == NULL) {
		printf("file import cannot open file: %s\n", filename.c_str());
	} else {
		while(1) {
			fgets(command, 256, fp);
			if (feof(fp)) {
				break;
			}
			if (0 == strncmp("#", command, strlen("#")) || strlen(command) < 10) {
				// first line contains #
				continue;
			}
			else {
				// parse data using delimiter ","
				// Id,City,Country,Latitude,Longitude
				long id;
				char city[256];
				char country[256];
				float latitute, longitude;
				char* p = strtok(command, ",\n");
				if (p) {
					id = atol(p);
				}
				p = strtok(NULL, ",\n");
				if (p) {
					strcpy(city, p);
				}
				p = strtok(NULL, ",\n");
				if (p) {
					strcpy(country, p);
				}
				p = strtok(NULL, ",\n");
				if (p) {
					latitute = atof(p);
				}
				p = strtok(NULL, ",\n");
				if (p) {
					longitude = atof(p);
				}
				//sscanf(command, "%ld,%[^,],%[^,],%f,%f",
				//	   &id, &city, &country, &latitute, &longitude);
				Woeid* w = new Woeid(id, city, country, latitute, longitude);
				aWoeidList.push_back(*w);
			}
		}
	}
	return aWoeidList;
}
