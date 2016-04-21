// C++ standard library
#include <chrono>
#include <thread>

// WiringPi
#include <wiringPi.h>

// Demonstrator
#include <demonstrator>

// Application
#include "../commandline.hpp"

void showHelp();
void runCalibration(
    demo::LinearActuators&);
arma::Mat<double> measure(
    demo::LinearActuators&);

int main (const int argc, const char* argv[]) {
  if (hasOption(argc, argv, "-h") || hasOption(argc, argv, "--help")) {
    showHelp();
    // Terminates the program after the help is shown.
    return 0;
  }

  if (hasOption(argc, argv, "--verbose")) {
    ::demo::isVerbose = true;
  }

  // Initialises WiringPi and uses the BCM pin layout.
  // For an overview on the pin layout, use the `gpio readall` command on a Raspberry Pi.
  ::wiringPiSetupGpio();

  demo::ExtensionSensors extensionSensors(demo::Gpio::allocateSpi(), {0, 1, 2, 3, 4, 5}, 0.0, 1.0);
  extensionSensors.setNumberOfSamplesPerMeasurment(1);

  std::vector<demo::Pin> directionPins;
  directionPins.push_back(demo::Gpio::allocatePin(22));
  directionPins.push_back(demo::Gpio::allocatePin(5));
  directionPins.push_back(demo::Gpio::allocatePin(6));
  directionPins.push_back(demo::Gpio::allocatePin(13));
  directionPins.push_back(demo::Gpio::allocatePin(19));
  directionPins.push_back(demo::Gpio::allocatePin(26));
  demo::ServoControllers servoControllers(std::move(directionPins), demo::Gpio::allocateI2c(), {0, 1, 2, 3, 4, 5}, 1.0);

  demo::LinearActuators linearActuators(std::move(servoControllers), std::move(extensionSensors), 0.1, 0.8);
  linearActuators.setAcceptableExtensionDeviation(0.01);

  runCalibration(linearActuators);

  return 0;
}

void showHelp() {
  std::cout << "Usage:\n"
            << "  program [options ...]\n"
            << "    Moves all actuators up by 10%, then expects the user to measure and enter the true extension to calculate adjustments.\n"
            << "\n"
            << "  Options:\n"
            << "         --verbose    Prints additional (debug) information\n"
            << "    -h | --help       Displays this help\n"
            << std::flush;
}

/**
 * Approach extension intervals from , in 10% steps. To measure the behaviour of the motor when working both with and against the weight of the robot, all extension levels are reached twice, both by extending and retracting the linear actuators.
 *
 * Return a Mat<double> with values filled according to `model/README.md`.
 */
void runCalibration(
    demo::LinearActuators& linearActuators) {
  const std::array<double, 7> extensions = {0.1, 0.2, 0.3, 0.4, 0.5, 0.6, 0.7};
  arma::Cube<double> actualMeasuredExtensions(20, extensions.size(), linearActuators.numberOfActuators_);
  arma::Mat<double> expectedMeasuredExtensions(linearActuators.numberOfActuators_, extensions.size());
  
  std::cout << "Starting extension sensor calibration." << std::endl;
  for (std::size_t n = 0; n < extensions.size(); ++n) {
    std::cout << "Extending to " << extensions.at(n) << "m." << std::endl;
    linearActuators.setExtensions(arma::zeros<arma::Row<double>>(linearActuators.numberOfActuators_) + extensions.at(n), arma::ones<arma::Row<double>>(linearActuators.numberOfActuators_));
    linearActuators.waitTillExtensionIsReached(std::chrono::seconds(10));

    for (std::size_t k = 0; k < linearActuators.numberOfActuators_; ++k) {
      std::cout << "Enter measured extension of sensor " << k << ": ";
      std::cin >> expectedMeasuredExtensions(k, n);
    }

    for (std::size_t k = 0; k < actualMeasuredExtensions.n_rows; ++k) {
      const arma::Row<double> measueredExtensions = linearActuators.getExtensions();
      
      for (std::size_t l = 0; l < measueredExtensions.n_elem; ++l) {
        actualMeasuredExtensions.subcube(k, n, l, k, n, l) = measueredExtensions(l);
      }
    }
  }
  std::cout << "Done." << std::endl;
  linearActuators.setExtensions(arma::zeros<arma::Row<double>>(linearActuators.numberOfActuators_) + 0.5, arma::ones<arma::Row<double>>(linearActuators.numberOfActuators_));
  linearActuators.waitTillExtensionIsReached(std::chrono::seconds(10));
  
  for (std::size_t n = 0; n < linearActuators.numberOfActuators_; ++n) {
    static_cast<arma::Mat<double>>(actualMeasuredExtensions.slice(n)).save("actualMeasuredExtensions_sensor" + std::to_string(n) + ".mat", arma::raw_ascii);
  }
  for (std::size_t n = 0; n < expectedMeasuredExtensions.n_rows; ++n) {
    static_cast<arma::Row<double>>(expectedMeasuredExtensions.row(n)).save("expectedMeasuredExtensions_sensor" + std::to_string(n) + ".mat", arma::raw_ascii);
  }
  
  arma::Mat<double> calibration(extensions.size(), linearActuators.numberOfActuators_);
  for (std::size_t n = 0; n < calibration.n_cols; ++n) {
    calibration.col(n) = arma::median(actualMeasuredExtensions.slice(n)).t();
  }
  
  for (std::size_t n = 0; n < calibration.n_rows; ++n) {
    if (n == 0) {
      calibration.row(n) = calibration.row(n) - (calibration.row(n + 1) - calibration.row(n)) / (expectedMeasuredExtensions.row(n + 1) - expectedMeasuredExtensions.row(n)) * (expectedMeasuredExtensions.row(n) + extensions.at(n));
    } else {
      calibration.row(n) = calibration.row(n) - (calibration.row(n) - calibration.row(n - 1)) / (expectedMeasuredExtensions.row(n) - expectedMeasuredExtensions.row(n - 1)) * (expectedMeasuredExtensions.row(n) + extensions.at(n));
    }
  }
  
  calibration.save("extensionSensors.calibration", arma::raw_ascii);
}
