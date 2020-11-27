#pragma once

#include <stdint.h>

#define Uint8(X)	static_cast<uint8_t> (X)
#define Uint16(X)	static_cast<uint16_t>(X)
#define Uint32(X)	static_cast<uint32_t>(X)
#define Uint64(X)	static_cast<uint64_t>(X)

#define Int8 (X)	static_cast<int8_t> (X)
#define Int16(X)	static_cast<int16_t>(X)
#define Int32(X)	static_cast<int32_t>(X)
#define Int64(X)	static_cast<int64_t>(X)

#define Float(X)	static_cast<float> (X)
#define Double(X)	static_cast<double>(X)

#define Cast(X, Y)	static_cast<X>(Y)