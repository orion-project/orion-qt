#ifndef ORI_LOREM_IPSUM_H
#define ORI_LOREM_IPSUM_H

#include <QString>

namespace LoremIpsum
{

enum WordOption { None, FirstCapital, CamelCase };

QString makeText(int numWords);
QString makeWords(int count, WordOption option = None);
QString makeEmail();

} // namespace LoremIpsum

#endif // ORI_LOREM_IPSUM_H
