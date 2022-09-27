#ifndef PLAYERMODEL_H
#define PLAYERMODEL_H

#include <QObject>
#include <QTimer>

class EqualizerModel;

class PlayerModel : public QObject {
    Q_OBJECT

    Q_PROPERTY(QStringList supportedFileExtensions READ supportedFileExtensions CONSTANT)
    Q_PROPERTY(bool isPlaying READ isPlaying NOTIFY statusChanged)
    Q_PROPERTY(double loopBeginTime READ loopBeginTime NOTIFY statusChanged)
    Q_PROPERTY(double loopEndTime READ loopEndTime NOTIFY statusChanged)
    Q_PROPERTY(double totalTime READ totalTime NOTIFY statusChanged)
    Q_PROPERTY(double progress READ progress NOTIFY progressChanged)
    Q_PROPERTY(QString file MEMBER _file NOTIFY fileChanged)
    Q_PROPERTY(QString title READ title NOTIFY fileChanged)
    Q_PROPERTY(bool isEqualizerEnabled READ isEqualizerEnabled NOTIFY statusChanged)

public:
    explicit PlayerModel(const EqualizerModel& equalizerModel,
                         QObject *parent = nullptr);
    virtual ~PlayerModel();

    QStringList supportedFileExtensions() const;
    bool isPlaying() const;
    double loopBeginTime() const;
    double loopEndTime() const;
    double totalTime() const;
    double progress() const;
    QString title() const;
    bool isEqualizerEnabled() const;

public slots:
    void setFile(const QString& file);
    void setDevice(const QString& output);
    void togglePlayPause();
    void setBegin(double value);
    void setEnd(double value);
    void toggleEqualizer();

signals:
    void statusChanged();
    void progressChanged();
    void fileChanged();

private slots:
    void onFiltersChanged();

private:
    // Due to conflicting declarations of some types between cinder and qt, we
    // have different compile units for each.
    class PlayerModelPrivate* _d;

    const EqualizerModel& _equalizerModel;

    QTimer _timer;
    QString _file;
    QString _title;
};

#endif // PLAYERMODEL_H
