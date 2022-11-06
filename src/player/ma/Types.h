#pragma once

namespace ma {

enum class StandardSampleRate {
    /* Standard rates need to be in priority order. */
    Rate48000  = 48000,     /* Most common */
    Rate44100  = 44100,

    Rate32000  = 32000,     /* Lows */
    Rate24000  = 24000,
    Rate22050  = 22050,

    Rate88200  = 88200,     /* Highs */
    Rate96000  = 96000,
    Rate176400 = 176400,
    Rate192000 = 192000,

    Rate16000  = 16000,     /* Extreme lows */
    Rate11025  = 11250,
    Rate8000   = 8000,

    Rate352800 = 352800,    /* Extreme highs */
    Rate384000 = 384000,

    RateMin    = Rate8000,
    RateMax    = Rate384000,
    RateCount  = 14         /* Need to maintain the count manually. Make sure this is updated if items are added to enum. */
};

} // namespace ma
