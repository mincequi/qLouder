#ifndef DIALOGMODEL_H
#define DIALOGMODEL_H

#include <QObject>

class DialogModel : public QObject {
    Q_OBJECT
public:
    explicit DialogModel(QObject *parent = nullptr);

signals:

};

#endif // DIALOGMODEL_H
