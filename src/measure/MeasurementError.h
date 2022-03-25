#ifndef MEASUREMENTERROR_H
#define MEASUREMENTERROR_H

#include <QAudio>

class MeasurementError {
public:
    static void errorToString(QAudio::Error error, QString& errorString, QString& errorDescription);

private:
    MeasurementError();
};

#endif // MEASUREMENTERROR_H
