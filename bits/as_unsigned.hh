#pragma once
// Copyright (c) 2016 Marc Butler <mockbutler@gmail.com>

namespace bits {
  // There must be a better way to do this.
  template <typename T>
  struct as_unsigned {
    static_assert(std::numeric_limits<T>::is_integer, "Must be an integer type.");
    static_assert(!std::numeric_limits<T>::is_signed, "Unhandled signed integer type.");
    typedef T type;
  };

  template <>
  struct as_unsigned<char> {
    typedef unsigned char type;
  };

  template <>
  struct as_unsigned<short> {
    typedef unsigned short type;
  };

  template <>
  struct as_unsigned<int> {
    typedef unsigned int type;
  };

  template <>
  struct as_unsigned<long> {
    typedef unsigned long type;
  };

  template <>
  struct as_unsigned<long long > {
    typedef unsigned long long type;
  };
}
