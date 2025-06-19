#ifndef ORI_TEMPLATES_H
#define ORI_TEMPLATES_H

#include <QString>
#include <QStringList>
#include <QVector>

namespace Ori {

//------------------------------------------------------------------------------
//                               Singleton
//------------------------------------------------------------------------------

template <typename T> class Singleton
{
public:
    static T& instance()
    {
        return *instancePtr();
    }

    static T* instancePtr()
    {
        static T* object = new T();
        return object;
    }

protected:
    Singleton() {}
};

//------------------------------------------------------------------------------
//                               Notifier
//------------------------------------------------------------------------------

template <typename T> class Notifier
{
public:
    void registerListener(T *listener)
    {
        if (!_listeners.contains(listener))
            _listeners.push_back(listener);
    }

    void unregisterListener(T *listener)
    {
        _listeners.removeOne(listener);
    }

    const QList<T*>& listeners() const { return _listeners; }

    template <typename TMethod, typename ...Args>
    void notify(TMethod method, Args ...args)
    {
        for (auto listener : _listeners)
            (listener->*method)(args...);
    }

protected:
    QList<T*> _listeners;
};

#define NOTIFY_LISTENERS(method)                      \
    for (int _i = 0; _i < _listeners.size(); _i++)    \
        _listeners.at(_i)->method()

#define NOTIFY_LISTENERS_1(method, arg1)              \
    for (int _i = 0; _i < _listeners.size(); _i++)    \
        _listeners.at(_i)->method(arg1)

#define NOTIFY_LISTENERS_2(method, arg1, arg2)        \
    for (int _i = 0; _i < _listeners.size(); _i++)    \
        _listeners.at(_i)->method(arg1, arg2)

#define NOTIFY_LISTENERS_3(method, arg1, arg2, arg3)  \
    for (int _i = 0; _i < _listeners.size(); _i++)    \
        _listeners.at(_i)->method(arg1, arg2, arg3)

//------------------------------------------------------------------------------

// This macro counts number of arguments in variadic macro, up to 15 items.
// http://efesx.com/2010/07/17/variadic-macro-to-count-number-of-arguments
// VA_ARGS_COUNT(x,y,z)
// VA_ARGS_COUNT_IMPL(x y z 5 4 3 2 1)
//                    1 2 3 4 5 C x y z
//                              ^
#ifdef Q_CC_MSVC
#define VA_ARGS_COUNT(...) std::tuple_size<decltype(std::make_tuple(__VA_ARGS__))>::value
#else
#define VA_ARGS_COUNT(...) VA_ARGS_COUNT_IMPL(__VA_ARGS__,15,14,13,12,11,10,9,8,7,6,5,4,3,2,1)
#define VA_ARGS_COUNT_IMPL(_1,_2,_3,_4,_5,_6,_7,_8,_9,_10,_11,_12,_13,_14,_15,count,...) count
#endif

#define DECLARE_ENUM(enum_type, start_value, first, ...)                         \
    enum enum_type {first = start_value, __VA_ARGS__};                           \
    static const int enum_type##_Count = VA_ARGS_COUNT(__VA_ARGS__) + 1;         \
    static const enum_type enum_type##_Min = enum_type(start_value);             \
    static const enum_type enum_type##_Max = enum_type(start_value + enum_type##_Count - 1);\
    static const QVector<enum_type> enum_type##_Values({first, __VA_ARGS__});    \
    inline const QVector<QString>& enum_type##_Names()                           \
    {                                                                            \
        static QVector<QString> names;                                           \
        if (names.empty())                                                       \
        {                                                                        \
            names.append(QString(#first));                                       \
            foreach (auto s, QString(#__VA_ARGS__).split(','))                   \
                names.append(s.trimmed());                                       \
        }                                                                        \
        return names;                                                            \
    }                                                                            \
    inline const QString& enum_type##_ItemName(enum_type value)                  \
    {                                                                            \
        return enum_type##_Names().at(value - start_value);                      \
    }                                                                            \
    inline enum_type enum_type##_GetItemByName(const QString& item_name, bool* ok)\
    {                                                                            \
        const QVector<QString>& names = enum_type##_Names();                     \
        for (int i = 0; i < names.size(); i++)                                   \
            if (names.at(i) == item_name)                                        \
            {                                                                    \
                *ok = true;                                                      \
                return enum_type##_Values.at(i);                                 \
            }                                                                    \
        *ok = false;                                                             \
        return enum_type##_Min;                                                  \
    }

#define ENUM_COUNT(enum_type) enum_type##_Count
#define ENUM_MIN(enum_type) enum_type##_Min
#define ENUM_MAX(enum_type) enum_type##_Max
#define ENUM_VALUES(enum_type) enum_type##_Values
#define ENUM_NAMES(enum_type) enum_type##_Names()
#define ENUM_ITEM_NAME(enum_type, item) enum_type##_ItemName(enum_type(item))
#define ENUM_ITEM_BY_NAME(enum_type, item_name, ok) enum_type##_GetItemByName(item_name, ok)

//------------------------------------------------------------------------------

#define BREAKABLE_BLOCK \
    int __breakable_block_counter = 0; \
    while (__breakable_block_counter++ < 1)

template <typename T> struct Optional {
    bool set;
    T value;
    Optional(): set(false) {}
    Optional(const T& value): set(true), value(value) {}
    static Optional null() { return Optional(); }
};

template <typename T>
inline bool operator ==(const Optional<T>& v1, const Optional<T>& v2)
{
    return v1.set && v2.set && v1.value == v2.value;
}

template <typename T> struct TmpAssign {
    TmpAssign(T* target, const T& value) {
        _target = target;
        _savedValue = *target;
        *target = value;
    }
    ~TmpAssign() {
        *_target = _savedValue;
    }
private:
    T* _target;
    T _savedValue;
};

} // namespace Ori

#define BOOL_PARAM(param_name) \
    struct param_name { \
        explicit param_name(bool v) : value(v) {} \
        operator bool() const { return value; } \
        bool value; \
    };

#define INT_PARAM(param_name) \
    struct param_name { \
        explicit param_name(int v) : value(v) {} \
        operator int() const { return value; } \
        int value; \
    };

#endif // ORI_TEMPLATES_H
