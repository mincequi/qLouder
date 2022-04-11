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

#ifndef FREQUENCYTABLE_H
#define FREQUENCYTABLE_H

#include <cstdint>
#include <map>
#include <vector>

/**
 * @brief The FrequencyTable class
 *
 * This class offers pre-defined frequencies for (fractional) octave bands such
 * as 1/3, 1/6, 1/12 or 1/24 of an octave which are widely used in engineering
 * acoustics.
 *
 * Analyzing a source via arbitrary single frequencies is computational expensive,
 * so the frequency range is divided into frequency bands which cover specific
 * ranges. For this reason, a scale of (fractional) octave bands has been
 * developed.
 */
template<class T>
class FrequencyTable {
public:
	FrequencyTable(uint8_t octaveFraction = 24,
                   T fMin = 20.0,
                   T fMax = 20000.0);

    const std::vector<T>& frequencies();

    std::vector<double> interpolate(const std::map<double, double>& in, bool shiftToZero = true);

    static const std::vector<T>& octaveBands();

    static uint16_t octaveBandsIndex(T f);

private:
    uint16_t octaveBand(T f);
    T octaveBandFrequency(T f);
    T lowerFrequency(uint16_t band);
    T upperFrequency(uint16_t band);

	const uint8_t m_octaveFraction = 24;
	const uint8_t m_stride = 1;
	const uint16_t m_fMin = 8;
	const uint16_t m_fMax = 240;
    std::vector<T> m_frequencies;

    static const std::vector<T> frequencyTable;
};

#endif // FREQUENCYTABLE_H
