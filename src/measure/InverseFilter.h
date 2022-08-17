#ifndef INVERSEFILTER_H
#define INVERSEFILTER_H

#include <vector>

class ExcitationSignal;

class InverseFilter {
public:
    static InverseFilter from(const ExcitationSignal& excitationSignal);

    InverseFilter();

    const std::vector<float>& data() const;

private:
    void amplitudeEnvelope(int offsetFront,
                           int offsetBack,
                           double minF,
                           double maxF);

    std::vector<float> _data;
};

#endif // INVERSEFILTER_H
