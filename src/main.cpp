#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QThread>

#include "charts/ChartModel.h"
#include "charts/FrChartModel.h"
#include "charts/IrChartModel.h"
#include "crossover/CrossoverModel.h"
#include "equalizer/EqualizerModel.h"
#include "equalizer/FilterModel.h"
#include "measure/Measurement.h"
#include "measure/MeasureModel.h"
#include "measure/MeasurementService.h"
#include "measure/SignalGenerator.h"
#include "player/PlayerBarModel.h"
#include "player/PlayerModel.h"
#include "project/ProjectModel.h"
#include "status/StatusModel.h"
#include "target/TargetModel.h"

class ModelProviderFactory {
public:
    static void init() {
        _crossoverModel = new CrossoverModel();
        _measureModel = new MeasureModel(MeasurementService::instance());
        _projectModel = new ProjectModel(MeasurementService::instance());
        _targetModel = new TargetModel();
        _equalizerModel = new EqualizerModel(*_targetModel);
        _playerModel = new PlayerModel(*_equalizerModel);
        _playerBarModel = new PlayerBarModel(*_measureModel, *_playerModel);
        _statusModel = StatusModel::init(*_crossoverModel, *_equalizerModel);
    }

    static QObject* crossoverModel(QQmlEngine*, QJSEngine*) {
        return _crossoverModel;
    }
    static QObject* measureModel(QQmlEngine*, QJSEngine*) {
        return _measureModel;
    }
    static QObject* projectModel(QQmlEngine*, QJSEngine*) {
        return _projectModel;
    }
    static QObject* statusModel(QQmlEngine*, QJSEngine*) {
        return _statusModel;
    }
    static QObject* targetModel(QQmlEngine*, QJSEngine*) {
        return _targetModel;
    }
    static QObject* equalizerModel(QQmlEngine*, QJSEngine*) {
        return _equalizerModel;
    }
    static QObject* playerModel(QQmlEngine*, QJSEngine*) {
        return _playerModel;
    }
    static QObject* playerBarModel(QQmlEngine*, QJSEngine*) {
        return _playerBarModel;
    }

private:
    ModelProviderFactory() {
    }

    static CrossoverModel* _crossoverModel;
    static MeasureModel* _measureModel;
    static ProjectModel* _projectModel;
    static StatusModel* _statusModel;
    static TargetModel* _targetModel;
    static EqualizerModel* _equalizerModel;
    static PlayerModel* _playerModel;
    static PlayerBarModel* _playerBarModel;
};

CrossoverModel* ModelProviderFactory::_crossoverModel;
MeasureModel* ModelProviderFactory::_measureModel;
ProjectModel* ModelProviderFactory::_projectModel;
StatusModel* ModelProviderFactory::_statusModel;
TargetModel* ModelProviderFactory::_targetModel;
EqualizerModel* ModelProviderFactory::_equalizerModel;
PlayerModel* ModelProviderFactory::_playerModel;
PlayerBarModel* ModelProviderFactory::_playerBarModel;

int main(int argc, char *argv[]) {
    //fftw_init_threads();
    //fftw_plan_with_nthreads(QThread::idealThreadCount());

#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
    QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
    QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("mincequi");
    QCoreApplication::setApplicationName("qLouder");

    // Set material design
    QQuickStyle::setStyle("Material");

    // Instantiate and register models
    ModelProviderFactory::init();
    qmlRegisterSingletonType<CrossoverModel>("CrossoverModel", 1, 0, "CrossoverModel", ModelProviderFactory::crossoverModel);
    qmlRegisterSingletonType<ProjectModel>("MeasureModel", 1, 0, "MeasureModel", ModelProviderFactory::measureModel);
    qmlRegisterSingletonType<ProjectModel>("ProjectModel", 1, 0, "ProjectModel", ModelProviderFactory::projectModel);
    qmlRegisterSingletonType<StatusModel>("StatusModel", 1, 0, "StatusModel", ModelProviderFactory::statusModel);
    qmlRegisterSingletonType<TargetModel>("TargetModel", 1, 0, "TargetModel", ModelProviderFactory::targetModel);
    qmlRegisterSingletonType<EqualizerModel>("EqualizerModel", 1, 0, "EqualizerModel", ModelProviderFactory::equalizerModel);
    qmlRegisterSingletonType<PlayerModel>("PlayerModel", 1, 0, "PlayerModel", ModelProviderFactory::playerModel);
    qmlRegisterSingletonType<PlayerModel>("PlayerBarModel", 1, 0, "PlayerBarModel", ModelProviderFactory::playerBarModel);

    qmlRegisterType<ChartModel>("ChartModel", 1, 0, "ChartModel");
    qmlRegisterUncreatableType<FilterModel>("FilterModel", 1, 0, "FilterModel", "Type cannot be created in QML");
    qmlRegisterType<FrChartModel>("FrChartModel", 1, 0, "FrChartModel");
    qmlRegisterType<IrChartModel>("IrChartModel", 1, 0, "IrChartModel");

    QQmlApplicationEngine engine;
    const QUrl url(QStringLiteral("qrc:/ui/main.qml"));
    QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
                     &app, [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl)
            QCoreApplication::exit(-1);
    }, Qt::QueuedConnection);
    engine.load(url);

    return app.exec();
}
