#include "poller.h"

#include <cstring>
#include <thread>
#include <chrono>
#include <curl/curl.h>

//temp
#include <iostream>

#define ADDR "https://www.panynj.gov/bin/portauthority/ridepath.json"

struct Memory {
  char*  buf;
  size_t size;

  Memory() {
    buf  = static_cast<char*>(malloc(1));
    buf[0] = 0; // neccesary?
    size   = 0;
  };

  ~Memory() {
    free(buf);
  };
};

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
  Memory* mem = static_cast<Memory*>(userp);

  mem->buf = static_cast<char*>(realloc(mem->buf, mem->size + realsize + 1));
  if(mem->buf == NULL) {
    std::cout << "ERROR mem_cb(): "
              << "not enough memory (realloc returned NULL)"
              << std::endl;
    return 0;
  }

  memcpy(mem->buf + mem->size, contents, realsize);
  mem->size += realsize;
  mem->buf[mem->size] = 0;

  return realsize;
}

int get(Memory& mem) {
  CURL* handle = curl_easy_init();

  curl_easy_setopt(handle, CURLOPT_URL, ADDR);
  curl_easy_setopt(handle, CURLOPT_WRITEFUNCTION, mem_cb);
  curl_easy_setopt(handle, CURLOPT_WRITEDATA, static_cast<void*>(&mem));
  curl_easy_setopt(handle, CURLOPT_USERAGENT, "libcurl-agent/1.0");

  CURLcode res = curl_easy_perform(handle);

  curl_easy_cleanup(handle);

  if(res != CURLE_OK) {
    std::cout << "ERROR curl_easy_perform(): "
              << curl_easy_strerror(res)
              << std::endl;
  }

  return res;
}

void Poller::poll() {
  while (!interrupt) {
    Memory mem;
    get(mem);
    std::cout << mem.buf << std::endl;
    std::this_thread::sleep_for(std::chrono::seconds(10));
  }
}
