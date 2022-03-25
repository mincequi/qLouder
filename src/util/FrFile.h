#ifndef FRFILE_H
#define FRFILE_H

#include <map>

#include <QString>

template<class T>
class FrFile {
public:
    static std::map<T,T> read(const QString& file);

private:
    FrFile();
};

#endif // FRFILE_H
