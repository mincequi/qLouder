#pragma once

#include <miniaudio.h>

namespace ma {

class DataSource : public ma_data_source_base {
protected:
    DataSource();
    ~DataSource();
};

} // namespace ma
