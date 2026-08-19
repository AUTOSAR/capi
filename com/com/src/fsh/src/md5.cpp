// Disclaimer
//
// This work (specification and/or software implementation) and the material
// contained in it, as released by AUTOSAR, is for the purpose of information
// only. AUTOSAR and the companies that have contributed to it shall not be
// liable for any use of the work.
//
// The material contained in this work is protected by copyright and other
// types of intellectual property rights. The commercial exploitation of the
// material contained in this work requires a license to such intellectual
// property rights.
//
// This work may be utilized or reproduced without any modification, in any
// form or by any means, for informational purposes only. For any other
// purpose, no part of the work may be utilized or reproduced, in any form
// or by any means, without permission in writing from the publisher.
//
// The work has been developed for automotive applications only. It has
// neither been developed, nor tested for non-automotive applications.
//
// The word AUTOSAR and the AUTOSAR logo are registered trademarks.
// --------------------------------------------------------------------------

/// ================================================================
///
/// File description:
/// ----------------
/// @file       md5.cpp
/// @brief
/// @details
/// @date       2024-09-11
/// @author     mazelin
/// @version    1.2.0
///
/// ================================================================

#include "md5.h"

#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include <cerrno>
#include <climits>
#include <cstdint>
#include <cstdio>
#include <cstdlib>
#include <cstring>

constexpr uint32_t kBlockBytes{64U};                         // 64
constexpr uint32_t kBlockAlignBytes{8U};                     // 8
constexpr uint32_t kWordBytes{4U};                           // 4
constexpr uint32_t kBlockWords{kBlockBytes / kWordBytes};    // 16
constexpr uint32_t kWordBits{kWordBytes * CHAR_BIT};         // 32
constexpr uint32_t kWordBitsLow{3};                          // << 3 for bytes->bits
constexpr uint32_t kWordBitsHigh{kWordBits - kWordBitsLow};  // >> 29 for high bits
constexpr uint32_t kBlockMask{kBlockBytes - 1};              // 63
constexpr uint32_t kPadBits{0x80};                           // 128

// Determine if little-endian
inline bool IsLittleEndian() noexcept
{
    uint32_t _{1};
    return reinterpret_cast< uint8_t* >(&_)[0] == 1;
}

// Byte reversal function optimized version
inline void ByteReverse(uint8_t* start, uint32_t count) noexcept
{
    constexpr uint32_t kShift24{3 * CHAR_BIT};
    constexpr uint32_t kShift16{2 * CHAR_BIT};
    constexpr uint32_t kShift8{1 * CHAR_BIT};
    do {
        uint32_t value{(static_cast< uint32_t >(start[0]) << kShift24) | (static_cast< uint32_t >(start[1]) << kShift16)
                       | (static_cast< uint32_t >(start[2]) << kShift8) | (static_cast< uint32_t >(start[3]))};
        memcpy(start, &value, sizeof(value));
        start += 4;
    } while (--count != 0);
}

struct MD5Context
{
    uint32_t buf[4]{};   // Buffer
    uint32_t bits[2]{};  // Bit count
    uint8_t block[kBlockBytes]{};
    int32_t doByteReverse{};  // Byte reversal flag
};

void MD5Init(MD5Context& ctx) noexcept;
void MD5Update(MD5Context& ctx, uint8_t const* buf, size_t len) noexcept;
void MD5Final(MD5Context& ctx, uint8_t* md5num) noexcept;
// MD5 transform function declaration
void MD5Transform(uint32_t* state, uint32_t const* block) noexcept;

void MD5Init(MD5Context& ctx) noexcept
{
    constexpr uint32_t kInitA{0x67452301U};
    constexpr uint32_t kInitB{0xEFCDAB89U};
    constexpr uint32_t kInitC{0x98BADCFEU};
    constexpr uint32_t kInitD{0x10325476U};

    // Initialize buffer
    ctx.buf[0] = kInitA;
    ctx.buf[1] = kInitB;
    ctx.buf[2] = kInitC;
    ctx.buf[3] = kInitD;

    // Initialize bit count
    ctx.bits[0] = 0;
    ctx.bits[1] = 0;

    // Set byte reversal flag
    ctx.doByteReverse = !IsLittleEndian() ? 1 : 0;
}

void MD5Update(MD5Context& ctx, uint8_t const* buf, size_t len) noexcept
{
    uint32_t t = ctx.bits[0];

    // Update bit count
    if ((ctx.bits[0] = t + (static_cast< uint32_t >(len) << 3)) < t) {
        ctx.bits[1]++;
    }
    ctx.bits[1] += len >> kWordBitsHigh;

    t = (t >> kWordBitsLow) & kBlockMask;  // Calculate current bytes in buffer

    // Process partial block
    if (t != 0) {
        uint8_t* p = static_cast< uint8_t* >(ctx.block) + t;

        t = kBlockBytes - t;
        if (len < t) {
            memcpy(p, buf, len);
            return;
        }
        memcpy(p, buf, t);
        if (ctx.doByteReverse != 0) {
            ByteReverse(ctx.block, kBlockWords);
        }
        MD5Transform(ctx.buf, reinterpret_cast< uint32_t* >(ctx.block));
        buf += t;
        len -= t;
    }

    // Process full 64-byte blocks
    while (len >= kBlockBytes) {
        memcpy(ctx.block, buf, kBlockBytes);
        if (ctx.doByteReverse != 0) {
            ByteReverse(ctx.block, kBlockWords);
        }
        MD5Transform(ctx.buf, reinterpret_cast< uint32_t* >(ctx.block));
        buf += kBlockBytes;
        len -= kBlockBytes;
    }

    // Save remaining data
    memcpy(ctx.block, buf, len);
}

void MD5Final(MD5Context& ctx, uint8_t* md5num) noexcept
{
    uint32_t count = (ctx.bits[0] >> kWordBitsLow) & kBlockMask;
    uint8_t* p     = ctx.block + count;

    // Add padding bits
    *p++  = kPadBits;
    count = kBlockBytes - 1 - count;

    // If remaining space insufficient for length, pad with 0 and process block
    if (count < kBlockAlignBytes) {
        memset(p, 0, count);
        if (ctx.doByteReverse != 0) {
            ByteReverse(reinterpret_cast< uint8_t* >(ctx.block), kBlockWords);
        }
        MD5Transform(ctx.buf, reinterpret_cast< uint32_t* >(ctx.block));

        memset(ctx.block, 0, kBlockBytes - kBlockAlignBytes);
    } else {
        memset(p, 0, count - kBlockAlignBytes);
    }
    if (ctx.doByteReverse != 0) {
        ByteReverse(reinterpret_cast< uint8_t* >(ctx.block), kBlockWords - 2);
    }

    // Add message length
    reinterpret_cast< uint32_t* >(ctx.block)[kBlockWords - 2] = ctx.bits[0];
    reinterpret_cast< uint32_t* >(ctx.block)[kBlockWords - 1] = ctx.bits[1];

    // Final transform
    MD5Transform(ctx.buf, reinterpret_cast< uint32_t* >(ctx.block));

    // If needed, byte reverse the output
    if (ctx.doByteReverse != 0) {
        ByteReverse(reinterpret_cast< uint8_t* >(ctx.buf), sizeof(ctx.buf) / sizeof(ctx.buf[0]));
    }

    // Copy result
    memcpy(md5num, ctx.buf, MD5_NUM_LEN);

    // Clear context
    memset(&ctx, 0, sizeof(ctx));
}

// MD5 transform function constant definitions
#define F1(x, y, z) ((z) ^ ((x) & ((y) ^ (z))))
#define F2(x, y, z) F1((z), (x), (y))
#define F3(x, y, z) ((x) ^ (y) ^ (z))
#define F4(x, y, z) ((y) ^ ((x) | ~(z)))

#define MD5STEP(f, w, x, y, z, data, s)                                                                                \
    ((w) += f((x), (y), (z)) + (data), (w) = (w) << (s) | (w) >> (kWordBits - (s)), (w) += (x))

void MD5Transform(uint32_t* state, uint32_t const* block) noexcept
{
    constexpr int32_t kR7{7};
    constexpr int32_t kR12{12};
    constexpr int32_t kR17{17};
    constexpr int32_t kR22{22};
    constexpr int32_t kR5{5};
    constexpr int32_t kR9{9};
    constexpr int32_t kR14{14};
    constexpr int32_t kR20{20};
    constexpr int32_t kR4{4};
    constexpr int32_t kR11{11};
    constexpr int32_t kR16{16};
    constexpr int32_t kR23{23};
    constexpr int32_t kR6{6};
    constexpr int32_t kR10{10};
    constexpr int32_t kR15{15};
    constexpr int32_t kR21{21};

    uint32_t a = state[0];
    uint32_t b = state[1];
    uint32_t c = state[2];
    uint32_t d = state[3];

    MD5STEP(F1, a, b, c, d, block[0] + 0xD76AA478, kR7);
    MD5STEP(F1, d, a, b, c, block[1] + 0xE8C7B756, kR12);
    MD5STEP(F1, c, d, a, b, block[2] + 0x242070DB, kR17);
    MD5STEP(F1, b, c, d, a, block[3] + 0xC1BDCEEE, kR22);
    MD5STEP(F1, a, b, c, d, block[4] + 0xF57C0FAF, kR7);
    MD5STEP(F1, d, a, b, c, block[5] + 0x4787C62A, kR12);
    MD5STEP(F1, c, d, a, b, block[6] + 0xA8304613, kR17);
    MD5STEP(F1, b, c, d, a, block[7] + 0xFD469501, kR22);
    MD5STEP(F1, a, b, c, d, block[8] + 0x698098D8, kR7);
    MD5STEP(F1, d, a, b, c, block[9] + 0x8B44F7AF, kR12);
    MD5STEP(F1, c, d, a, b, block[10] + 0xFFFF5BB1, kR17);
    MD5STEP(F1, b, c, d, a, block[11] + 0x895CD7BE, kR22);
    MD5STEP(F1, a, b, c, d, block[12] + 0x6B901122, kR7);
    MD5STEP(F1, d, a, b, c, block[13] + 0xFD987193, kR12);
    MD5STEP(F1, c, d, a, b, block[14] + 0xA679438E, kR17);
    MD5STEP(F1, b, c, d, a, block[15] + 0x49B40821, kR22);

    MD5STEP(F2, a, b, c, d, block[1] + 0xF61E2562, kR5);
    MD5STEP(F2, d, a, b, c, block[6] + 0xC040B340, kR9);
    MD5STEP(F2, c, d, a, b, block[11] + 0x265E5A51, kR14);
    MD5STEP(F2, b, c, d, a, block[0] + 0xE9B6C7AA, kR20);
    MD5STEP(F2, a, b, c, d, block[5] + 0xD62F105D, kR5);
    MD5STEP(F2, d, a, b, c, block[10] + 0x02441453, kR9);
    MD5STEP(F2, c, d, a, b, block[15] + 0xD8A1E681, kR14);
    MD5STEP(F2, b, c, d, a, block[4] + 0xE7D3FBC8, kR20);
    MD5STEP(F2, a, b, c, d, block[9] + 0x21E1CDE6, kR5);
    MD5STEP(F2, d, a, b, c, block[14] + 0xC33707D6, kR9);
    MD5STEP(F2, c, d, a, b, block[3] + 0xF4D50D87, kR14);
    MD5STEP(F2, b, c, d, a, block[8] + 0x455A14ED, kR20);
    MD5STEP(F2, a, b, c, d, block[13] + 0xA9E3E905, kR5);
    MD5STEP(F2, d, a, b, c, block[2] + 0xFCEFA3F8, kR9);
    MD5STEP(F2, c, d, a, b, block[7] + 0x676F02D9, kR14);
    MD5STEP(F2, b, c, d, a, block[12] + 0x8D2A4C8A, kR20);

    MD5STEP(F3, a, b, c, d, block[5] + 0xFFFA3942, kR4);
    MD5STEP(F3, d, a, b, c, block[8] + 0x8771F681, kR11);
    MD5STEP(F3, c, d, a, b, block[11] + 0x6D9D6122, kR16);
    MD5STEP(F3, b, c, d, a, block[14] + 0xFDE5380C, kR23);
    MD5STEP(F3, a, b, c, d, block[1] + 0xA4BEEA44, kR4);
    MD5STEP(F3, d, a, b, c, block[4] + 0x4BDECFA9, kR11);
    MD5STEP(F3, c, d, a, b, block[7] + 0xF6BB4B60, kR16);
    MD5STEP(F3, b, c, d, a, block[10] + 0xBEBFBC70, kR23);
    MD5STEP(F3, a, b, c, d, block[13] + 0x289B7EC6, kR4);
    MD5STEP(F3, d, a, b, c, block[0] + 0xEAA127FA, kR11);
    MD5STEP(F3, c, d, a, b, block[3] + 0xD4EF3085, kR16);
    MD5STEP(F3, b, c, d, a, block[6] + 0x04881D05, kR23);
    MD5STEP(F3, a, b, c, d, block[9] + 0xD9D4D039, kR4);
    MD5STEP(F3, d, a, b, c, block[12] + 0xE6DB99E5, kR11);
    MD5STEP(F3, c, d, a, b, block[15] + 0x1FA27CF8, kR16);
    MD5STEP(F3, b, c, d, a, block[2] + 0xC4AC5665, kR23);

    MD5STEP(F4, a, b, c, d, block[0] + 0xF4292244, kR6);
    MD5STEP(F4, d, a, b, c, block[7] + 0x432AFF97, kR10);
    MD5STEP(F4, c, d, a, b, block[14] + 0xAB9423A7, kR15);
    MD5STEP(F4, b, c, d, a, block[5] + 0xFC93A039, kR21);
    MD5STEP(F4, a, b, c, d, block[12] + 0x655B59C3, kR6);
    MD5STEP(F4, d, a, b, c, block[3] + 0x8F0CCC92, kR10);
    MD5STEP(F4, c, d, a, b, block[10] + 0xFFEFF47D, kR15);
    MD5STEP(F4, b, c, d, a, block[1] + 0x85845DD1, kR21);
    MD5STEP(F4, a, b, c, d, block[8] + 0x6FA87E4F, kR6);
    MD5STEP(F4, d, a, b, c, block[15] + 0xFE2CE6E0, kR10);
    MD5STEP(F4, c, d, a, b, block[6] + 0xA3014314, kR15);
    MD5STEP(F4, b, c, d, a, block[13] + 0x4E0811A1, kR21);
    MD5STEP(F4, a, b, c, d, block[4] + 0xF7537E82, kR6);
    MD5STEP(F4, d, a, b, c, block[11] + 0xBD3AF235, kR10);
    MD5STEP(F4, c, d, a, b, block[2] + 0x2AD7D2BB, kR15);
    MD5STEP(F4, b, c, d, a, block[9] + 0xEB86D391, kR21);

    state[0] += a;
    state[1] += b;
    state[2] += c;
    state[3] += d;
}

// Calculate MD5 value from string
int32_t Str2MD5Num(char const* str, uint8_t* md5num) noexcept
{
    MD5Context ctx;

    MD5Init(ctx);
    MD5Update(ctx, reinterpret_cast< uint8_t const* >(str), strlen(str));
    MD5Final(ctx, md5num);

    return 0;
}

// Calculate MD5 value from file
int32_t File2MD5Num(char const* file, uint8_t* md5num) noexcept
{
    int32_t fd = open(file, O_RDONLY | O_CLOEXEC);
    if (fd == -1) {
        return errno;
    }

    MD5Context ctx{};
    MD5Init(ctx);

    uint8_t buf[BUFSIZ]{};
    ssize_t ret{};
    // Read file and update MD5
    while ((ret = read(fd, buf, BUFSIZ)) > 0) {
        MD5Update(ctx, buf, static_cast< uint32_t >(ret));
    }

    // Check read error
    if (ret == -1) {
        close(fd);
        return errno;
    }

    close(fd);
    MD5Final(ctx, md5num);

    return 0;
}

// Convert MD5 value to string representation
int32_t MD5Num2Str(uint8_t const* md5num, char* md5str) noexcept
{
    for (int32_t n = 0; n < MD5_STR_LEN; n++) {
        sprintf(md5str + (n << 1), "%02x", md5num[n]);
    }
    md5str[MD5_STR_LEN] = '\0';

    return 0;
}

// Convert MD5 string to numeric representation
int32_t MD5Str2Num(uint8_t* md5num, char const* md5str) noexcept
{
    if (strlen(md5str) != MD5_STR_LEN) {
        return -1;
    }

    for (int32_t n = 0; n < MD5_NUM_LEN; ++n) {
        uint32_t value{};
        if (sscanf(md5str + (n << 1), "%x", &value) != 1) {  // NOLINT -- compatibility>[cert-err34-c]
            return -1;
        }
        md5num[n] = static_cast< uint8_t >(value);
    }

    return 0;
}