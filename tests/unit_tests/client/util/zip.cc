/**                                                                                           //
 * Copyright (c) 2015-2016, The Kovri I2P Router Project                                      //
 *                                                                                            //
 * All rights reserved.                                                                       //
 *                                                                                            //
 * Redistribution and use in source and binary forms, with or without modification, are       //
 * permitted provided that the following conditions are met:                                  //
 *                                                                                            //
 * 1. Redistributions of source code must retain the above copyright notice, this list of     //
 *    conditions and the following disclaimer.                                                //
 *                                                                                            //
 * 2. Redistributions in binary form must reproduce the above copyright notice, this list     //
 *    of conditions and the following disclaimer in the documentation and/or other            //
 *    materials provided with the distribution.                                               //
 *                                                                                            //
 * 3. Neither the name of the copyright holder nor the names of its contributors may be       //
 *    used to endorse or promote products derived from this software without specific         //
 *    prior written permission.                                                               //
 *                                                                                            //
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY        //
 * EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF    //
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL     //
 * THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       //
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,               //
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    //
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,          //
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF    //
 * THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               //
 */

#define BOOST_TEST_DYN_LINK

#include <boost/test/unit_test.hpp>

#include <algorithm>
#include <cstdint>
#include <memory>
#include <string>
#include <vector>

#include "util/zip.h"

BOOST_AUTO_TEST_SUITE(ZIP);

struct ZIPFixture {
  // A complete, healthy, ZIP stream
  const std::vector<unsigned char> good_bytes {
    // 0-3: Header signature
    0x50, 0x4b, 0x03, 0x04,
    // 4-5: Unused (version needed to extract)
    0x14, 0x00,
    // 6-7: General purpose bit flag
    0x00, 0x00,
    // 8-9: Compression method
    0x08, 0x00,
    // 10-11: Unused (last mod file time)
    0x91, 0x19,
    // 12-13: Unused (last mod file date)
    0x87, 0x48,
    // 14-17: CRC-32
    0x4b, 0x29, 0xda, 0xdd,
    // 18-21: Compressed size
    0x88, 0x02, 0x00, 0x00,
    // 22-25: Uncompressed size
    0x9f, 0x02, 0x00, 0x00,
    // 26-27: Filename length
    0x3b, 0x00,
    // 28-29: Unused (extra field length)
    0x1c, 0x00,
    // 30-xx+: Content and/or data descriptor content
    0x72, 0x6f, 0x75, 0x74, 0x65, 0x72, 0x49, 0x6e, 0x66, 0x6f, 0x2d, 0x31, 0x6d, 0x41, 0x6d, 0x41,
    0x47, 0x67, 0x6b, 0x6d, 0x54, 0x77, 0x64, 0x67, 0x69, 0x44, 0x51, 0x55, 0x4d, 0x79, 0x37, 0x55,
    0x55, 0x4f, 0x39, 0x71, 0x78, 0x63, 0x56, 0x53, 0x73, 0x73, 0x57, 0x5a, 0x4c, 0x33, 0x71, 0x62,
    0x53, 0x63, 0x51, 0x72, 0x49, 0x67, 0x3d, 0x2e, 0x64, 0x61, 0x74, 0x55, 0x54, 0x09, 0x00, 0x03,
    0x22, 0xd0, 0x05, 0x57, 0x8d, 0x6b, 0x06, 0x57, 0x75, 0x78, 0x0b, 0x00, 0x01, 0x04, 0xe8, 0x03,
    0x00, 0x00, 0x04, 0x0a, 0x00, 0x00, 0x00, 0x93, 0x9d, 0x9c, 0xc9, 0x9a, 0x7b, 0xfe, 0x98, 0x53,
    0xd2, 0x0e, 0xe3, 0xda, 0xb8, 0xe7, 0x3b, 0x8d, 0xd9, 0x2f, 0xd9, 0xe7, 0x26, 0xe9, 0x44, 0x09,
    0xbc, 0x5b, 0xf6, 0xa4, 0x79, 0x81, 0x8e, 0xd8, 0xed, 0x7f, 0xed, 0x81, 0x17, 0xd5, 0xf7, 0x5e,
    0xbb, 0xfd, 0xef, 0xd0, 0xd1, 0xfa, 0x7b, 0xcf, 0xbb, 0xe3, 0xab, 0xfe, 0x26, 0x95, 0xde, 0xb0,
    0x38, 0x77, 0xe9, 0xcf, 0x9c, 0x12, 0x3d, 0xcf, 0x98, 0xe4, 0x87, 0x8b, 0xfc, 0xb6, 0x6f, 0x90,
    0x35, 0xa8, 0x0a, 0x3d, 0xb5, 0x2f, 0x2f, 0xf3, 0x0b, 0xd7, 0xa7, 0x07, 0xcb, 0x14, 0xf8, 0xcf,
    0xcc, 0xbd, 0xd5, 0x61, 0x24, 0xb5, 0xca, 0xd4, 0xeb, 0x5d, 0x6a, 0xe3, 0x3f, 0xf5, 0x50, 0xf6,
    0x7b, 0x79, 0x7b, 0x2d, 0x57, 0x6c, 0x34, 0xbf, 0xca, 0xac, 0xb3, 0x47, 0x59, 0xf6, 0x5a, 0x32,
    0xef, 0x6c, 0x63, 0x17, 0x7f, 0xbe, 0xfe, 0x08, 0xbe, 0xb5, 0xb7, 0xde, 0x76, 0xce, 0x5f, 0x1a,
    0x69, 0xfa, 0x74, 0x93, 0x57, 0xc5, 0xdb, 0xe8, 0x40, 0x46, 0x27, 0xdf, 0x83, 0x57, 0x1f, 0x3f,
    0x6a, 0xee, 0x13, 0xe9, 0x37, 0x5a, 0xf6, 0x43, 0xcf, 0x6f, 0x3b, 0xbb, 0xe4, 0x93, 0x8f, 0x89,
    0xb2, 0x1a, 0x29, 0x2a, 0x92, 0x1b, 0x3d, 0x93, 0x4f, 0x48, 0x6d, 0x50, 0x0b, 0xf3, 0x6b, 0xe3,
    0xe8, 0x71, 0xac, 0x5e, 0x29, 0x1c, 0xf5, 0xeb, 0xc2, 0x2a, 0x03, 0xf5, 0x87, 0x06, 0x6a, 0xb9,
    0xde, 0x4e, 0x5c, 0x46, 0xce, 0xa6, 0x0f, 0xe7, 0x87, 0xd6, 0xd4, 0x2d, 0x8c, 0xcf, 0x0b, 0xc8,
    0xaa, 0x5f, 0x7a, 0x32, 0xf7, 0x75, 0x97, 0x7a, 0xc7, 0xdd, 0xe3, 0x8f, 0x18, 0xef, 0xed, 0xdb,
    0xb6, 0xc1, 0x4a, 0x26, 0xee, 0xc0, 0x93, 0x69, 0x7b, 0xb2, 0xcb, 0xce, 0x6e, 0xe6, 0xdd, 0xef,
    0xc9, 0xbf, 0x31, 0xda, 0x67, 0x7e, 0x8c, 0xdb, 0x89, 0xba, 0xcb, 0x0a, 0x12, 0x49, 0x27, 0x65,
    0xde, 0x32, 0x3e, 0x29, 0xdc, 0x58, 0xa5, 0x1a, 0xb5, 0xe8, 0x92, 0xb8, 0x11, 0x87, 0x68, 0x42,
    0x7c, 0xe8, 0xfb, 0x85, 0x0b, 0x0d, 0xa3, 0xbd, 0x32, 0x0a, 0x8b, 0xad, 0x67, 0x7f, 0x3d, 0x98,
    0xc2, 0xc5, 0xa1, 0x14, 0xc0, 0xf3, 0x61, 0xe3, 0x93, 0x27, 0x96, 0x73, 0x0a, 0x42, 0x95, 0x72,
    0xea, 0xba, 0x4c, 0x8b, 0xa7, 0x14, 0x9c, 0x69, 0x3d, 0x9a, 0x1d, 0xbc, 0xbe, 0x23, 0xd1, 0x60,
    0xd7, 0xd7, 0x66, 0xb9, 0x9d, 0x13, 0x67, 0x6b, 0xe5, 0xb3, 0xde, 0x15, 0x88, 0x3f, 0xf7, 0x38,
    0x9d, 0x89, 0xe1, 0xb0, 0xb5, 0x9b, 0x89, 0xd9, 0x34, 0xa3, 0x7c, 0xad, 0x4f, 0x8e, 0x6f, 0xbe,
    0x7d, 0x5a, 0x68, 0x50, 0x9d, 0x53, 0xca, 0xd0, 0x7b, 0x52, 0xbf, 0x53, 0xb4, 0xd3, 0x47, 0x4f,
    0x6e, 0xca, 0x87, 0xa8, 0xfe, 0x3f, 0x53, 0x4c, 0x0e, 0xab, 0x0a, 0x3c, 0x32, 0xef, 0x4a, 0x79,
    0xe9, 0x9e, 0xc9, 0x65, 0x7a, 0x69, 0x69, 0x93, 0x81, 0x27, 0x2b, 0x03, 0x0b, 0x03, 0x3b, 0x03,
    0x10, 0x30, 0x06, 0xbf, 0xdb, 0xca, 0xfd, 0x8f, 0x89, 0x95, 0x01, 0x0a, 0x98, 0x83, 0x83, 0x43,
    0x19, 0xe2, 0x59, 0x92, 0x13, 0x0b, 0x8a, 0x6d, 0x99, 0x9c, 0x9c, 0xad, 0x59, 0x32, 0xf2, 0x8b,
    0x4b, 0x6c, 0x79, 0x0d, 0x4d, 0x8c, 0xf4, 0x4c, 0xf4, 0x8c, 0x0c, 0x2c, 0xf4, 0x0c, 0x0d, 0x4d,
    0xad, 0x99, 0xb3, 0x53, 0x2b, 0x6d, 0x75, 0x0c, 0x73, 0x1d, 0x73, 0x1d, 0xdd, 0xd3, 0xb3, 0x73,
    0x43, 0xca, 0x53, 0xd2, 0x33, 0x5d, 0x02, 0x43, 0x7d, 0x2b, 0xcd, 0x43, 0x43, 0xfd, 0x2d, 0x0b,
    0x2b, 0x92, 0xc3, 0x82, 0x8b, 0x8b, 0xc3, 0xa3, 0x7c, 0x8c, 0x0b, 0x93, 0x82, 0x93, 0x03, 0x8b,
    0x3c, 0xd3, 0x6d, 0xad, 0x59, 0x0a, 0xf2, 0x8b, 0x4a, 0x6c, 0x59, 0x0d, 0x0d, 0x0c, 0x2d, 0xcd,
    0xad, 0xb9, 0x61, 0xb6, 0xb1, 0xf8, 0x85, 0x38, 0x07, 0x30, 0x28, 0x61, 0xb5, 0x04, 0x45, 0x07,
    0x03, 0x83, 0x0e, 0xd4, 0x4d, 0xbe, 0x41, 0xd6, 0xac, 0x79, 0xa9, 0x25, 0x9e, 0x29, 0xb6, 0x8c,
    0x46, 0xd6, 0x7c, 0x45, 0xf9, 0xa5, 0x25, 0xa9, 0x45, 0x7a, 0x65, 0xa9, 0x45, 0xc5, 0x99, 0xf9,
    0x79, 0xb6, 0x6c, 0x06, 0x7a, 0x96, 0x7a, 0x46, 0xa6, 0xd6, 0x53, 0xba, 0xfe, 0xff, 0xf5, 0x98,
    0x7f, 0x35, 0xe5, 0x53, 0xe2, 0xfb, 0x8e, 0x15, 0x95, 0x4b, 0xef, 0x06, 0x68, 0xb7, 0x59, 0x4e,
    0x16, 0xf9, 0x1a, 0xce, 0xcd, 0x60, 0xf4, 0x8b, 0x6f, 0x51, 0xb6, 0xd1, 0xdc, 0xc6, 0xb5, 0x8e,
    0x4a, 0x2c, 0x7b, 0xd8, 0x44, 0x8e, 0x5c, 0x79, 0x62, 0xc1, 0xbd, 0xc0, 0x68, 0xe5, 0x16, 0x57,
    0xa1, 0x9d, 0x73, 0x4a, 0xfc, 0x98, 0xb5, 0xce, 0x54, 0xaa, 0xff, 0x58, 0xce, 0x02, 0x00, 0x50,
    0x4b, 0x01, 0x02, 0x1e, 0x03, 0x14, 0x00, 0x00, 0x00, 0x08, 0x00, 0x91, 0x19, 0x87, 0x48, 0x4b,
    0x29, 0xda, 0xdd, 0x88, 0x02, 0x00, 0x00, 0x9f, 0x02, 0x00, 0x00, 0x3b, 0x00, 0x18, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xa4, 0x81, 0x00, 0x00, 0x00, 0x00, 0x72, 0x6f, 0x75,
    0x74, 0x65, 0x72, 0x49, 0x6e, 0x66, 0x6f, 0x2d, 0x31, 0x6d, 0x41, 0x6d, 0x41, 0x47, 0x67, 0x6b,
    0x6d, 0x54, 0x77, 0x64, 0x67, 0x69, 0x44, 0x51, 0x55, 0x4d, 0x79, 0x37, 0x55, 0x55, 0x4f, 0x39,
    0x71, 0x78, 0x63, 0x56, 0x53, 0x73, 0x73, 0x57, 0x5a, 0x4c, 0x33, 0x71, 0x62, 0x53, 0x63, 0x51,
    0x72, 0x49, 0x67, 0x3d, 0x2e, 0x64, 0x61, 0x74, 0x55, 0x54, 0x05, 0x00, 0x03, 0x22, 0xd0, 0x05,
    0x57, 0x75, 0x78, 0x0b, 0x00, 0x01, 0x04, 0xe8, 0x03, 0x00, 0x00, 0x04, 0x0a, 0x00, 0x00, 0x00,
    0x50, 0x4b, 0x05, 0x06, 0x00, 0x00, 0x00, 0x00, 0x01, 0x00, 0x01, 0x00, 0x81, 0x00, 0x00, 0x00,
    0xfd, 0x02, 0x00, 0x00, 0x00, 0x00,
  };
};

class ZIPFixtureImpl : ZIPFixture {
  const std::size_t offset, amount;
  const unsigned char byte;

 public:
  /// @brief Implements ZIP unit-test
  ZIPFixtureImpl(
      const std::size_t offset = 0,
      const std::size_t amount = 0,
      const unsigned char byte = {})
      : offset(offset),
        amount(amount),
        byte(byte) {}
  ~ZIPFixtureImpl() {}

  /// @brief Replaces good byte(s) with bad one(s), then implements ZIP
  /// @return False on failure
  bool Unzip() {
    std::vector<unsigned char> bad_bytes = good_bytes;
    std::fill_n(bad_bytes.begin() + offset, amount, byte);
    std::string str(bad_bytes.begin(), bad_bytes.end());
    kovri::util::ZIP zip(str, str.size());
    return zip.Unzip();
  }
};

// First, test that the good stream is valid
BOOST_AUTO_TEST_CASE(GoodZIP) {
  ZIPFixtureImpl zip;
  BOOST_CHECK(zip.Unzip());
}

// Now, perform tests with a bad byte
BOOST_AUTO_TEST_CASE(BadHeader) {
  ZIPFixtureImpl zip(0, 1, 0x41);  // Offset, amount, value
  BOOST_CHECK(!zip.Unzip());
}

BOOST_AUTO_TEST_CASE(DataDescriptorPresent) {
  ZIPFixtureImpl zip(6, 1, 0x08);
  BOOST_CHECK(!zip.Unzip());
}

BOOST_AUTO_TEST_CASE(BadCompressionMethod) {
  ZIPFixtureImpl zip(8, 1, 0x41);
  BOOST_CHECK(!zip.Unzip());
}

BOOST_AUTO_TEST_CASE(BadCRC32) {
  ZIPFixtureImpl zip(16, 1, 0x41);
  BOOST_CHECK(!zip.Unzip());
}

BOOST_AUTO_TEST_CASE(BadCompressedSize) {
  ZIPFixtureImpl zip(18, 1, 0x41);
  BOOST_CHECK(!zip.Unzip());
}

BOOST_AUTO_TEST_CASE(BadUncompressedSize) {
  ZIPFixtureImpl zip(22, 1, 0x41);
  BOOST_CHECK(!zip.Unzip());
}

BOOST_AUTO_TEST_CASE(BadFilenameLength) {
  ZIPFixtureImpl zip(26, 1, 0x41);
  BOOST_CHECK(!zip.Unzip());
}

BOOST_AUTO_TEST_SUITE_END()
