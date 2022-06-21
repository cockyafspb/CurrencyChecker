#define CURL_STATICLIB
#include "curl.h"

#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <map>
#include <conio.h>

#include "nlohmann/json.hpp"
#include "windows.h"

std::string const url("https://www.cbr-xml-daily.ru/daily_json.js");


std::size_t callback(
    const char* in,
    std::size_t size,
    std::size_t num,
    std::string* out)
{
    const std::size_t totalBytes(size * num);
    out->append(in, totalBytes);
    return totalBytes;
}

void printAverage(std::map <std::string, std::vector<double>> & buf) {
    std::cout << "Average values for each valute:" << std::endl;
    for (auto j : buf) {
        std::cout << j.first << " - ";
        double res = 0;
        for (double x : j.second)
            res += x;
        res /= j.second.size();
        std::cout << res << std::endl;
    }
}

void printMedian(std::map <std::string, std::vector<double>> & buf) {
    std::cout << "Median values for each valute:" << std::endl;
    for (auto & j : buf) {
        std::cout << j.first << " - ";
        sort(j.second.begin(), j.second.end());
        auto size = j.second.size();
        if (size % 2) {
            std::cout << j.second[size / 2] << std::endl;
        }
        else {
            std::cout << (j.second[size / 2] + j.second[size / 2 - 1]) / 2 << std::endl;
        }
    }
}


int main()
{  
    system("chcp 65001 > nul");

    CURL* curl = curl_easy_init();
    if (curl){

        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
      
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, callback);

        std::unique_ptr<std::string> httpData(new std::string());

        curl_easy_setopt(curl, CURLOPT_WRITEDATA, httpData.get());
        
        std::map <std::string, std::vector<double>> buf;

        while (!_kbhit()) {
            CURLcode res = curl_easy_perform(curl);

            if (res != CURLE_OK) {
                fprintf(stderr, "curl_easy_perform() failed: %s\n",
                    curl_easy_strerror(res));
            }
            else {
                auto jsonData = nlohmann::json::parse(httpData->begin(), httpData->end());

                for (auto & value : jsonData["Valute"]) {
                    std::cout << value["Name"] << " - " << value["Value"] << std::endl;
                    buf[value["CharCode"]].push_back(value["Value"]);
                }
                std::cout << std::endl;
            }
            httpData->clear();
            Sleep(10000);
        }

        printAverage(buf);
        std::cout << std::endl;
        printMedian(buf);

        curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        curl = nullptr;
    }
    return 0;
}


