#pragma once

#include <QString>

class QWidget;

namespace dialogs {
namespace ask {
QString open_file(QWidget *parent) noexcept;
}

namespace critical {
void file_not_found(QWidget *parent, const QString &path) noexcept;
void file_cant_read(QWidget *parent, const QString &path) noexcept;
void file_empty(QWidget *parent, const QString &path) noexcept;
void transfer_failed(QWidget *parent, const QString &reason) noexcept;
} // namespace critical

namespace information {
void data_written_successful(QWidget *parent) noexcept;
}
} // namespace dialogs
