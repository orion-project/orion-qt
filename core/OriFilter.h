#ifndef ORI_FILTER_H
#define ORI_FILTER_H

#include <vector>

namespace Ori {

template <typename TTarget, typename TCondition> class Filter
{
public:
    Filter() {}

    Filter(std::initializer_list<TCondition*> conditions): _conditions(conditions) {}

    virtual ~Filter()
    {
        for (TCondition* condition : _conditions)
            delete condition;
    }

    /// The functions checks if a target satisfies to all conditions of this filter.
    bool check(TTarget target) const
    {
        for (TCondition* condition : _conditions)
            if (!condition->check(target))
                    return false;
        return true;
    }

    /// The function appends a condition into conditions list of this filter.
    /// The filter takes ownership of the condition.
    void append(TCondition* condition)
    {
        _conditions.push_back(condition);
    }

    /// The function returns the number of object in a list satisfying to all conditions of this filter.
    template <typename TContainer>
    int count(const TContainer& container) const
    {
        int count = 0;
        typename TContainer::const_iterator it;
        for (it = container.begin(); it != container.end(); it++)
            if (check(*it))
                count++;
        return count;
    }

    /// The function returns a container of the same type as the input container,
    /// but containing only objects satisfying to all conditions of this filter.
    template <typename TContainer>
    TContainer filter(const TContainer& container) const
    {
        TContainer result;
        typename TContainer::const_iterator it;
        for (it = container.begin(); it != container.end(); it++)
            if (check(*it))
                result.insert(result.end(), *it);
        return result;
    }

protected:
    std::vector<TCondition*> _conditions;
};

} // namespace Ori

#endif // ORI_FILTER_H
