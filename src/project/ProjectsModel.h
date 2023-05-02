#pragma once

#include <QObject>
#include <QQmlListProperty>

class ProjectModel;

class ProjectsModel : public QObject {
    Q_OBJECT

     Q_PROPERTY(QQmlListProperty<ProjectModel> projects READ projects NOTIFY projectsChanged)
public:
    explicit ProjectsModel(QObject *parent = nullptr);

    QQmlListProperty<ProjectModel> projects();

public slots:
    void addProject();
    void removeProject(int index);

signals:
    void projectsChanged();

private:
    QList<ProjectModel*> _projects;
};
