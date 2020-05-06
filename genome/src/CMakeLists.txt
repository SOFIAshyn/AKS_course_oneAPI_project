set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS}")
add_executable (genome main.cpp ../file_readers/read_fasta.cpp ../file_readers/read_csv.cpp)
target_link_libraries(genome OpenCL sycl)

add_custom_target(run ./sepia)


