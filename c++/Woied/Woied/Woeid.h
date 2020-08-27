#pragma once
#ifndef __WOEID_H__
#define __WOEID_H__
#include <string>

class Woeid
{
public:
	Woeid() : id(-1), city(""), country(""), latitude(0.0f), longitude(0.0f) {};
	Woeid(long id, std::string city, std::string country, float latitude, float longitude);
	Woeid(const Woeid& other);
	virtual ~Woeid() {};

	long getID();
	void setID(long id_); 
	std::string getCity(); 
	void setCity(std::string city_);
	std::string getCountry();
	void setCountry(std::string country_);
	float getLatitude();
	void setLatitude(float latitude_);
	float getLongitude();
	void setLongitude(float longitude_);
	std::string toString();

private:
	long id;
	std::string city;
	std::string country;
	double latitude;
	double longitude;
};
#endif

