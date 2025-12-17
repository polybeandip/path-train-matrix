#ifndef TRAIN_H
#define TRAIN_H

#include <string>

struct Train {
  std::string target;
  std::string head_sign;
  std::string line_color;
  
  int         arrival_sec;
  std::string arrival_msg;

  Train(std::string target, 
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

  bool operator==(const Train& other) const = default;
  bool operator!=(const Train& other) const = default;
};

#endif // TRAIN_H
