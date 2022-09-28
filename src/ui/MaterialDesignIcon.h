#pragma once

#include <QPixmap>

namespace material {
    QPixmap pixmap(const QString& name, const QSize& size = QSize(18, 18), const QColor& color = Qt::white);
}
