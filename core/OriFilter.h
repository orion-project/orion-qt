#ifndef ORI_FILTER_H
#define ORI_FILTER_H

#include <vector>

namespace Ori {

template <typename TTarget, typename TCondition> class Filter
{
public:
    Filter() {}

    Filter(std::initializer_list<TCondition*> conditions): _conditions(conditions) {}

    ~Filter()
    {
        for (TCondition* condition : _conditions)
            delete condition;
    }

    bool check(TTarget target)
    {
        for (TCondition* condition : _conditions)
            if (!condition->check(target))
                    return false;
        return true;
    }

    void append(TCondition* condition) { _conditions.push_back(condition); }

protected:
    std::vector<TCondition*> _conditions;
};

} // namespace Ori

#endif // ORI_FILTER_H
