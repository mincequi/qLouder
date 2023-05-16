#include "Config.h"

Config* Config::instance() {
	static Config instance;
	return &instance;
}

Config::Config() {
}

Config::~Config() {
}
