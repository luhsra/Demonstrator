find_path(WIRINGPI_INCLUDE_DIRS NAMES wiringPi.h)
find_library(WIRINGPI_LIBRARIES NAMES wiringPi)

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(wiringPi DEFAULT_MSG WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)

mark_as_advanced(WIRINGPI_LIBRARIES WIRINGPI_INCLUDE_DIRS)
