#include "poller.h"

#include <sstream>
#include <thread>
#include <chrono>
#include <curl/curl.h>

//temp
#include <iostream>

#define ADDR "https://www.panynj.gov/bin/portauthority/ridepath.json"

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

void Poller::poll() {
  while (!interrupt) {
    std::ostringstream serializedJSON;
    
    serializedJSON.str("");
    serializedJSON.clear();

    CURLcode rc = get(serializedJSON);

    std::this_thread::sleep_for(std::chrono::seconds(1));
  }
}
