/*
	Copyright (C) 2022 Manuel Weichselbaumer <mincequi@web.de>

	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
*/

#include "FrequencyTable.h"

#include <iostream>
#include <math.h>

template <class T>
FrequencyTable<T>::FrequencyTable(uint8_t _octaveFraction,
                                  T _fMin,
                                  T _fMax) :
	m_octaveFraction(_octaveFraction),
	m_stride(round(24.0/m_octaveFraction)),
	m_fMin(octaveBand(_fMin)),
	m_fMax(octaveBand(_fMax)) {
}

template <class T>
const std::vector<T>& FrequencyTable<T>::frequencies() {
	if (!m_frequencies.empty()) return m_frequencies;

	const uint8_t stride = round(24.0/m_octaveFraction);
	for (size_t i = m_fMin; i < frequencyTable.size();  i += stride) {
		if (i >= m_fMin && i <= m_fMax) {
			m_frequencies.push_back(frequencyTable.at(i));
		}
	}
	return m_frequencies;
}

template <class T>
std::vector<double> FrequencyTable<T>::interpolate(const std::map<double, double>& in, bool shiftToZero) {
    if (in.empty()) return {};

    std::vector<double> out;
	out.reserve(frequencies().size());
	for (auto i = m_fMin; i <= m_fMax; i += m_stride) {

        // Check what "they" have to offer for "our" frequency band
		auto lower = in.lower_bound(lowerFrequency(i));
		auto upper = in.upper_bound(upperFrequency(i));
		auto distance = std::distance(lower, upper);

        // Case 1: multiple values from "in" for our current band -> compute average
		if (distance > 1) {
            T v = 0.0;
			for (auto it = lower; it != upper; ++it) {
				v += it->second;
			}
			v /= distance;
			out.push_back(v);
			continue;
		}

        // Case 2: no values from "in" for our current band and at the beginning of "in" -> add 0.0
        if (distance < 1 && (lower == in.begin() || upper == in.end())) {
            out.push_back(0.0);
            continue;
        }

        // Case 3: exactly one value and exact match
        if (lower->first == frequencyTable.at(i)) {
            out.push_back(lower->second);
            continue;
        }

        // Case 4: exactly one value from "in" for our current band and at the beginning of "in" -> add value
        if (lower == in.begin() || upper == in.end()) {
            out.push_back(lower->second);
            continue;
        }

        // Case 5: exactly one or no values -> interpolate
        lower = in.lower_bound(frequencyTable.at(i));
        upper = in.upper_bound(frequencyTable.at(i));
        --lower;

        const T x = frequencyTable.at(i);
        const T x0 = lower->first;
        const T x1 = upper->first;
        const T xlog = log(x);
        const T x0log = log(x0);
        const T x1log = log(x1);
        const T f0 = lower->second;
        const T f1 = upper->second;
        const T v = (f0 * (x1log - xlog) + f1 * (xlog - x0log))/(x1log - x0log);

        out.push_back(v);
	}

    if (shiftToZero) {
        double aMax = -144.0;
        for (const auto& a : out) {
            aMax = std::max(a, aMax);
        }
        for (auto& a : out) {
            a -= aMax;
        }
    }

	return out;
}

template <class T>
const std::vector<T>& FrequencyTable<T>::octaveBands() {
	return frequencyTable;
}

template <class T>
uint16_t FrequencyTable<T>::octaveBandsIndex(T f) {
	uint i = 0;
    // Find next greater frequency of f
	for (const auto it : frequencyTable) {
        if (it < f)
            ++i;
        else
            break;
	}

    // Check if we reached the end
	for (; i < frequencyTable.size() - 1; ++i) {
        if (frequencyTable.at(i+1) >= f)
            break;
	}

	double c;
	if (i+1 > frequencyTable.size()) {
		c = frequencyTable.at(i);
	} else {
		c = sqrt(frequencyTable.at(i)*frequencyTable.at(i+1));
	}

    // Check if lower or upper frequency is closer to f
    if (f <= c)
        return i;
    else
        return i+1;
}

template <class T>
uint16_t FrequencyTable<T>::octaveBand(T f) {
	uint i = 0;
	for (const auto it : frequencyTable) {
		if (it < f) ++i; else break;
	}

	for (; i < frequencyTable.size() - m_stride; i += m_stride) {
		if (frequencyTable.at(i+m_stride) >= f) break;
	}

	double c;
	if (i+m_stride > frequencyTable.size()) {
		c = frequencyTable.at(i);
	} else {
		c = sqrt(frequencyTable.at(i)*frequencyTable.at(i+m_stride));
	}
	if (f <= c) return i;
	else return i+1;
}

template <class T>
T FrequencyTable<T>::octaveBandFrequency(T f) {
	return frequencyTable.at(octaveBand(f));
}

template <class T>
T FrequencyTable<T>::lowerFrequency(uint16_t band) {
	return sqrt(frequencyTable.at(band)*frequencyTable.at(band-m_stride)) * 0.995;
}

template <class T>
T FrequencyTable<T>::upperFrequency(uint16_t band) {
	return sqrt(frequencyTable.at(band)*frequencyTable.at(band+m_stride)) * 1.005;
}

// 1/24th octave (or Renard R80) frequency table
template <class T>
const std::vector<T> FrequencyTable<T>::frequencyTable {
	  16.0,     16.5,    17.0,    17.5,    18.0,    18.5,    19.0,    19.5,

	  20.0,     20.6,    21.2,    21.8,    22.4,    23.0,    23.6,    24.3,
	  25.0,     25.8,    26.5,    27.2,    28.0,    29.0,    30.0,    30.7,
	  31.5,     32.5,    33.5,    34.5,    35.5,    36.5,    37.5,    38.7,
	  40.0,     41.2,    42.5,    43.7,    45.0,    46.2,    47.5,    48.7,
	  50.0,     51.5,    53.0,    54.5,    56.0,    57.5,    60.0,    61.5,
	  63.0,     65.0,    67.0,    69.0,    71.0,    73.0,    75.0,    77.5,
	  80.0,     82.5,    85.0,    87.5,    90.0,    92.5,    95.0,    97.5,
	 100.0,    103.0,   106.0,   109.0,   112.0,   115.0,   118.0,   122.0,
	 125.0,    128.0,   132.0,   136.0,   140.0,   145.0,   150.0,   155.0,
	 160.0,    165.0,   170.0,   175.0,   180.0,   185.0,   190.0,   195.0,

	 200.0,    206.0,   212.0,   218.0,   224.0,   230.0,   236.0,   243.0,
	 250.0,    258.0,   265.0,   272.0,   280.0,   290.0,   300.0,   307.0,
	 315.0,    325.0,   335.0,   345.0,   355.0,   365.0,   375.0,   387.0,
	 400.0,    412.0,   425.0,   437.0,   450.0,   462.0,   475.0,   487.0,
	 500.0,    515.0,   530.0,   545.0,   560.0,   575.0,   600.0,   615.0,
	 630.0,    650.0,   670.0,   690.0,   710.0,   730.0,   750.0,   775.0,
	 800.0,    825.0,   850.0,   875.0,   900.0,   925.0,   950.0,   975.0,
	 1000.0,  1030.0,  1060.0,  1090.0,  1120.0,  1150.0,  1180.0,  1220.0,
	 1250.0,  1280.0,  1320.0,  1360.0,  1400.0,  1450.0,  1500.0,  1550.0,
	 1600.0,  1650.0,  1700.0,  1750.0,  1800.0,  1850.0,  1900.0,  1950.0,

	 2000.0,  2060.0,  2120.0,  2180.0,  2240.0,  2300.0,  2360.0,  2430.0,
	 2500.0,  2580.0,  2650.0,  2720.0,  2800.0,  2900.0,  3000.0,  3070.0,
	 3150.0,  3250.0,  3350.0,  3450.0,  3550.0,  3650.0,  3750.0,  3870.0,
	 4000.0,  4120.0,  4250.0,  4370.0,  4500.0,  4620.0,  4750.0,  4870.0,
	 5000.0,  5150.0,  5300.0,  5450.0,  5600.0,  5750.0,  6000.0,  6150.0,
	 6300.0,  6500.0,  6700.0,  6900.0,  7100.0,  7300.0,  7500.0,  7750.0,
	 8000.0,  8250.0,  8500.0,  8750.0,  9000.0,  9250.0,  9500.0,  9750.0,
	10000.0, 10300.0, 10600.0, 10900.0, 11200.0, 11500.0, 11800.0, 12200.0,
	12500.0, 12800.0, 13200.0, 13600.0, 14000.0, 14500.0, 15000.0, 15500.0,
	16000.0, 16500.0, 17000.0, 17500.0, 18000.0, 18500.0, 19000.0, 19500.0,

	20000.0, 20600.0, 21200.0, 21800.0, 22400.0, 23000.0, 23600.0, 24300.0,
	25000.0
};

template class FrequencyTable<float>;
template class FrequencyTable<double>;
