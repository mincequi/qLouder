#include "MeasurementError.h"

MeasurementError::MeasurementError() {
}

void MeasurementError::errorToString(QAudio::Error error, QString& errorString, QString& errorDescription) {
    switch (error) {
    case QAudio::UnderrunError:
        errorString = "Buffer underrun detected";
        errorDescription = "The audio output ran out of signal buffers while measuring. This can "
                           "occur in heavy load situations and/or when the same device is used for "
                           "audio input and output. Most audio applications can recover from this. "
                           "However, this is clearly not desired when doing acoustic measurements.";
        break;
    default:
        errorString.clear();
        errorDescription.clear();
        break;
    }
}
