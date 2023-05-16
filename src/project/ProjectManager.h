#pragma once

#include <QObject>

#include <Types.h>

#include <rxcpp/rx-lite.hpp>

class Project;

class ProjectManager : public QObject {
    Q_OBJECT
public:
    static ProjectManager& instance();
    ~ProjectManager();

    void setFrCalibration(Calibration calibration);
    rxcpp::observable<std::vector<double>> calibratedFr() const;

    void add(Project* project);
    void remove(int index);

signals:
    void currentProjectChanged(Project* project);

private:
    explicit ProjectManager(QObject *parent = nullptr);

    std::vector<Project*> _projects;
    Project* _currentProject = nullptr;

    rxcpp::subjects::subject<std::vector<double>> _calibratedFr;
};
