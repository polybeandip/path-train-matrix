#ifndef TRAIN_H
#define TRAIN_H

#include <string>
#include <ostream>
#include <vector>

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
        std::string arrival_msg);

  bool operator==(const Train& other) const = default;
  bool operator!=(const Train& other) const = default;

  friend std::ostream& operator<<(std::ostream& os, const Train& train);
};

std::ostream& operator<<(std::ostream& os, const Train& train);
std::ostream& operator<<(std::ostream& os, const std::vector<Train>& trains);

#endif // TRAIN_H
