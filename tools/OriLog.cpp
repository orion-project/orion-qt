#include "OriLog.h"

#include <stdio.h>

#ifndef ORI_LOG_FILE
#define ORI_LOG_FILE "orion.log"
#endif

namespace Ori {
namespace Log {

void write(const char* fileName, const char* msg)
{
    FILE *log = fopen(fileName, "a");
    fprintf(log, "%s\n", msg);
    fclose(log);
}

void write(const char *fileName, const QString& msg)
{
    write(fileName, msg.toStdString().c_str());
}

void write(const char *msg)
{
    write(ORI_LOG_FILE, msg);
}

void write(const QString& msg)
{
    write(ORI_LOG_FILE, msg);
}

} // namespace Log
} // namespace Ori
