#pragma once

#include "as_unsigned.hh"

namespace bits {
  template <typename T>
  std::size_t count(T var) {
    typename as_unsigned<T>::type v(var);
    std::size_t cnt = 0;
    while (v) {
      v &= v - 1;
      cnt++;
    }
    return cnt;
  }

  template <typename Iter>
  std::size_t count(Iter bg, Iter nd) {
    std::size_t cnt = 0;
    while (bg != nd) {
      cnt += count(*bg++);
    }
    return cnt;
  }

  typedef unsigned bit_off;

  struct rng {
    bit_off hi;
    bit_off lo;

    rng(bit_off h, bit_off l) :
      hi(h), lo(l) {
      assert(hi >= lo);
    }

    rng(bit_off r[2]) :
      hi(r[0]), lo(r[1]) {
      assert(hi > lo);
    }

    static rng one(bit_off o) {
      return rng(o, o);
    }
    
    std::size_t size() const {
      return (hi - lo) + 1;
    }
  };

  std::size_t range_size(bit_off hi, bit_off lo) {
    return (hi - lo) + 1;
  }

  template <typename T>
  T make_mask(bit_off hi, bit_off lo) {
    static const T msk = std::numeric_limits<T>::max();
    std::size_t sz = (hi - lo) + 1;
    return (msk >> (std::numeric_limits<T>::digits - sz)) << lo;
  }

  template <typename T>
  T make_mask(const rng &r) {
    return make_mask<T>(r.hi, r.lo);
  }

  template <typename T>
  void alter(T &var, bit_off hi, bit_off lo, bool val) {
    typedef typename as_unsigned<T>::type u_type;
    u_type msk = make_mask<u_type>(hi, lo);
    var = (val) ? var | msk : var & ~msk;
  }

  template <typename Iter>
  void alter(Iter bg, Iter nd, bit_off hi, bit_off lo, bool val) {
    typedef typename std::iterator_traits<Iter>::value_type IT;
    std::size_t blk_size = std::numeric_limits<typename as_unsigned<IT>::type>::digits;

    // Should optimize by skipping to the first block containing to the lowest bit.
    for (Iter i = bg; i != nd; i++) {
      auto blk_off = std::distance(bg, i) * blk_size; // Block offset in bits.

      // If the bit range starts within the current block.
      if (lo >= blk_off && lo <= (blk_off + blk_size)) {
        auto rem = hi - lo;     // Number of bits remaining in target range.

        // Highest bit of the target range within the block.
        auto blk_hi = (rem / blk_size) ? blk_size - 1 : (lo + hi) % blk_size;
        alter(*i, blk_hi, lo % blk_size, val);
        if ((blk_off + blk_size) >= hi) {
          break;
        }
        lo += blk_size - (lo % blk_size);
      }
    }
  }

  template <typename ST, typename DT>
  void copy(ST src, rng src_rng, DT &dst, bit_off dst_off) {
    typedef typename as_unsigned<ST>::type UST;
    typedef std::numeric_limits<UST> USL;
    typedef typename as_unsigned<DT>::type UDT;
    typedef std::numeric_limits<UDT> UDL;

    assert(src_rng.size() > 0);
    if (src_rng.size() == 0)
      return;
    
    assert(USL::digits >= src_rng.size() + src_rng.lo);

    rng dst_rng(dst_off + src_rng.size() - 1, dst_off);
    assert(UDL::digits > dst_rng.hi);

    dst &= ~bits::make_mask<UDT>(dst_rng);
    src = (src & make_mask<UST>(src_rng)) >> src_rng.lo;
    dst |= src << dst_rng.lo;
  }
}
