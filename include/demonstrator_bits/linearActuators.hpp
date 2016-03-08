#pragma once

// C++ standard library
#include <vector>

// Demonstrator
#include "demonstrator_bits/extensionSensors.hpp"
#include "demonstrator_bits/servoControllers.hpp"
namespace demo {
  class Pin;
}

namespace demo {

  /**
   * Represents an array of [INSERT PRODUCT NAME HERE][1] linear actuators.
   *
   * Each actuator can be instructed to approach a certain extension, and queried for its current extension. These features are implemented in the classes `ServoControllers` and `ExtensionSensors` and combined in this one.
   *
   * [1]: http://INSERT-PRODUCT-PAGE-HERE
   */
  class LinearActuators {
   public:
    explicit LinearActuators(
        std::vector<Pin> servoControllerDirectionPins,
        const std::vector<unsigned int>& servoControllerChannels,
        const std::vector<unsigned int>& extensionSensorChannels);

    /**
     * Let each actuator approach its new position. This method blocks until all actuators have reached their extension!
     *
     * Both `extensions` and `speeds` need to be between in range [0, 1]. Both values refer to a percentage of their respective intervals: [minimalExtension, maximalExtension] and [minimalSpeeds, maximalSpeeds].
     */
    void setExtensions(
        const std::vector<double>& extensions,
        const std::vector<double>& speeds);

    void setMinimalExtensions(
        const std::vector<double>& minimalExtensions);
    std::vector<double> getMinimalExtensions() const;

    void setMaximalExtensions(
        const std::vector<double>& maximalExtensions);
    std::vector<double> getMaximalExtensions() const;

    void setMinimalSpeeds(
        const std::vector<double>& minimalSpeeds);
    std::vector<double> getMinimalSpeeds() const;

    void setMaximalSpeeds(
        const std::vector<double>& maximalSpeeds);
    std::vector<double> getMaximalSpeeds() const;

   protected:
    const ServoControllers servoControllers_;
    const ExtensionSensors extensionSensors_;

    /**
     * Minimal/maximal allowed extension of each actuator, in cm.
     *
     * Refers only to the movement margin, not the total length of the actuator.
     */
    std::vector<double> minimalExtensions_;
    std::vector<double> maximalExtensions_;

    /**
     * Minimal/maximal allowed extension speed of each actuator, in cm/second.
     */
    std::vector<double> minimalSpeeds_;
    std::vector<double> maximalSpeeds_;
  };
}