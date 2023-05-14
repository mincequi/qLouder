#pragma once

#include <vector>

class ExcitationSignal;

class InverseSignal {
public:
    static InverseSignal from(const ExcitationSignal& excitationSignal);

    InverseSignal();

    const std::vector<float>& data() const;

private:
    void amplitudeEnvelope(int offsetFront,
                           int offsetBack,
                           double minF,
                           double maxF);

    std::vector<float> _data;
};
