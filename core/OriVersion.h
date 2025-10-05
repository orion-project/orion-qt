#ifndef ORI_VERSION_H
#define ORI_VERSION_H

#include <QString>
#include <QDebug>

namespace Ori {

class Version
{
public:
    Version(): _major(0), _minor(0), _patch(0)
    {}

    Version(int major, int minor = 0, int patch = 0): _major(major), _minor(minor), _patch(patch)
    {}

    Version(const QString& ver)
    {
        bool majorOk = true, minorOk = true, patchOk = true;
        auto parts = ver.split('.');
        if (parts.size() >= 3)
        {
            _major = parts.at(0).toInt(&majorOk);
            _minor = parts.at(1).toInt(&minorOk);
            _patch = parts.at(2).toInt(&patchOk);
            // Handle codename
            // 2.0.1-beta2 is a valid version
            if (!patchOk)
            {
                parts = parts.at(2).split('-');
                if (parts.size() >= 2)
                    _patch = parts.at(0).toInt(&patchOk);
            }
        }
        else if (parts.size() == 2)
        {
            _major = parts.at(0).toInt(&majorOk);
            _minor = parts.at(1).toInt(&minorOk);
            _patch = 0;
        }
        else
        {
            _major = ver.toInt(&majorOk);
            _minor = 0;
            _patch = 0;
        }
        if (!majorOk || !minorOk || !patchOk)
        {
            _major = 0;
            _minor = 0;
            _patch = 0;
        }
    }

    #undef major // disable GNU C Lib macro
    #undef minor // disable GNU C Lib macro
    int major() const { return _major; }
    int minor() const { return _minor; }
    int patch() const { return _patch; }

    bool isValid() const
    {
        return _major > 0 || _minor > 0 || _patch > 0;
    }

    bool match(int major, int minor = 0, int patch = 0) const
    {
        return _major == major && _minor == minor && _patch == patch;
    }

    bool less(int major, int minor = 0, int patch = 0) const
    {
        return (_major < major) ||
               (_major == major && _minor < minor) ||
               (_major == major && _minor == minor && _patch < patch);
    }

    bool more(int major, int minor = 0, int patch = 0) const
    {
        return (_major > major) ||
               (_major == major && _minor > minor) ||
               (_major == major && _minor == minor && _patch > patch);
    }

    QString str(int parts = 2) const
    {
        if (parts <= 1)
            return QString::number(_major);

        if (parts == 2)
            return QString("%1.%2").arg(_major).arg(_minor);

        return QString("%1.%2.%3").arg(_major).arg(_minor).arg(_patch);
    }

    bool operator == (const Version& other) const
    {
        return match(other.major(), other.minor(), other.patch());
    }

    bool operator != (const Version& other) const
    {
        return !match(other.major(), other.minor(), other.patch());
    }

    bool operator < (const Version& other) const
    {
        return less(other.major(), other.minor(), other.patch());
    }

    bool operator > (const Version& other) const
    {
        return more(other.major(), other.minor(), other.patch());
    }

    bool operator <= (const Version& other) const
    {
        return less(other.major(), other.minor(), other.patch()) ||
               match(other.major(), other.minor(), other.patch());
    }

    bool operator >= (const Version& other) const
    {
        return more(other.major(), other.minor(), other.patch()) ||
               match(other.major(), other.minor(), other.patch());
    }

private:
    int _major;
    int _minor;
    int _patch;
};

}

#endif // ORI_VERSION_H
