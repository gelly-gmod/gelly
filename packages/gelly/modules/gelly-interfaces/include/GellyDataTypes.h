#ifndef GELLYDATATYPES_H
#define GELLYDATATYPES_H
#include <cstdint>

namespace Gelly {
namespace DataTypes {
using uint = unsigned int;
using uint4 = uint32_t;
using byte = unsigned char;
using ushort = unsigned short;
using ulong = unsigned long;
using ulonglong = unsigned long long;
using longlong = long long;
template <typename T>
using cref = const T&;
}  // namespace DataTypes
}  // namespace Gelly

#endif	// GELLYDATATYPES_H
