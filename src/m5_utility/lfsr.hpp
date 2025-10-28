/*
 * SPDX-FileCopyrightText: 2025 M5Stack Technology CO LTD
 *
 * SPDX-License-Identifier: MIT
 */
/*!
  @file lfsr.hpp
  @brief Linear feedback shift register (LFSR）
*/
#ifndef M5_UTILITY_LFSR_HPP
#define M5_UTILITY_LFSR_HPP

#include <bitset>
#include <climits>

namespace m5 {
namespace utility {

/*!
  @class FibonacciLFSR_Right
  @brief Fibonacci LFSRs (right-shift version)
  @tparam N Value bit width (1..64)
  @tparam Taps Tap bit indexes (polynomial exponent)
  @note FibonacciLFSR_Right<16,16,14,13,11> means 16bits x^16 + x^14 + x^13 + x^11 + 1
 */
template <uint32_t N, uint32_t... Taps>
class FibonacciLFSR_Right {
    static_assert(N > 0, "N must be > 0");
    static_assert(N <= 64, "N must be <= 64");
    static_assert(sizeof...(Taps) >= 1, "At least one tap required");

    // Tap boundary check: 1..N-1
    template <uint32_t...>
    struct all_valid : std::true_type {
        //
    };
    template <uint32_t A, uint32_t... Rest>
    struct all_valid<A, Rest...> : std::integral_constant<bool, (A >= 1 && A <= N) && all_valid<Rest...>::value> {
        //
    };
    static_assert(all_valid<Taps...>::value, "Taps out of range");

protected:
    // XOR of taps; bit index is N - Ts (Ts is exponent)
    template <uint32_t... Ts>
    static bool taps_xor(const std::bitset<N>& s)
    {
        bool r{};
        using swallow = int[];
        (void)swallow{0, (r ^= s.test(N - Ts), 0)...};  //  Swallow idiom
        return r;
    }

public:
    using state_type_t = std::bitset<N>;  //!< State type

    ///@name Constructor
    ///@{
    explicit FibonacciLFSR_Right(const uint64_t seed) noexcept : _state{seed}
    {
    }
    explicit FibonacciLFSR_Right(const state_type_t s) noexcept : _state{s}
    {
    }
    ///@}

    //! @brief Gets the state
    inline const state_type_t& state() const noexcept
    {
        return _state;
    }

    //! @brief Gets the state value
    template <typename UL                                                                  = unsigned long,
              typename std::enable_if<(sizeof(UL) * CHAR_BIT >= 64), std::nullptr_t>::type = nullptr>
    inline uint64_t value() const
    {
        return static_cast<uint64_t>(_state.to_ulong());
    }
    template <typename UL                                                                 = unsigned long,
              typename std::enable_if<(sizeof(UL) * CHAR_BIT < 64), std::nullptr_t>::type = nullptr>
    inline uint64_t value() const
    {
        return _state.to_ullong();
    }

    //! @brief Shift 1 step (Right). Returns output bit (LSB before shift)
    bool step() noexcept
    {
        const bool out = _state.test(0);             // LSB (output)
        const bool fb  = taps_xor<Taps...>(_state);  // feedback
        _state >>= 1;                                // Shift to right
        _state.set(N - 1, fb);                       // Insert feedback into MSB
        return out;
    }

    //! @brief Shift nbit steps and gets
    uint64_t step(const uint32_t nbits) noexcept
    {
        uint64_t v{};
        for (uint32_t i = 0; i < nbits; ++i) {
            v |= (uint64_t(step()) << i);
        }
        return v;
    }

    ///@name Helper
    ///@{
    //!@brief Shift 16 bits and get
    inline uint16_t next16() noexcept
    {
        return (uint16_t)step(16);
    }
    //!@brief Shift 32 bits and get
    inline uint32_t next32() noexcept
    {
        return (uint32_t)step(32);
    }
    //!@brief Shift 64 bits and get
    inline uint64_t next64() noexcept
    {
        return step(64);
    }
    ///@}

protected:
    static inline bool taps_xor_all(const state_type_t& s)
    {
        return taps_xor<Taps...>(s);
    }

    state_type_t _state;
};

/*!
  @class FibonacciLFSR_Left
  @brief Fibonacci LFSR (left-shift version)
  @tparam N Value bit width (1..64)
  @tparam Taps Tap bit indexes
  @note Output bit = MSB, feedback is inserted to LSB (left shift).
 */
template <uint32_t N, uint32_t... Taps>
class FibonacciLFSR_Left {
    static_assert(N > 0, "N must be > 0");
    static_assert(N <= 64, "N must be <= 64");
    static_assert(sizeof...(Taps) >= 1, "At least one tap required");

    // Tap boundary check: 1..N-1
    template <uint32_t...>
    struct all_valid : std::true_type {
        //
    };
    template <uint32_t A, uint32_t... Rest>
    struct all_valid<A, Rest...> : std::integral_constant<bool, (A >= 1 && A <= N) && all_valid<Rest...>::value> {
        //
    };
    static_assert(all_valid<Taps...>::value, "Taps out of range");

protected:
    // XOR of taps; bit index is N - Ts (Ts is exponent)
    template <uint32_t... Ts>
    static bool taps_xor(const std::bitset<N>& s)
    {
        bool r{};
        using swallow = int[];
        //(void)swallow{0, (r ^= s.test(N - Ts), 0)...};  // Swallow idiom
        (void)swallow{0, (r ^= s.test(Ts - 1), 0)...};  // Swallow idiom
        //(void)swallow{0, (r ^= s.test((Ts == N) ? (N - 1) : (N - Ts)), 0)...};        return r;
        return r;
    }

public:
    using state_type_t = std::bitset<N>;

    /// @name Constructor
    /// @{
    explicit FibonacciLFSR_Left(const uint64_t seed) noexcept : _state{seed}
    {
    }
    explicit FibonacciLFSR_Left(const state_type_t s) noexcept : _state{s}
    {
    }
    /// @}

    //! @brief Gets the state
    inline const state_type_t& state() const noexcept
    {
        return _state;
    }

    //! @brief Gets the state value
    template <typename UL                                                                  = unsigned long,
              typename std::enable_if<(sizeof(UL) * CHAR_BIT >= 64), std::nullptr_t>::type = nullptr>
    inline uint64_t value() const
    {
        return static_cast<uint64_t>(_state.to_ulong());
    }
    template <typename UL                                                                 = unsigned long,
              typename std::enable_if<(sizeof(UL) * CHAR_BIT < 64), std::nullptr_t>::type = nullptr>
    inline uint64_t value() const
    {
        return _state.to_ullong();
    }

    //! @brief Shift 1 step (Left). Returns output bit (MSB before shift)
    bool step() noexcept
    {
        const bool out = _state.test(N - 1);         // MSB (output)
        const bool fb  = taps_xor<Taps...>(_state);  // feedback
        _state <<= 1;                                // shift to left
        _state.set(0, fb);                           // insert feedback into LSB
        return out;
    }

    //! @brief Shift nbits steps and pack outputs LSB-first
    uint64_t step(const uint32_t nbits) noexcept
    {
        uint64_t v{};
        for (uint32_t i = 0; i < nbits; ++i) {
            v |= (uint64_t(step()) << i);  // i-th output placed at bit i
        }
        return v;
    }

    /// @name Helpers
    /// @{
    inline uint16_t next16() noexcept
    {
        return (uint16_t)step(16);
    }
    inline uint32_t next32() noexcept
    {
        return (uint32_t)step(32);
    }
    inline uint64_t next64() noexcept
    {
        return step(64);
    }
    /// @}

protected:
    static inline bool taps_xor_all(const state_type_t& s)
    {
        return taps_xor<Taps...>(s);
    }
    state_type_t _state;
};

}  // namespace utility
}  // namespace m5
#endif
