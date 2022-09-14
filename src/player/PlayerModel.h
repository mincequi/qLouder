#ifndef PLAYERMODEL_H
#define PLAYERMODEL_H

#include <QObject>
#include <QTimer>

class EqualizerModel;

class PlayerModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QStringList supportedFileExtensions READ supportedFileExtensions CONSTANT)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY isPlayingChanged)
    Q_PROPERTY(double begin READ begin NOTIFY loopChanged)
    Q_PROPERTY(double end READ end NOTIFY loopChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)

public:
    explicit PlayerModel(const EqualizerModel& equalizerModel, QObject *parent = nullptr);
    virtual ~PlayerModel();

    QStringList supportedFileExtensions() const;
    bool isPlaying() const;
    double begin() const;
    double end() const;
    double progress() const;

public slots:
    void setFile(const QString& file);
    void setDevice(const QString& output);
    void togglePlayPause();
    void setBegin(double value);
    void setEnd(double value);

signals:
    void isPlayingChanged();
    void loopChanged();
    void progressChanged();

private slots:
    void onFiltersChanged();

private:
    // Due to conflicting declarations of some types between cinder and qt, we
    // have different compile units for each.
    class PlayerModelPrivate* _d;

    const EqualizerModel& _equalizerModel;

    QTimer _timer;
};

#endif // PLAYERMODEL_H
