#ifndef STATUSMODEL_H
#define STATUSMODEL_H

#include <QObject>
#include <QTimer>

class CrossoverModel;
class EqualizerModel;

#define qlStatus StatusModel::instance()

class StatusModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER _text NOTIFY statusChanged)

    Q_PROPERTY(int tabIndex MEMBER _tabIndex NOTIFY rangeChanged)
    Q_PROPERTY(QString propertyTitle MEMBER _propertyTitle NOTIFY rangeChanged)
    Q_PROPERTY(double sumMin READ sumMin NOTIFY rangeChanged)
    Q_PROPERTY(double sumMax READ sumMax NOTIFY rangeChanged)
    Q_PROPERTY(QString ripple READ ripple NOTIFY rangeChanged)

public:
    static StatusModel* instance();
    static StatusModel* init(const CrossoverModel& crossoverModel, const EqualizerModel& equalizerModel);

    void clearMessage();
    void showMessage(const QString& message, int timeout = 0);

    Q_INVOKABLE void setTabIndex(int index);

    double sumMin() const;
    double sumMax() const;
    QString ripple() const;

signals:
    void statusChanged();
    void rangeChanged();

private:
    explicit StatusModel(const CrossoverModel& crossoverModel,
                         const EqualizerModel& equalizerModel,
                         QObject *parent = nullptr);

    const CrossoverModel& _crossoverModel;
    const EqualizerModel& _equalizerModel;

    int _tabIndex = 0;
    QString _propertyTitle;
    QString _text;
    QTimer _timer;
};

#endif // STATUSMODEL_H
