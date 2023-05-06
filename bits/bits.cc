#include <cassert>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <typeinfo>
#include <vector>

#include "bits.hh"

template <typename T, typename UT>
struct core_test {
  static_assert(sizeof(T) == sizeof(UT), "Differently sized signed/unsigned types.");
  static_assert(std::numeric_limits<T>::is_signed, "T is not signed.");
  static_assert(!std::numeric_limits<UT>::is_signed, "UT is signed.");
                
  static const auto bit_count = std::numeric_limits<UT>::digits;
  static const auto all_ones = std::numeric_limits<UT>::max();
  
  core_test() {
    assert(typeid(UT) == typeid(typename bits::as_unsigned<T>::type));
    assert(bits::count(T(0)) == 0);
    assert(bits::count(T(0)) == bits::count(UT(0)));
    assert(bits::count(T(all_ones)) == bit_count);
    assert(bits::count(T(all_ones)) == bits::count(UT(all_ones)));
    assert(bits::count(T(1)) == 1);
    assert(bits::count(T(1)) == bits::count(UT(1)));
  }  
};

int main() {
  using namespace std;

  core_test<char, unsigned char> char_test;
  core_test<short, unsigned short> short_test;
  core_test<long, unsigned long> long_test;
  core_test<long long, unsigned long long> long_long_test;

  typedef unsigned char uchar;
  typedef unsigned short ushort;
  uchar ucdst = 0;
  ushort usdst = 0;
  
  bits::copy(char(1), bits::rng::one(0), ucdst, 7);
  assert(ucdst == 0x80U);

  bits::copy(char(1), bits::rng::one(0), usdst, 15);  
  assert(usdst = 0x8000U);

  ucdst = 0;
  bits::copy(ushort(1), bits::rng::one(0), ucdst, 7);
  assert(ucdst == 0x80U);

  usdst = 0;
  bits::copy(char(0x55), bits::rng(7, 0), usdst, 4);
  assert(usdst == 0x550U);
  
  usdst = 0;
  bits::copy(char(0x55), bits::rng(7, 0), usdst, 5);
  assert(usdst == 0xAA0U);

  char v[] = { 0x1, 0x3, 0x7 };
  assert(bits::count(&v[0], &v[3]) == 6);
          
  char v2[] = { 0, 0, 0 };
  bits::alter(&v2[0], &v2[3], 11, 10, 1);
  assert((std::vector<char>(&v2[0], &v2[3]) == std::vector<char>{ 0, 0x3c, 0 }));
  
  unsigned short v1 = 0;
  bits::alter(v1, 3, 0, 1);
  assert(v1 = 0x4);
  return 0;
}
