#ifndef STATUSMODEL_H
#define STATUSMODEL_H

#include <QObject>
#include <QTimer>

#define qlStatus StatusModel::instance()

class StatusModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER _text NOTIFY statusChanged)

public:
    static StatusModel* instance();

    void clearMessage();
    void showMessage(const QString& message, int timeout = 0);

signals:
    void statusChanged();

private:
    explicit StatusModel(QObject *parent = nullptr);

    QString _text = "";
    QTimer _timer;
};

#endif // STATUSMODEL_H
