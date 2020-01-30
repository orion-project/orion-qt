#ifndef ORI_ARG_H
#define ORI_ARG_H

namespace Ori {

template <typename TArg, typename TArgTag>
struct Argument
{
    explicit Argument(const TArg v): value(v) {}

    const TArg value;
};

} // namespace Ori

#endif // ORI_ARG_H
