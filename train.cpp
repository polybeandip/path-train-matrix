#include "train.h"

Train::Train(std::string target,
             std::string head_sign, 
             std::string line_color, 
             int arrival_sec, 
             std::string arrival_msg)
  : target      {target}
  , head_sign   {head_sign}
  , line_color  {line_color}
  , arrival_sec {arrival_sec}
  , arrival_msg {arrival_msg}
{}

std::ostream& operator<<(std::ostream& os, const Train& train) {
  os << "{head_sign : " 
     << train.head_sign
     << ", "
     << "arrival_msg : "
     << train.arrival_msg
     << "}";

  return os;
}

std::ostream& operator<<(std::ostream& os, const std::vector<Train>& trains) {
  os << "[";
  for (unsigned int i = 0; i < trains.size(); i++) {
    os << trains[i];
    if (i < trains.size() - 1) os << ", ";
  }
  os << "]";
  return os;
}
