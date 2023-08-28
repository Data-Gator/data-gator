#include <unity.h>
#include <Arduino.h>

#include <Wire.h>
#include <SDCard.hpp>
#include <SPI.h>

#define TT_CLK 18
#define TT_MISO 19
#define TT_MOSI 23
#define TT_SS 13

#define LED_BUILTIN 9

SDCard &getSD() {
  static SDCard sd;
  if (!sd) {
    const auto ok = sd.begin(TT_CLK, TT_MISO, TT_MOSI, TT_SS, &SPI);
    TEST_ASSERT_TRUE(ok);
  }
  TEST_ASSERT(sd);
  return sd;
}

const char *kName1 = "/test.txt";
const char *kName2 = "/test2.txt";
const char *kDir = "/testdir";
const char *kDirName1 = "/testdir/test.txt";

void createTestFile(const char *path) {
  auto sd = getSD();

  auto myFile = sd.open(path, FILE_WRITE);
  if (myFile) {
    auto numBytes = myFile.println("testing 1, 2, 3.");
    myFile.close();
  }
}

void testBeginEnd() {
  SDCard sd;

  const auto ok = sd.begin(TT_CLK, TT_MISO, TT_MOSI, TT_SS, &SPI);
  TEST_ASSERT_TRUE(ok);

  sd.end();
}

void testOpen() {
  auto sd = getSD();
  auto myFile = sd.open(kName1, FILE_WRITE);
  TEST_ASSERT(myFile);
}

void testWrite() {
  auto sd = getSD();
  auto myFile = sd.open(kName1, FILE_WRITE);
  if (myFile) {
    auto numBytes = myFile.println("testing 1, 2, 3.");
    myFile.close();
    TEST_ASSERT(!myFile); // Check that close worked.
    // 16 chars plus "\r\n"
    TEST_ASSERT_EQUAL_INT(16 + 2, numBytes);
  }
}

void testRead() {
  // This test assumes testWrite worked!
  auto sd = getSD();
  auto myFile = sd.open(kName1, FILE_READ);
  TEST_ASSERT(myFile);
  if (myFile) {
    const size_t kBufSize = 256;
    uint8_t buffer[kBufSize];
    auto numBytes = myFile.read(buffer, kBufSize);
    myFile.close();
    TEST_ASSERT(!myFile);
    TEST_ASSERT_EQUAL_INT(16 + 2, numBytes);
  }
}

void testExists() {
  auto sd = getSD();
  TEST_ASSERT(sd.exists(kName1));
}

void testRename() {
  auto sd = getSD();
  TEST_ASSERT(sd.exists(kName1));
  sd.rename(kName1, kName2);
  TEST_ASSERT(!sd.exists(kName1));
  TEST_ASSERT(sd.exists(kName2));
}

void testRemove() {
  auto sd = getSD();
  TEST_ASSERT(sd.exists(kName2));
  sd.remove(kName2);
  TEST_ASSERT(!sd.exists(kName2));
}

void testDir() {
  auto sd = getSD();
  if (sd.exists(kDir)) {
    sd.rmdir(kDir);
    if (sd.exists(kDirName1)) {
      sd.remove(kDirName1);
      sd.rmdir(kDir);
    }
  }
  TEST_ASSERT(!sd.exists(kDir));
  auto ok = sd.mkdir(kDir);
  TEST_ASSERT(ok);
  TEST_ASSERT(sd.exists(kDir));
  // Also should work if dir exists.
  TEST_ASSERT(sd.mkdir(kDir));
  TEST_ASSERT(sd.rmdir(kDir));
  TEST_ASSERT(!sd.exists(kDir));

  // Test attempt to rmdir a file.
  createTestFile(kName1);
  TEST_ASSERT(!sd.rmdir(kName1));

  // Now test dir with a file in it.
  TEST_ASSERT(sd.mkdir(kDir));
  createTestFile(kDirName1);
  TEST_ASSERT(!sd.rmdir(kDir));
  TEST_ASSERT(sd.remove(kDirName1));
  TEST_ASSERT(sd.rmdir(kDir));
  TEST_ASSERT(!sd.exists(kDir));
}

void setup() {
    pinMode(14, OUTPUT); // enable power
    digitalWrite(14, HIGH);
  // NOTE!!! Wait for >2 secs
  // if board doesn't support software reset via Serial.DTR/RTS
  delay(2000);

  UNITY_BEGIN();
  RUN_TEST(testBeginEnd);
  RUN_TEST(testOpen);
  RUN_TEST(testWrite);
  RUN_TEST(testRead);
  RUN_TEST(testExists);
  RUN_TEST(testRename);
  RUN_TEST(testRemove);
  RUN_TEST(testDir);
  UNITY_END();

  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() {
  // Blink LED when tests are done
  digitalWrite(LED_BUILTIN, HIGH);
  delay(100);
  digitalWrite(LED_BUILTIN, LOW);
  delay(500);
}
