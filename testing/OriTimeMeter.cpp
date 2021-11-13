#include "OriTimeMeter.h"

#include <QDebug>

#ifdef Q_OS_WIN
// In MinGW (7.3 at the moment) on Windows, the system timer is used
// for `std::chrono::high_resolution_clock` which has a bad resolution.
// This implementation of the true high resolution clock for Windows is from here:
// https://stackoverflow.com/questions/16299029/resolution-of-stdchronohigh-resolution-clock-doesnt-correspond-to-measureme
#include <windows.h>
#endif

namespace Ori {
namespace Testing {

QString formatDuration(int64_t duration_ns)
{
    QString time, unit;
    double d = duration_ns;
    if (d/1e3 < 1) {
        time = QString::number(duration_ns);
        unit = QStringLiteral(" ns");
    } else if (d/1e6 < 1) {
        time = QString::number(d/1e3, 'f', 3);
        unit = QStringLiteral(" µs");
    } else if (d/1e9 < 1) {
        time = QString::number(d/1e6, 'f', 3);
        unit = QStringLiteral(" ms");
    } else {
        time = QString::number(d/1e9, 'f', 3);
        unit = QStringLiteral(" s");
    }
    if (time.contains('.'))
        while (time.endsWith('0'))
            time.chop(1);
    return time + unit;
}

//------------------------------------------------------------------------------
//                             WindowsHighResClock
//------------------------------------------------------------------------------

#ifdef Q_OS_WIN
static const int64_t __frequency = []() -> int64_t
{
    LARGE_INTEGER frequency;
    QueryPerformanceFrequency(&frequency);
    return frequency.QuadPart;
}();

WindowsHighResClock::time_point WindowsHighResClock::now()
{
    LARGE_INTEGER count;
    QueryPerformanceCounter(&count);
    return time_point(duration(count.QuadPart * static_cast<int64_t>(std::nano::den) / __frequency));
}
#endif

//------------------------------------------------------------------------------
//                                 TimeMeter
//------------------------------------------------------------------------------

TimeMeter::TimeMeter(const QString& ident) : ident(ident)
{
    startTime = HighResClock::now();
}

TimeMeter::~TimeMeter()
{
    if (!stopped) {
        stop();
        qDebug() << ident << formatDuration(duration_ns);
    }
}

int64_t TimeMeter::stop()
{
    stopped = true;
    stopTime = HighResClock::now();
    duration_ns = std::chrono::duration_cast<std::chrono::nanoseconds>(stopTime - startTime).count();
    return duration_ns;
}

} // namespace Tests
} // namespace Ori
