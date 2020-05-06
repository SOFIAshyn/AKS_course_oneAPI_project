#ifndef DEVICESELECTOR_HPP

#define DEVICESELECTOR_HPP

#include <string>

#include <cstring>

#include <iostream>

#include "CL/sycl.hpp"


class MyDeviceSelector : public cl::sycl::device_selector {
public:
    MyDeviceSelector() {}


    virtual int operator()(const cl::sycl::device &device) const override {

        const std::string name = device.get_info<cl::sycl::info::device::name>();

        std::cout << "Trying device: " << name << "..." << std::endl;

        std::cout << "  Vendor: "
                  << device.get_info<cl::sycl::info::device::vendor>() << std::endl;

        if (device.is_gpu()) return 500;

        if (device.is_accelerator()) return 400;

        if (device.is_cpu()) return 300;

        if (device.is_host()) return 100;

        return -1;
    }
};

#endif
