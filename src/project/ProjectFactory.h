#pragma once

#include <string>

class Project;

class ProjectFactory {
public:
    static Project* fromDisk(const std::string& fileName);
    static void toDisk(Project& measurement, const std::string& fileName);
};
