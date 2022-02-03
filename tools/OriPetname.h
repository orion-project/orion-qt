#ifndef ORI_PETNAME_H
#define ORI_PETNAME_H

#include <QString>

// https://github.com/lcherone/node-petname

namespace OriPetname {

QString make(int count = 2, QChar separator = ' ');

}

#endif // ORI_PETNAME_H
