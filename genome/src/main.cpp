#include <iostream>
#include <set>
#include "../headers/read_csv.h"
#include "../headers/read_fasta.h"
#include "../headers/ahocorasick.h"
#include <thread>
#include <vector>
#include <chrono>
#include <atomic>
#include <filesystem>

#include <CL/sycl.hpp>
#include <CL/sycl/intel/fpga_extensions.hpp>
#include "../headers/device_selector.h"

namespace fs = std::filesystem;

inline std::chrono::high_resolution_clock::time_point get_current_time_fenced() {
    std::atomic_thread_fence(std::memory_order_seq_cst);
    auto res_time = std::chrono::high_resolution_clock::now();
    std::atomic_thread_fence(std::memory_order_seq_cst);
    return res_time;
}

template<class D>
inline long long to_us(const D &d) {
    return std::chrono::duration_cast<std::chrono::microseconds>(d).count();
}


void readAllFasta(concurrent_que<std::string> *q) {
    auto start = get_current_time_fenced();
    std::string path = "../data/archive";
    for (const auto &entry: fs::directory_iterator(path)) {
        read_fasta(entry.path(), q);
    }
    q->push(" ");
    auto end = get_current_time_fenced();
}


int main() {

    // oneapi

    auto exception_handler = [](cl::sycl::exception_list exceptionList) {
        for (std::exception_ptr const &e : exceptionList) {
            try {
                std::rethrow_exception(e);
            } catch (cl::sycl::exception const &e) {
                // std::terminate() will exit the process, return non-zero, and output a
                // message to the user about the exception
                std::terminate();
            }
        }
    };

    MyDeviceSelector sel;

    auto propList = cl::sycl::property_list{cl::sycl::property::queue::enable_profiling()};
    cl::sycl::queue q(sel, exception_handler, propList);

    std::cout << "Running on "

              << q.get_device().get_info<cl::sycl::info::device::name>()

              << "\n";


    //


    concurrent_que<std::string> qFasta{};
    int numThreads = 7;
    int numFastas = 3;


    auto start_time = get_current_time_fenced();

    // read csv

    std::vector <std::string> st;
    read_csv("../data/markers.csv", st);

    std::cout << "Finished reading csv\n";


    auto read_csv = get_current_time_fenced();

    // read fasta

    std::thread fastaThread{readAllFasta, &qFasta};


    auto bld_st = get_current_time_fenced();
    auto *a = new aho_corasick::AhoCorasick(st);
    auto builded_nomy = get_current_time_fenced();


    std::vector <std::vector<myMap>> allMaps(numFastas);
    std::vector <std::thread> threads;

    int start = 0, end = 0, maxMarker = a->getMaxMarker(), sizeFasta = 0, iter = 0;
    std::string text;
    auto start_thread = get_current_time_fenced();

    std::vector <myMap> &mapPerFasta = allMaps[0];

    while (qFasta.back() != " ") {

        threads.clear();
        mapPerFasta = allMaps[iter];
        mapPerFasta.resize(numThreads);
        text = qFasta.pop();
        sizeFasta = text.size();
        for (int i = 0; i < numThreads; ++i) {
            start = (int) (i * (sizeFasta / numThreads)) - maxMarker;
            end = (int) ((i + 1) * (sizeFasta / numThreads)) + maxMarker;
            threads.emplace_back(&aho_corasick::AhoCorasick::matchWords, a, text, start > 0 ? start : 0,
                                 end < sizeFasta ? end : sizeFasta, std::ref(mapPerFasta[i]));
        }
        for (auto &t: threads) {
            t.join();
        }
        ++iter;
    }
    auto bld_end = get_current_time_fenced();
    fastaThread.join();


    for (auto &map: mapPerFasta) {
        std::cout << "size : " << map.size() << std::endl;
//        for (auto &itr: map) {
//            std::cout << itr.first << " : ";
//            for (auto &w: itr.second) {
//                std::cout << w << " ";
//            }
//            std::cout << std::endl;
//        }
    }

    std::cout << to_us(builded_nomy - bld_st) << std::endl;
    std::cout << to_us(bld_end - builded_nomy) << std::endl;


    auto end_thread = get_current_time_fenced();

    delete (a);


    std::cout << "\nCSV time : " << to_us(read_csv - start_time)
              << "\nBuilding trie time : " << to_us(read_csv - read_csv) <<
              "\nCalculate : " << to_us(end_thread - start_thread);


    std::cout << "\n Total: " << to_us(end_thread - start_time);


    return 0;
}

