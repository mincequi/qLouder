#ifndef FASTMATH_H
#define FASTMATH_H

#include <cstdint>

// This is used to prevent denormalization.
const double vsa = 1.0 / 4294967295.0; // for double as float

// These constants are so important, I made my own copy. If you improve
// the resolution of double be sure to add more significant digits to these.
const double kPi    = 3.1415926535897932384626433832795;
const double kPi_2  = 1.57079632679489661923;
const double kLn2   = 0.693147180559945309417;
const double kLn10  = 2.30258509299404568402;

// Three approximations for both sine and cosine at a given angle.
// The faster the routine, the larger the error.
// From http://lab.polygonal.de/2007/07/18/fast-and-accurate-sinecosine-approximation/

// Tuned for maximum pole stability. r must be in the range 0..pi
// This one breaks down considerably at the higher angles. It is
// included only for educational purposes.
inline void fastestsincos(double r, double *sn, double *cs)
{
    const double c=0.70710678118654752440; // sqrt(2)/2
    double v=(2-4*c)*r*r+c;
    if (r<kPi_2)
    {
        *sn=v+r; *cs=v-r;
    }
    else
    {
        *sn=r+v; *cs=r-v;
    }
}

// Lower precision than ::fastsincos() but still decent
inline void fastersincos(double x, double *sn, double *cs)
{
    //always wrap input angle to -PI..PI
    if              (x < -kPi) x += 2*kPi;
    else if (x >  kPi) x -= 2*kPi;
    //compute sine
    if (x < 0)      *sn = 1.27323954 * x + 0.405284735 * x * x;
    else            *sn = 1.27323954 * x - 0.405284735 * x * x;
    //compute cosine: sin(x + PI/2) = cos(x)
    x += kPi_2;
    if (x > kPi) x -= 2*kPi;
    if (x < 0)      *cs = 1.27323954 * x + 0.405284735 * x * x;
    else            *cs = 1.27323954 * x - 0.405284735 * x * x;
}

// Slower than ::fastersincos() but still faster than
// sin(), and with the best accuracy of these routines.
inline void fastsincos(double x, double *sn, double *cs)
{
    double s, c;
    //always wrap input angle to -PI..PI
    if (x < -kPi) x += 2*kPi;
    else if (x >  kPi) x -= 2*kPi;
    //compute sine
    if (x < 0)
    {
        s = 1.27323954 * x + .405284735 * x * x;
        if (s < 0)      s = .225 * (s * -s - s) + s;
        else            s = .225 * (s *  s - s) + s;
    }
    else
    {
        s = 1.27323954 * x - 0.405284735 * x * x;
        if (s < 0)      s = .225 * (s * -s - s) + s;
        else            s = .225 * (s *  s - s) + s;
    }
    *sn=s;
    //compute cosine: sin(x + PI/2) = cos(x)
    x += kPi_2;
    if (x > kPi) x -= 2*kPi;
    if (x < 0)
    {
        c = 1.27323954 * x + 0.405284735 * x * x;
        if (c < 0)      c = .225 * (c * -c - c) + c;
        else            c = .225 * (c *  c - c) + c;
    }
    else
    {
        c = 1.27323954 * x - 0.405284735 * x * x;
        if (c < 0)      c = .225 * (c * -c - c) + c;
        else            c = .225 * (c *  c - c) + c;
    }
    *cs=c;
}

// Faster approximations to sqrt()
//      From http://ilab.usc.edu/wiki/index.php/Fast_Square_Root
//      The faster the routine, the more error in the approximation.

// Log Base 2 Approximation
// 5 times faster than sqrt()

inline float fastsqrt1(float x)
{
    union { std::int32_t i; float x; } u;
    u.x = x;
    u.i = (int32_t(1)<<29) + (u.i >> 1) - (int32_t(1)<<22);
    return u.x;
}

inline double fastsqrt1(double x)
{
    union { int64_t i; double x; } u;
    u.x = x;
    u.i = (int64_t(1)<<61) + (u.i >> 1) - (int64_t(1)<<51);
    return u.x;
}

// Log Base 2 Approximation with one extra Babylonian Step
// 2 times faster than sqrt()
inline float fastsqrt2(float x)
{
    float v = fastsqrt1(x);
    v = 0.5f * (v + x/v); // One Babylonian step
    return v;
}

inline double fastsqrt2(const double x)
{
    double v = fastsqrt1(x);
    v = 0.5f * (v + x/v); // One Babylonian step
    return v;
}

// Log Base 2 Approximation with two extra Babylonian Steps
// 50% faster than sqrt()
inline float fastsqrt3(float x)
{
    float v = fastsqrt1(x);
    v =                v + x/v;
    v = 0.25f* v + x/v; // Two Babylonian steps
    return v;
}

inline double fastsqrt3(const double x)
{
    double v = fastsqrt1(x);
    v =                v + x/v;
    v = 0.25 * v + x/v; // Two Babylonian steps
    return v;
}


#endif // FASTMATH_H
