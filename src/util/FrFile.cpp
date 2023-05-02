#include "FrFile.h"

#include <QFile>
#include <QIODevice>
#include <QRegExp>

template <class T>
FrFile<T>::FrFile() {
}

template <class T>
std::map<T,T> FrFile<T>::read(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    std::map<T,T> out;
    QRegExp rx("(([0-9]*[.])?[0-9]+)\\t([+-]?([0-9]*[.])?[0-9]+)");
    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        if (line.startsWith('#')) continue;

        if (rx.indexIn(line) != -1) {
            out[rx.cap(1).toDouble()] = -rx.cap(3).toDouble();
        }
    }

    for (auto it = out.begin(); it != out.end(); ) {
         if (it->first < 19.5 || it->first > 20600.0) {
              it = out.erase(it);
         } else {
              ++it;
         }
    }

    return out;
}

template class FrFile<float>;
template class FrFile<double>;
