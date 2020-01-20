#ifndef ORI_RESULT_H
#define ORI_RESULT_H

#include <QString>

namespace Ori {

template <typename TResult> class Result
{
public:
    TResult result() const { return _result; }
    const QString& error() const { return _error; }
    bool ok() const { return _error.isEmpty(); }

    static Result fail(const QString& error) { return Result(error); }
    static Result ok(TResult result) { return Result(QString(), result); }

private:
    Result(const QString& error): _error(error) {}
    Result(const QString& error, TResult result): _error(error), _result(result) {}

    QString _error;
    TResult _result;
};

}

#endif // ORI_RESULT_H
