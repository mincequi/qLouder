#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QThread>

#include <fftw3.h>

#include "charts/ChartModel.h"
#include "charts/FrChartModel.h"
#include "charts/IrChartModel.h"
#include "crossover/CrossoverModel.h"
#include "equalizer/EqualizerLogic.h"
#include "equalizer/EqualizerModel.h"
#include "measure/Measurement.h"
#include "measure/MeasureModel.h"
#include "measure/MeasurementService.h"
#include "measure/SignalGenerator.h"
#include "player/PlayerBarModel.h"
#include "player/PlayerModel.h"
#include "project/ProjectModel.h"
#include "status/StatusModel.h"
#include "target/TargetModel.h"

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
    auto crossoverModel = new CrossoverModel();
    qmlRegisterSingletonType<CrossoverModel>("CrossoverModel", 1, 0, "CrossoverModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
        return crossoverModel;
    });
    auto measureModel = new MeasureModel(MeasurementService::instance());
	qmlRegisterSingletonType<ProjectModel>("MeasureModel", 1, 0, "MeasureModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
		return measureModel;
	});
    auto projectModel = new ProjectModel(MeasurementService::instance());
	qmlRegisterSingletonType<ProjectModel>("ProjectModel", 1, 0, "ProjectModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
		return projectModel;
	});
    qmlRegisterSingletonType<StatusModel>("StatusModel", 1, 0, "StatusModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
        return StatusModel::instance();
    });
    auto targetModel = new TargetModel();
    qmlRegisterSingletonType<TargetModel>("TargetModel", 1, 0, "TargetModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
        return targetModel;
    });
    auto equalizerLogic = new EqualizerLogic();
    qmlRegisterSingletonType<EqualizerLogic>("EqualizerLogic", 1, 0, "EqualizerLogic", [&](QQmlEngine*, QJSEngine*) -> QObject* {
        return equalizerLogic;
    });
    auto equalizerModel = new EqualizerModel(*equalizerLogic, *targetModel);
    qmlRegisterSingletonType<EqualizerModel>("EqualizerModel", 1, 0, "EqualizerModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
        return equalizerModel;
    });
    auto playerModel = new PlayerModel(*equalizerModel);
    qmlRegisterSingletonType<PlayerModel>("PlayerModel", 1, 0, "PlayerModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
        return playerModel;
    });
    auto playerBarModel = new PlayerBarModel(*measureModel, *playerModel);
    qmlRegisterSingletonType<PlayerModel>("PlayerBarModel", 1, 0, "PlayerBarModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
        return playerBarModel;
    });

    qmlRegisterType<ChartModel>("ChartModel", 1, 0, "ChartModel");
    qmlRegisterType<FrChartModel>("FrChartModel", 1, 0, "FrChartModel");
    qmlRegisterType<IrChartModel>("IrChartModel", 1, 0, "IrChartModel");

	QQmlApplicationEngine engine;
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	engine.load(url);

	return app.exec();
}
