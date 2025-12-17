#include "poller.h"
#include "train.h"

#include <sstream>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>

//temp
#include <iostream>

#define ADDR "https://www.panynj.gov/bin/portauthority/ridepath.json"

using json = nlohmann::json;

Poller& Poller::obtain() {
  static Poller poller;

  return poller;
}

Poller::Poller() {
  curl_global_init(CURL_GLOBAL_ALL);
}

Poller::~Poller() {
  curl_global_cleanup();
}

std::vector<Train> Poller::getTrains() {
  std::lock_guard<std::mutex> lock(mtx);
  return trains;
}

size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  std::ostringstream* serializedJSON = static_cast<std::ostringstream*>(userp);

  serializedJSON->write(static_cast<char*>(contents), realsize);

  if (serializedJSON->good()) return realsize;
  
  std::cout << "ERROR mem_cb(): "
            << "not enough memory (serializedJSON.good() == False)"
            << std::endl;

  return 0; // is this right?
}

CURLcode get(std::ostringstream& serializedJSON) {
  CURL* handle = curl_easy_init();

  curl_easy_setopt(handle, CURLOPT_URL, ADDR);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, mem_cb);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, static_cast<void*>(&serializedJSON));
  curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  CURLcode rc = curl_easy_perform(handle);

  curl_easy_cleanup(handle);

  if(rc != CURLE_OK) {
    std::cout << "ERROR curl_easy_perform(): "
              << curl_easy_strerror(rc)
              << std::endl;
  }

  return rc;
}

std::vector<Train> parseJSON(std::string serial) {
  std::vector<Train> trains;
  
  try {
    json data = json::parse(serial);
    for (auto& station : data.at("results")) {
      if (station.at("consideredStation") != "JSQ") continue;

      for (auto& dest : station.at("destinations")) {
        if (dest.at("label") != "ToNY") continue;

        for (auto& msg : dest.at("messages")) {
          std::string sec = msg.at("secondsToArrival");
          trains.emplace_back(
            msg.at("target"),   
            msg.at("headSign"), 
            msg.at("lineColor"),
            std::stoi(sec),
            msg.at("arrivalTimeMessage")
          );
        }

        break;
      }

      break;
    }
  }
  catch (const std::exception& e) {
    std::cout << "ERROR parseJSON(): " 
              << e.what() 
              << std::endl;
  }

  return trains;
}

void Poller::poll() {
  using namespace std::chrono_literals;

  static int count = 0;

  while (!interrupt) {
    if (count == 0) {
      std::ostringstream serializedJSON;
      serializedJSON.str("");
      serializedJSON.clear();

      CURLcode rc = get(serializedJSON);
      if (rc == CURLE_OK) {
        std::vector<Train> newtrains = parseJSON(serializedJSON.str());
        
        mtx.lock();
        std::swap(newtrains, trains);
        mtx.unlock();
      }
    }
  
    count = (count + 1) % 60; // `count` hits 0 every 30 seconds
    std::this_thread::sleep_for(500ms); 
  }
}
