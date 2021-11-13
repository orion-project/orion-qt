#ifndef ORI_TIME_METER_H
#define ORI_TIME_METER_H

#include <QString>

#include <chrono>

namespace Ori {
namespace Testing {

QString formatDuration(int64_t duration_ns);

#ifdef Q_OS_WIN
struct WindowsHighResClock
{
    typedef std::chrono::duration<int64_t, std::nano> duration;
    typedef std::chrono::time_point<WindowsHighResClock> time_point;
    static time_point now();
};

using HighResClock = WindowsHighResClock;
using TimePoint = WindowsHighResClock::time_point;
#else
using HighResClock = std::chrono::high_resolution_clock;
using TimePoint = std::chrono::time_point<std::chrono::high_resolution_clock>;
#endif

struct TimeMeter
{
    TimeMeter(const QString& ident = QString());
    ~TimeMeter();
    int64_t stop();

    QString ident;
    bool stopped = false;
    TimePoint startTime;
    TimePoint stopTime;
    int64_t duration_ns;
};

} // namespace Tests
} // namespace Ori

#endif // ORI_TIME_METER_H
