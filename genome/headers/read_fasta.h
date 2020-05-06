//
// Created by heeve on 10.04.20.
//

#ifndef AC_ONET_READ_FASTA_H
#define AC_ONET_READ_FASTA_H

#include "../src/concurrent_q.h"

void read_fasta(const std::string &path, concurrent_que<std::string> *q);

#endif //AC_ONET_READ_FASTA_H
