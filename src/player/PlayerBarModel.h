#ifndef PLAYBARMODEL_H
#define PLAYBARMODEL_H

#include <QObject>

class PlayerModel;
class MeasureModel;

class PlayerBarModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY stateChanged)
    Q_PROPERTY(bool isMeasuring READ isMeasuring NOTIFY stateChanged)
    Q_PROPERTY(QString leftReadout READ leftReadout NOTIFY stateChanged)
    Q_PROPERTY(QString rightReadout READ rightReadout NOTIFY stateChanged)
    Q_PROPERTY(QString title READ title NOTIFY stateChanged)

public:
    explicit PlayerBarModel(MeasureModel& measurementModel,
                          PlayerModel& playerModel,
                          QObject *parent = nullptr);

    bool isPlaying() const;
    bool isMeasuring() const;
    QString leftReadout() const;
    QString rightReadout() const;
    QString title() const;

signals:
    void stateChanged();

public slots:
    void togglePlayPause();
    void toggleMeasure();

private:
    MeasureModel& _measurementModel;
    PlayerModel& _playerModel;
};

#endif // PLAYBARMODEL_H
