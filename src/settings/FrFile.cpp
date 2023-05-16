#include "FrFile.h"

#include <QFile>
#include <QIODevice>
#include <QRegExp>

FrFile::FrFile() {
}

std::map<double,double> FrFile::read(const QString& filename) {
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
        return {};

    std::map<double,double> out;
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
