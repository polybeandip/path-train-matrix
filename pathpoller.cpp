#include "pathpoller.h"
#include "train.h"

#include <sstream>
#include <thread>
#include <chrono>
#include <curl/curl.h>
#include <nlohmann/json.hpp>
#include <boost/log/trivial.hpp>

#define ADDR "https://www.panynj.gov/bin/portauthority/ridepath.json"
#define FREQ 10 // frequency to poll `ADDR` in seconds

using json = nlohmann::json;

PathPoller& PathPoller::obtain() {
  static PathPoller poller;

  return poller;
}

PathPoller::PathPoller() {
  CURLcode rc = curl_global_init(CURL_GLOBAL_ALL);

  if (rc != CURLE_OK) {
    BOOST_LOG_TRIVIAL(fatal) << "curl_global_init(): "
                             << curl_easy_strerror(rc);
    throw std::runtime_error("PathPoller(): curl_global_init failed");
  }
}

PathPoller::~PathPoller() {
  curl_global_cleanup();
}

std::vector<Train> PathPoller::getTrains() {
  std::lock_guard<std::mutex> lock(mtx);
  return trains;
}

size_t mem_cb(void *contents, size_t size, size_t nmemb, void *userp) {
  size_t realsize = size * nmemb;
  std::ostringstream* serializedJSON = static_cast<std::ostringstream*>(userp);

  serializedJSON->write(static_cast<char*>(contents), realsize);

  if (serializedJSON->good()) return realsize;
  
  BOOST_LOG_TRIVIAL(error) << "mem_cb(): not enough memory "
                              "(serializedJSON.good() == false)";

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

  return rc;
}

std::vector<Train> parseJSON(std::string serial) {
  std::vector<Train> trains;
  
  try {
    const json data = json::parse(serial);

    const json* station = nullptr;
    for (const auto& s : data.at("results"))
      if (s.at("consideredStation") == "JSQ") {
        station = &s;
        break;
      }
    if (!station) throw std::runtime_error("no 'JSQ' station found");

    const json* dest = nullptr;
    for (const auto& d : (*station).at("destinations"))
      if (d.at("label") == "ToNY") {
        dest = &d;
        break;
      }
    if (!dest) throw std::runtime_error("no 'ToNY' destination found");

    for (const auto& msg : (*dest).at("messages")) {
      std::string sec = msg.at("secondsToArrival");
      trains.emplace_back(
        msg.at("target"),   
        msg.at("headSign"), 
        msg.at("lineColor"),
        std::stoi(sec),
        msg.at("arrivalTimeMessage")
      );
    }
  }
  catch (const std::exception& e) {
    BOOST_LOG_TRIVIAL(error) << "parseJSON(): " << e.what();
  }

  return trains;
}

void PathPoller::loop() {
  using namespace std::chrono_literals;

  static int count = -1;

  while (!interrupt) {
    std::this_thread::sleep_for(500ms); 

    count = (count + 1) % (2 * FREQ);

    if (count != 0) continue;

    std::ostringstream serializedJSON;
    serializedJSON.str("");
    serializedJSON.clear();

    CURLcode rc = get(serializedJSON);

    switch (rc) {
      case CURLE_OK: {
        BOOST_LOG_TRIVIAL(info) << "loop(): JSON="
                                << serializedJSON.str();

        std::vector<Train> newtrains = parseJSON(serializedJSON.str());
        
        BOOST_LOG_TRIVIAL(info) << "loop(): newtrains="
                                << newtrains;
        
        mtx.lock();
        std::swap(newtrains, trains);
        mtx.unlock();
        break;
      }

      default:
        BOOST_LOG_TRIVIAL(error) << "curl_easy_perform(): "
                                 << curl_easy_strerror(rc);
    }
  }
}

void PathPoller::poll() {
  BOOST_LOG_TRIVIAL(info) << "poll(): start";

  try { 
    loop(); 
  }
  catch (...) {
    // do nothing.
  }

  BOOST_LOG_TRIVIAL(info) << "poll(): end";
}
