#ifndef ORI_FLOATING_POINT_H
#define ORI_FLOATING_POINT_H

#include <limits>
#include <stdint.h>

#define SAME_DOUBLE(a, b) \
    Double(double(a)).almostEqual(Double(double(b)))

/**
    This class represents double-precision IEEE floating-point number
    http://en.wikipedia.org/wiki/IEEE_floating-point_standard

    The class is borrowed from googletest framework
    https://code.google.com/p/googletest/source/browse/trunk/include/gtest/internal/gtest-internal.h

    The purpose of this class is to do more sophisticated number comparison.
    (Due to round-off error, etc, it's very unlikely that two floating-points will be equal exactly.
    Hence a naive comparison by the == operation often doesn't work.)

    Format of IEEE floating-point:

    sign_bit | exponent_bits | fraction_bits
      1 bit       11 bits          52 bits

    The most-significant bit being the leftmost
*/
struct Double
{
public:
    /// Defines the unsigned integer type that has the same size as the floating point number.
    typedef uint64_t Bits;

    /// # of bits in a number.
    static const int _bitCount = 64;

    /// # of fraction bits in a number.
    static const int _fractionBitCount = std::numeric_limits<double>::digits - 1;

    /// # of exponent bits in a number.
    static const int _exponentBitCount = _bitCount - 1 - _fractionBitCount;

    /// The mask for the sign bit.
    static const Bits _signBitMask = Bits(1) << (_bitCount - 1);

    /// The mask for the fraction bits.
    static const Bits _fractionBitMask = ~Bits(0) >> (_exponentBitCount + 1);

    /// The mask for the exponent bits.
    static const Bits _exponentBitMask = ~(_signBitMask | _fractionBitMask);

    /// How many ULP's (Units in the Last Place) we want to tolerate when comparing two numbers.
    /// The larger the value, the more error we allow.A 0 value means that two numbers must be
    /// exactly the same to be considered equal.
    ///
    /// The maximum error of a single floating-point operation is 0.5 units in the last place.
    /// On Intel CPU's, all floating-point calculations are done with 80-bit precision, while
    /// double has 64 bits. Therefore, 4 should be enough for ordinary use.
    ///
    /// See the following article for more details on ULP:
    /// http://randomascii.wordpress.com/2012/02/25/comparing-floating-point-numbers-2012-edition/
    static const int _maxUlps = 4;

    /// Constructs a FloatingPoint from a raw floating-point number.
    ///
    /// On an Intel CPU, passing a non-normalized NAN (Not a Number) around may change its bits,
    /// although the new value is guaranteed to be also a NAN. Therefore, don't expect this
    /// constructor to preserve the bits in x when x is a NAN.
    explicit Double(const double& x) { _u._value = x; }

    /// Reinterprets a bit pattern as a floating-point number.
    /// This function is needed to test the AlmostEquals() method.
    static double reinterpretBits(const Bits bits)
    {
        Double d(0);
        d._u._bits = bits;
        return d._u._value;
    }

    /// Returns the floating-point number that represent positive infinity.
    static double infinity() { return reinterpretBits(_exponentBitMask); }

    /// Returns the maximum representable finite floating-point number.
    static double max() { return std::numeric_limits<double>::max(); }

    /// Returns the floating-point number that represent NAN (not a number).
    static double nan() { return reinterpretBits(_exponentBitMask | 1); }

    /// Returns the bits that represents this number.
    const Bits& bits() const { return _u._bits; }

    /// Returns the exponent bits of this number.
    Bits exponentBits() const { return _exponentBitMask & _u._bits; }

    /// Returns the fraction bits of this number.
    Bits fractionBits() const { return _fractionBitMask & _u._bits; }

    /// Returns the sign bit of this number.
    Bits signBit() const { return _signBitMask & _u._bits; }

    /// Returns true iff this is NAN (not a number).
    /// It's a NAN if the exponent bits are all ones and the fraction bits are not entirely zeros.
    bool isNan() const
    {
        return (exponentBits() == _exponentBitMask) && (fractionBits() != 0);
    }

    /// Returns true iff this is infinite.
    /// It's a Inf if the exponent bits are all ones and the fraction bits are entirely zeros.
    bool isInfinity() const
    {
        return (exponentBits() == _exponentBitMask) && (fractionBits() == 0);
    }

    /// Returns true iff this number is at most _maxUlps ULP's away from rhs.
    /// In particular, this function:
    ///   - returns false if either number is (or both are) NAN.
    ///   - treats really large numbers as almost equal to infinity.
    ///   - thinks +0.0 and -0.0 are 0 DLP's apart.
    /// The IEEE standard says that any comparison operation involving a NAN must return false.
    bool almostEqual(const Double& rhs) const
    {
        if (isNan() || rhs.isNan()) return false;

        return distanceBetweenSignAndMagnitudeNumbers(_u._bits, rhs._u._bits) <= _maxUlps;
    }

    bool isZero() const
    {
        static Double zero(0);
        return almostEqual(zero);
    }

    bool is(const double& b)
    {
        return almostEqual(Double(b));
    }

    bool isNot(const double& b)
    {
        return !almostEqual(Double(b));
    }

private:
    /// The data type used to store the actual floating-point number.
    union
    {
        double _value;  ///< The raw floating-point number.
        Bits _bits;     ///< The bits that represent the number.
    } _u;

    /// Converts an integer from the sign-and-magnitude representation to the biased representation.
    /// More precisely, let N be 2 to the power of (_bitCount - 1), an integer x is represented by
    /// the unsigned number x + N.
    ///
    /// For instance,
    ///
    ///   -N + 1 (the most negative number representable using sign-and-magnitude) is represented by 1;
    ///   0      is represented by N; and
    ///   N - 1  (the biggest number representable using sign-and-magnitude) is represented by 2N - 1.
    ///
    /// Read http://en.wikipedia.org/wiki/Signed_number_representations
    /// for more details on signed number representations.
    static Bits signAndMagnitudeToBiased(const Bits& sam)
    {
        // sam represents a negative number.
        if (_signBitMask & sam) return ~sam + 1;

        // sam represents a positive number.
        return _signBitMask | sam;
    }

    /// Given two numbers in the sign-and-magnitude representation,
    /// returns the distance between them as an unsigned number.
    static Bits distanceBetweenSignAndMagnitudeNumbers(const Bits& sam1, const Bits& sam2)
    {
        const Bits biased1 = signAndMagnitudeToBiased(sam1);
        const Bits biased2 = signAndMagnitudeToBiased(sam2);
        return (biased1 >= biased2) ? (biased1 - biased2) : (biased2 - biased1);
    }
};

#endif // ORI_FLOATING_POINT_H
