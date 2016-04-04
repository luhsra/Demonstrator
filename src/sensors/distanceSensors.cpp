#include "demonstrator_bits/sensors/distanceSensors.hpp"

// C++ standard library
#include <algorithm>
#include <chrono>
#include <ratio>
#include <thread>
// IWYU pragma: no_include <ext/alloc_traits.h>
// IWYU pragma: no_include <ext/new_allocator.h>

namespace demo {
  DistanceSensors::DistanceSensors(
      std::vector<Pin> pins)
      : Sensors(pins.size()),
        pins_(std::move(pins)) {
    for (std::size_t n = 0; n < numberOfSensors_; ++n) {
      pins_.at(n).set(Pin::Digital::Low);
    }
  }

  std::vector<double> DistanceSensors::measureImplementation() {
    std::vector<double> distances;
    for (std::size_t n = 0; n < numberOfSensors_; ++n) {
      // Step 1: Send 10us pulse trigger.
      pins_.at(n).set(Pin::Digital::High);
      std::this_thread::sleep_for(std::chrono::microseconds(10));
      pins_.at(n).set(Pin::Digital::Low);

      // Step 2: Wait for echo signal. Abort after 250 milliseconds because TODO EXPLAIN THIS
      pins_.at(n).waitForSignalEdge(std::chrono::milliseconds(250));

      // TODO: If we can set `maximalMeasurableDistance_`, we need to adjust the wait time to a dynamic value, too.
      auto echoDuration = std::chrono::duration_cast<std::chrono::microseconds>(pins_.at(n).waitForSignalEdge(std::chrono::milliseconds(2)));

      // Reset the pin to its initial state for the next measurement.
      pins_.at(n).set(Pin::Digital::Low);

      // Step 3: Calculate the distance in meter using the equation in the data
      //         sheet: distance [cm] = us/58
      distances.push_back(echoDuration.count() / 0.0058);
    }

    return distances;
  }
}
