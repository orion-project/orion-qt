#ifndef ORI_VERSION_H
#define ORI_VERSION_H

#include <QString>

namespace Ori {

class Version
{
public:
    Version(): _major(0), _minor(0)
    {}

    Version(int major, int minor): _major(major), _minor(minor)
    {}

    Version(const QString& ver)
    {
        _major = ver.section('.', 0, 0).toInt();
        _minor = ver.section('.', 1, 1).toInt();
    }

    int major() const { return _major; }
    int minor() const { return _minor; }

    bool match(int major, int minor) const
    {
        return _major == major && _minor == minor;
    }

    bool less(int major, int minor) const
    {
        return (_major < major) || (_major == major && _minor < minor);
    }

    bool more(int major, int minor) const
    {
        return (_major > major) || (_major == major && _minor > minor);
    }

    QString str() const
    {
        return QString("%1.%2").arg(_major).arg(_minor);
    }

    bool operator == (const Version& other) const
    {
        return match(other.major(), other.minor());
    }

    bool operator != (const Version& other) const
    {
        return !match(other.major(), other.minor());
    }

    bool operator < (const Version& other) const
    {
        return less(other.major(), other.minor());
    }

    bool operator > (const Version& other) const
    {
        return more(other.major(), other.minor());
    }

    bool operator <= (const Version& other) const
    {
        return less(other.major(), other.minor()) || match(other.major(), other.minor());
    }

    bool operator >= (const Version& other) const
    {
        return more(other.major(), other.minor()) || match(other.major(), other.minor());
    }

private:
    int _major;
    int _minor;
};

}

#endif // ORI_VERSION_H
