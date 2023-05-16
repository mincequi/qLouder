#pragma once

#include <QObject>
#include <QQmlListProperty>

class ProjectModel;

class ProjectsModel : public QObject {
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<ProjectModel> projects READ projects NOTIFY projectsChanged)
    Q_PROPERTY(int index MEMBER _index NOTIFY indexChanged)

public:
    explicit ProjectsModel(QObject *parent = nullptr);

    QQmlListProperty<ProjectModel> projects();
    Q_INVOKABLE void loadProject(const QUrl& fileName);
    Q_INVOKABLE void setIndex(int index);

public slots:
    void removeProject(int index);

signals:
    void projectsChanged();
    void indexChanged();

private:
    QList<ProjectModel*> _projects;
    QQmlListProperty<ProjectModel> _projectObjects;
    int _index = 0;
};
