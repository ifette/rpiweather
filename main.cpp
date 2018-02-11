/*
 * main.cpp
 *
 *  Created on: Sep 25, 2017
 *      Author: ian
 */

#include <iostream>
#include <stdio.h>
//#include <sys/select.h>
#include <curl/curl.h>
#include "fast-cpp-csv-parser/csv.h"

#include <string>
#include <map>

#include <wiringPi.h>

using namespace std;

void setLED(int color) {
	digitalWrite(0,1);
	digitalWrite(1,1);
	digitalWrite(2,1);
	switch (color) {
		case 0: // red
			digitalWrite(0,0);
			break;
		case 1: // Green
			digitalWrite(1,0);
			break;
		case 2: // Blue
			digitalWrite(2,0);
			break;
	}
}

int main() {
	FILE* metar_file = fopen("metars.txt", "w");

	cout << "Hello world" << endl;

	CURL *curl = curl_easy_init();
	if(curl) {
	  CURLcode res;
	  curl_easy_setopt(curl, CURLOPT_URL, "https://aviationweather.gov/adds/dataserver_current/current/metars.cache.csv");
	  curl_easy_setopt(curl, CURLOPT_WRITEDATA, metar_file);
	  res = curl_easy_perform(curl);
	  curl_easy_cleanup(curl);
	  fclose(metar_file);
	}

	io::CSVReader<2> reader("metars.txt");
	map<string, string> weather_map;
	for (int i = 0; i < 5; i++)
		reader.next_line();
	reader.read_header(io::ignore_extra_column,"station_id","flight_category");
	
	string station, category;
	while(reader.read_row(station, category)) {
		weather_map[station] = category;
		//cout << "Station: " << station << " Category: " << category << endl;
	}
	cout << "MMU weather: " << weather_map["KMMU"] << endl;
	cout << "DFW weather: " << weather_map["KDFW"] << endl;
	cout << "PHX weather: " << weather_map["KPHX"] << endl;
	wiringPiSetup();
	pinMode(0, OUTPUT);
	pinMode(1, OUTPUT);
	pinMode(2, OUTPUT);
	setLED(-1);
	while(true) {
		cout << "Airport? ";
		string airport;
		cin >> airport;
		if (weather_map.count(airport) != 1) {
			setLED(-1);
			continue;
		}
		if (weather_map[airport] == "VFR") {
			setLED(1);
		}
		else if (weather_map[airport] == "MVFR") {
			setLED(2);
		}
		else {
			setLED(0);
		}
	}
	
      
	return 0;



}


