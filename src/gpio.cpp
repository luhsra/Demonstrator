#include "demonstrator_bits/gpio.hpp"
#include "demonstrator_bits/config.hpp" // IWYU pragma: keep

// C++ standard library
#include <iostream>
#include <mutex>
#include <string>

namespace demo {
  // Sets all elements in `ownedPins_` to false.
  decltype(Gpio::ownedPins_) Gpio::ownedPins_({});
  decltype(Gpio::mutex_) Gpio::mutex_;
  
  Pin Gpio::allocatePin(
      const unsigned int pinNumber) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (::demo::isVerbose) {
      std::cout << "Allocating pin " << pinNumber << std::endl;
    }
    
    if (pinNumber < 2 || pinNumber > 27) {
      throw std::domain_error("The pin number must be within [2, 27].");
    } else if(ownedPins_.at(pinNumber - 2)) {
      throw std::runtime_error("The pin must not already be allocated.");
    }

    // `pinNumber` is reduced by 2 as the GPIO pins range from 2 to 27, but the array's indices range from 0 to 25.
    ownedPins_.at(pinNumber - 2) = true;

    return Pin(pinNumber);
  }

  Spi Gpio::allocateSpi() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (::demo::isVerbose) {
      std::cout << "Allocating SPI pins" << std::endl;
    }
    
    if (ownedPins_.at(7) || ownedPins_.at(8) || ownedPins_.at(9) || ownedPins_.at(10) || ownedPins_.at(11)) {
      throw std::runtime_error("The SPI pins must not already be allocated");
    }

    // The pin number is reduced by 2 as the GPIO pins range from 2 to 27, but the array's indices range from 0 to 25.
    ownedPins_.at(7) = true;
    ownedPins_.at(8) = true;
    ownedPins_.at(9) = true;
    ownedPins_.at(10) = true;
    ownedPins_.at(11) = true;

    return Spi();
  }

  I2c Gpio::allocateI2c() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (ownedPins_.at(0) || ownedPins_.at(1)) {
      throw std::runtime_error("The I2C pins must not already be allocated");
    }

    // The pin number is reduced by 2 as the GPIO pins range from 2 to 27, but the array's indices range from 0 to 25.
    ownedPins_.at(0) = true;
    ownedPins_.at(1) = true;

    return I2c();
  }

  Uart Gpio::allocateUart() {
    std::lock_guard<std::mutex> lock(mutex_);

    if (::demo::isVerbose) {
      std::cout << "Allocating UART pins" << std::endl;
    }

    if (ownedPins_.at(12) || ownedPins_.at(13)) {
      throw std::runtime_error("The UART pins must not already be allocated");
    }

    // The pin number is reduced by 2 as the GPIO pins range from 2 to 27, but the array's indices range from 0 to 25.
    ownedPins_.at(12) = true;
    ownedPins_.at(13) = true;

    return Uart();
  }

  void Gpio::deallocate(
      Pin& pin) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (::demo::isVerbose) {
      std::cout << "Deallocating pin " << pin.pinNumber_ << std::endl;
    }
    
    if (pin.ownsPin_) {
      ownedPins_.at(pin.pinNumber_ - 2) = false;
      pin.ownsPin_ = false;
    }
  }

  void Gpio::deallocate(
      Spi& spi) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (::demo::isVerbose) {
      std::cout << "Deallocating SPI pins" << std::endl;
    }
    
    if (spi.ownsSpi_) {
      ownedPins_.at(2) = false;
      ownedPins_.at(3) = false;
      spi.ownsSpi_ = false;
    }
  }

  void Gpio::deallocate(
      I2c& i2c) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (::demo::isVerbose) {
      std::cout << "Deallocating I2C pins" << std::endl;
    }
    
    if (i2c.ownsI2c_) {
      ownedPins_.at(2) = false;
      ownedPins_.at(3) = false;
      i2c.ownsI2c_ = false;
    }
  }

  void Gpio::deallocate(
      Uart& uart) {
    std::lock_guard<std::mutex> lock(mutex_);

    if (::demo::isVerbose) {
      std::cout << "Deallocating UART pins" << std::endl;
    }
    
    if (uart.ownsUart_) {
      ownedPins_.at(2) = false;
      ownedPins_.at(3) = false;
      uart.ownsUart_ = false;
    }
  }
}
