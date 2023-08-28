/**
 * @brief SDCard Interface Utilities
 *
 * @author Doug Park
 * @file SDCard.hpp
 */
#ifndef SDCARD_H
#define SDCARD_H
/// \brief Support for SD Card
///
/// For now this is a thin wrapper around the SD implementation on ESP32.

//#include <Debug.h>
#include <SD.h>
#include <SPI.h>

// For ESP32 these can be assigned to nearly any GPIO pins.
#define TT_CLK 14
#define TT_MISO 25
#define TT_MOSI 13
#define TT_SS 2

/// \brief Thin wrapper around ESP32 SD card API.
///
/// Only works with ESP32 Arduino framework
///
/// \todo Future versions should support other platforms/frameworks.
class SDCard {
public:
  /// File implements close(), read(), write(),
  /// fs implements open, etc.
  using File = fs::File;

public:
  SDCard() = default;

  /// Operator to test if properly initialized.
  /// \return true if connected to the SD interface.
  explicit operator bool() const { return mCard != nullptr; }

  /// Function to connect to an SD card using the specified pins and
  /// SPI interface.
  ///
  /// For more information on SPI
  /// [https://en.wikipedia.org/wiki/Serial_Peripheral_Interface](https://en.wikipedia.org/wiki/Serial_Peripheral_Interface)
  /// ESP32 docs
  /// [https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html](https://docs.espressif.com/projects/esp-idf/en/latest/esp32/api-reference/peripherals/spi_master.html)
  /// \param clkPin - GPIO pin that provides the clock signal.
  /// \param misoPin - Master in slave out pin.
  /// \param mosiPin - Master out slave in pin.
  /// \param ssPin - Chip/Save select.
  /// \param spi - Pointer to SPI interface.
  ///
  /// \return true if interface setup worked false otherwise.
  bool begin(uint8_t clkPin = TT_CLK, uint8_t misoPin = TT_MISO,
             uint8_t mosiPin = TT_MOSI, uint8_t ssPin = TT_SS,
             SPIClass *spi = &SPI) {
    //debugMsg("SDCard::begin()");

    // Connect to the SD Card API.
    mCard = &SD;

    // Start SPI with the specified pins.
    // Probably need an API that takes the SPI
    spi->begin(clkPin, misoPin, mosiPin, ssPin);

    //debugMsg("Initializing SD card...");

    if (!mCard->begin(ssPin, *spi)) {
      //debugMsg("initialization failed!");
      return false;
    }
    //debugMsg("initialization done.");
    //debugLog(mCard->cardSize());
    return true;
  }

  /// Terminate connection to SD Card and associated SPI interface.
  void end() { mCard->end(); }

  /// @{
  /// Open and possibly create a file.
  ///
  /// \param path Name of file. This API uses the 8.3 DOS file name rules.
  /// The full path of the file needs to be specified. Files in the root of the
  /// device will requre a "/". For example "/test.txt".
  /// \param mode "w", "r", "a". Simple C style modes.
  /// \param create If true allows creation of the
  /// file. true is implied if mode is "w".
  ///
  /// \return File interface.
  File open(const char *path, const char *mode = FILE_READ,
            const bool create = false) {
    return mCard->open(path, mode, create);
  }

  File open(const String &path, const char *mode = FILE_READ,
            const bool create = false) {
    return mCard->open(path, mode, create);
  }
  /// @}

  /// @{
  /// Check to see if path exists.
  ///
  /// \param path path to file or directory.
  ///
  /// \return true if path exists false otherwise.
  bool exists(const char *path) { return mCard->exists(path); }
  bool exists(const String &path) { return mCard->exists(path); }
  /// @}

  /// @{
  /// Permantly delete a file or directory
  ///
  /// \param path path to file or directory.
  ///
  /// \return true if path was deleted false otherwise.
  bool remove(const char *path) { return mCard->remove(path); }
  bool remove(const String &path) { return mCard->remove(path); }
  /// @}

  /// @{
  /// Permantly rename a file or directory
  ///
  /// \param pathFrom path to original file or directory
  /// \param pathTo path to new file or directory
  ///
  /// \return true if path was renamed false otherwise.
  bool rename(const char *pathFrom, const char *pathTo) {
    return mCard->rename(pathFrom, pathTo);
  }
  bool rename(const String &pathFrom, const String &pathTo) {
    return mCard->rename(pathFrom, pathTo);
  }
  /// @}

  /// @{
  /// Create a directory
  ///
  /// If directory doesn't exist it will be created.
  ///
  /// \param path path to directory to create. Naming
  /// conventions are the same as for files. For example
  /// "\testdir" will be in the root of the device.
  ///
  /// \return true if path was created false otherwise.
  bool mkdir(const char *path) { return mCard->mkdir(path); }
  bool mkdir(const String &path) { return mCard->mkdir(path); }
  /// @}

  /// @{
  /// Delete a directory
  ///
  /// path must be an empty directory.
  ///
  /// \param path path to directory to delete.
  ///
  /// \return true if path was deleted false otherwise.
  bool rmdir(const char *path) { return mCard->rmdir(path); }
  bool rmdir(const String &path) { return mCard->rmdir(path); }
  /// @}

private:
  fs::SDFS *mCard;
};

#endif // SDCARD_H
