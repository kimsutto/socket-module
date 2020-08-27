#include "Woeid.h"
#include <cstdarg>
#include <memory>

Woeid::Woeid(long id_, std::string city_, std::string country_, float latitude_, float longitude_)
{
	id = id_;
	city = city_;
	country = country_;
	latitude = latitude_;
	longitude = longitude_;
}

Woeid::Woeid(const Woeid & other) 
{
	id = other.id;
	city = other.city;
	country = other.country;
	latitude = other.latitude;
	longitude = other.longitude;
}

long Woeid::getID() 
{
	return id;
}

void Woeid::setID(long id_) 
{
	id = id_;
}

std::string Woeid::getCity() 
{
	return city;
}

void Woeid::setCity(std::string city_) 
{
	city = city_;
}

std::string Woeid::getCountry() 
{
	return country;
}

void Woeid::setCountry(std::string country_) 
{
	country = country_;
}

float Woeid::getLatitude() 
{
	return latitude;
}

void Woeid::setLatitude(float latitude_) 
{
	latitude = latitude_;
}

float Woeid::getLongitude() 
{
	return longitude;
}

void Woeid::setLongitude(float longitude_) 
{
	longitude = longitude_;
}

// https://rextester.com/WMK79392
std::string format(const char* format, ...)
{
	va_list args;
	va_start(args, format);
#ifdef _MSC_VER
	size_t size = std::snprintf(nullptr, 0, format, args) + 1; // Extra space for '\0'
	std::unique_ptr<char[]> buf(new char[size]);
	std::vsnprintf(buf.get(), size, format, args);
	return std::string(buf.get(), buf.get() + size - 1); // We don't want the '\0' inside
#else
	int size = _vscprintf(format, args);
	std::string result(++size, 0);
	vsnprintf_s((char*)result.data(), size, _TRUNCATE, format, args);
	return result;
#endif
	va_end(args);
}

std::string Woeid::toString()
{
	char temp[256];
	sprintf_s(temp, "%ld,%s,%s,%f,%f", id, city.c_str(), country.c_str(), latitude, longitude);
	std::string t = temp;
	return t;
	//return format("%ld,%s,%s,%f,%f", id, city.c_str(), country.c_str(), latitude, longitude);
}