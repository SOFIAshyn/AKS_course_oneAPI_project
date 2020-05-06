#include <iostream>
#include <fstream>
#include <deque>
#include "../headers/read_fasta.h"

void read_fasta(const std::string& path, concurrent_que<std::string>* q) {


    std::ifstream input(path);

    if (!input.good()) {
        std::cerr << "Error opening '" << "'. Bailing out." << std::endl;
        return;
    }

    std::string container;

    std::getline(input, container);
    container.clear();

    auto const chunk_size = std::size_t{BUFSIZ};


    std::string chunk;
    chunk.resize(chunk_size);
    while (
            input.read(&chunk[0], chunk_size) ||
            input.gcount()
            ) {
        container += input.gcount() != chunk_size ? chunk.substr(0, input.gcount()) : chunk;
    }
    input.close();
    q->push(container);
}