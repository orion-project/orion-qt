#ifndef ORI_LOG_H
#define ORI_LOG_H

#include <QString>

namespace Ori {
namespace Log {

void write(const char *msg);
void write(const QString& msg);
void write(const char *fileName, const char *msg);
void write(const char *fileName, const QString& msg);

} // namespace Log
} // namespace Ori

#endif // ORI_LOG_H
