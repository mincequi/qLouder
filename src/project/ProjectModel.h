#pragma once

#include <QObject>

class ProjectModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY valuesChanged)

public:
    explicit ProjectModel(QObject *parent = nullptr);

    QString name() const;

signals:
    void valuesChanged();

private:
    QString _name;
};
