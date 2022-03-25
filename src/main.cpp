#include <QApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>

#include "charts/ChartModel.h"
#include "charts/FrChartModel.h"
#include "charts/IrChartModel.h"
#include "measure/Measurement.h"
#include "measure/MeasureModel.h"
#include "measure/MeasurementService.h"
#include "measure/SignalGenerator.h"
#include "project/ProjectModel.h"
#include "status/StatusModel.h"

int main(int argc, char *argv[]) {
#if QT_VERSION < QT_VERSION_CHECK(6, 0, 0)
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
#endif
	QApplication app(argc, argv);

    QCoreApplication::setOrganizationName("mincequi");
    QCoreApplication::setApplicationName("qLouder");

	// Set material design
	QQuickStyle::setStyle("Material");

	// Instantiate and register models
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

    qmlRegisterType<ChartModel>("ChartModel", 1, 0, "ChartModel");
    qmlRegisterType<FrChartModel>("FrChartModel", 1, 0, "FrChartModel");
    qmlRegisterType<IrChartModel>("IrChartModel", 1, 0, "IrChartModel");

    // @TODO: for testing
    /*
    std::vector<float> signal;
    SignalGenerator::sineSweep(signal, 250, 48000, 20.0, 20000.0);
    std::vector<float> filter = signal;
    std::reverse(filter.begin(), filter.end());
    SignalGenerator::volumeEnvelope(filter.begin(), filter.end(), 20.0, 20000.0);
    Measurement<float> measurement(48000, signal, filter);
    auto chartModel = new ChartModel(measurement);
    qmlRegisterSingletonType<ChartModel>("ChartModel", 1, 0, "ChartModel", [&](QQmlEngine*, QJSEngine*) -> QObject* {
        return chartModel;
    });
    */

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
