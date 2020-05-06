//
// Created by heeve on 10.04.20.
//

#ifndef AC_ONET_READ_CSV_H
#define AC_ONET_READ_CSV_H

#include "../src/ahocorasick.cpp"
#include "../src/concurrent_q.h"

void read_csv(std::string path, std::vector<std::string>& markers);
#endif //AC_ONET_READ_CSV_H
