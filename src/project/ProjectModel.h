#pragma once

#include <QObject>

class Project;

class ProjectModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name READ name NOTIFY valuesChanged)

public:
    explicit ProjectModel(const QString& fileName, QObject *parent = nullptr);

    QString name() const;

signals:
    void valuesChanged();

private:
    QString _name;
};
