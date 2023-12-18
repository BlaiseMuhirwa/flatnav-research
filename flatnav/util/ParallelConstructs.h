#pragma once

#include <atomic>
#include <cstdint>
#include <exception>
#include <mutex>
#include <thread>
#include <vector>

namespace flatnav {

/**
 * @brief template for executing a function in parallel using STL's threading
 * library This is preferred in lieu of OpenMP only because it will not require
 * having logic for installing OpenMP on the host system while installing the
 * Python library.
 *
 * @tparam Function
 * @param start_index
 * @param end_index
 * @param num_threads
 * @param function
 */
template <typename Function>
void executeInParallel(uint32_t start_index, uint32_t end_index,
                       uint32_t num_threads, Function function) {
  if (num_threads == 0) {
    throw std::invalid_argument("Invalid number of threads");
  }

  // This needs to be an atomic because mutliple threads will be
  // modifying it concurrently.
  std::atomic<uint32_t> current(start_index);
  std::thread thread_objects[num_threads];

  auto parallel_executor = [&] {
    while (true) {
      uint32_t current_vector_idx = current.fetch_add(1);
      if (current_vector_idx >= end_index) {
        break;
      }
      function(current_vector_idx);
    }
  };

  for (uint32_t id = 0; id < num_threads; id++) {
    thread_objects[id] = std::thread(parallel_executor);
  }
  for (uint32_t id = 0; id < num_threads; id++) {
    thread_objects[id].join();
  }
}

} // namespace flatnav