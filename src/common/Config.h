#pragma once

#include <map>

#include "Types.h"

#define cfg Config::instance()

class Config {
public:
	static Config* instance();
	virtual ~Config();

    std::map<double, double> calibration0;
    std::map<double, double> calibration90;
    Calibration calibration;

private:
	explicit Config();

	friend class ProjectModel;
};
