#include "OriLoremIpsum.h"

#include <QStringList>

namespace LoremIpsum {

const QList<QString>& words()
{
    static QList<QString> items({
        "consetetur", "sadipscing" , "elitr" , "sed" , "diam" , "nonumy" , "eirmod",
        "tempor", "invidunt" , "ut" , "labore" , "et" , "dolore" , "magna" , "aliquyam", "erat" , "sed" , "diam" , "voluptua" ,
        "at", "vero" , "eos" , "et" , "accusam" , "et" , "justo" , "duo" , "dolores", "et" , "ea" , "rebum" , "stet" , "clita" ,
        "kasd", "gubergren" , "no" , "sea" , "takimata" , "sanctus" , "est" , "lorem", "ipsum" , "dolor" , "sit" , "amet" ,
        "lorem", "ipsum" , "dolor" , "sit" , "amet" , "consetetur" , "sadipscing", "elitr" , "sed" , "diam" , "nonumy" , "eirmod" ,
        "tempor", "invidunt" , "ut" , "labore" , "et" , "dolore" , "magna" , "aliquyam", "erat" , "sed" , "diam" , "voluptua" ,
        "at", "vero" , "eos" , "et" , "accusam" , "et" , "justo" , "duo" , "dolores", "et" , "ea" , "rebum" , "stet" , "clita" ,
        "kasd", "gubergren" , "no" , "sea" , "takimata" , "sanctus" , "est" , "lorem", "ipsum" , "dolor" , "sit" , "amet" ,
        "lorem", "ipsum" , "dolor" , "sit" , "amet" , "consetetur" , "sadipscing", "elitr" , "sed" , "diam" , "nonumy" , "eirmod" ,
        "tempor", "invidunt" , "ut" , "labore" , "et" , "dolore" , "magna" , "aliquyam", "erat" , "sed" , "diam" , "voluptua" ,
        "at", "vero" , "eos" , "et" , "accusam" , "et" , "justo" , "duo" , "dolores", "et" , "ea" , "rebum" , "stet" , "clita" ,
        "kasd", "gubergren" , "no" , "sea" , "takimata" , "sanctus" , "est" , "lorem", "ipsum" , "dolor" , "sit" , "amet" , "duis" ,
        "autem", "vel" , "eum" , "iriure" , "dolor" , "in" , "hendrerit" , "in", "vulputate" , "velit" , "esse" , "molestie" ,
        "consequat", "vel" , "illum" , "dolore" , "eu" , "feugiat" , "nulla", "facilisis" , "at" , "vero" , "eros" , "et" ,
        "accumsan", "et" , "iusto" , "odio" , "dignissim" , "qui" , "blandit", "praesent" , "luptatum" , "zzril" , "delenit" ,
        "augue", "duis" , "dolore" , "te" , "feugait" , "nulla" , "facilisi", "lorem" , "ipsum" , "dolor" , "sit" , "amet" ,
        "consectetuer", "adipiscing" , "elit" , "sed" , "diam" , "nonummy" , "nibh", "euismod" , "tincidunt" , "ut" , "laoreet" ,
        "dolore", "magna" , "aliquam" , "erat" , "volutpat" , "ut" , "wisi" , "enim", "ad" , "minim" , "veniam" , "quis" ,
        "nostrud", "exerci" , "tation" , "ullamcorper" , "suscipit" , "lobortis", "nisl" , "ut" , "aliquip" , "ex" , "ea" ,
        "commodo", "consequat" , "duis" , "autem" , "vel" , "eum" , "iriure", "dolor" , "in" , "hendrerit" , "in" , "vulputate" ,
        "velit", "esse" , "molestie" , "consequat" , "vel" , "illum" , "dolore", "eu" , "feugiat" , "nulla" , "facilisis" , "at" ,
        "vero", "eros" , "et" , "accumsan" , "et" , "iusto" , "odio" , "dignissim", "qui" , "blandit" , "praesent" , "luptatum" ,
        "zzril", "delenit" , "augue" , "duis" , "dolore" , "te" , "feugait" , "nulla", "facilisi" , "nam" , "liber" , "tempor" ,
        "cum", "soluta" , "nobis" , "eleifend" , "option" , "congue" , "nihil", "imperdiet" , "doming" , "id" , "quod" , "mazim" ,
        "placerat", "facer" , "possim" , "assum" , "lorem" , "ipsum" , "dolor", "sit" , "amet" , "consectetuer" , "adipiscing" ,
        "elit", "sed" , "diam" , "nonummy" , "nibh" , "euismod" , "tincidunt", "ut" , "laoreet" , "dolore" , "magna" , "aliquam" ,
        "erat", "volutpat" , "ut" , "wisi" , "enim" , "ad" , "minim" , "veniam", "quis" , "nostrud" , "exerci" , "tation" ,
        "ullamcorper", "suscipit" , "lobortis" , "nisl" , "ut" , "aliquip" , "ex", "ea" , "commodo" , "consequat" , "duis" ,
        "autem", "vel" , "eum" , "iriure" , "dolor" , "in" , "hendrerit" , "in", "vulputate" , "velit" , "esse" , "molestie" ,
        "consequat", "vel" , "illum" , "dolore" , "eu" , "feugiat" , "nulla", "facilisis" , "at" , "vero" , "eos" , "et" , "accusam" ,
        "et", "justo" , "duo" , "dolores" , "et" , "ea" , "rebum" , "stet" , "clita", "kasd" , "gubergren" , "no" , "sea" ,
        "takimata", "sanctus" , "est" , "lorem" , "ipsum" , "dolor" , "sit" , "amet", "lorem" , "ipsum" , "dolor" , "sit" ,
        "amet", "consetetur" , "sadipscing" , "elitr" , "sed" , "diam" , "nonumy", "eirmod" , "tempor" , "invidunt" , "ut" ,
        "labore", "et" , "dolore" , "magna" , "aliquyam" , "erat" , "sed" , "diam", "voluptua" , "at" , "vero" , "eos" , "et" ,
        "accusam", "et" , "justo" , "duo" , "dolores" , "et" , "ea" , "rebum", "stet" , "clita" , "kasd" , "gubergren" , "no" ,
        "sea", "takimata" , "sanctus" , "est" , "lorem" , "ipsum" , "dolor" , "sit", "amet" , "lorem" , "ipsum" , "dolor" , "sit" ,
        "amet", "consetetur" , "sadipscing" , "elitr" , "at" , "accusam" , "aliquyam", "diam" , "diam" , "dolore" , "dolores" ,
        "duo", "eirmod" , "eos" , "erat" , "et" , "nonumy" , "sed" , "tempor", "et" , "et" , "invidunt" , "justo" , "labore" ,
        "stet", "clita" , "ea" , "et" , "gubergren" , "kasd" , "magna" , "no", "rebum" , "sanctus" , "sea" , "sed" , "takimata" ,
        "ut", "vero" , "voluptua" , "est" , "lorem" , "ipsum" , "dolor" , "sit", "amet" , "lorem" , "ipsum" , "dolor" , "sit" ,
        "amet", "consetetur" , "sadipscing" , "elitr" , "sed" , "diam" , "nonumy", "eirmod" , "tempor" , "invidunt" , "ut" ,
        "labore", "et" , "dolore" , "magna" , "aliquyam" , "erat" , "consetetur", "sadipscing" , "elitr" , "sed" , "diam" ,
        "nonumy", "eirmod" , "tempor" , "invidunt" , "ut" , "labore" , "et" , "dolore", "magna" , "aliquyam" , "erat" , "sed" ,
        "diam", "voluptua" , "at" , "vero" , "eos" , "et" , "accusam" , "et" , "justo", "duo" , "dolores" , "et" , "ea" ,
        "rebum", "stet" , "clita" , "kasd" , "gubergren" , "no" , "sea" , "takimata", "sanctus" , "est" , "lorem" , "ipsum"
    });
    return items;
}

const QList<QString>& domains()
{
    static QList<QString> items({
        "ac", "ad" , "ae" , "aero" , "af" , "ag" , "ai" , "al" , "am" , "an" , "ao", "aq" , "ar" , "arpa" , "as" ,
        "asia", "at" , "au" , "aw" , "ax" , "az" , "ba" , "bb" , "bd" , "be" , "bf", "bg" , "bh" , "bi" , "biz" ,
        "bj", "bm" , "bn" , "bo" , "br" , "bs" , "bt" , "bv" , "bw" , "by" , "bz", "ca" , "cc" , "cf" , "cg" , "ch" ,
        "ci", "ck" , "cl" , "cm" , "cn" , "co" , "com" , "coop" , "cr" , "cs" , "cu", "cv" , "cx" , "cy" , "cz" ,
        "de", "dj" , "dk" , "dm" , "do" , "dz" , "ec" , "edu" , "ee" , "eg" , "eh", "er" , "es" , "et" , "eu" , "fi" ,
        "firm", "fj" , "fk" , "fm" , "fo" , "fr" , "fx" , "ga" , "gb" , "gd" , "ge", "gf" , "gh" , "gi" , "gl" , "gm" ,
        "gn", "gov" , "gp" , "gq" , "gr" , "gs" , "gt" , "gu" , "gw" , "gy" , "hk", "hm" , "hn" , "hr" , "ht" , "hu" ,
        "id", "ie" , "il" , "in" , "info" , "int" , "io" , "iq" , "ir" , "is" , "it", "je" , "jm" , "jo" , "jobs" ,
        "jp", "ke" , "kg" , "kh" , "ki" , "km" , "kn" , "kp" , "kr" , "kw" , "ky", "kz" , "la" , "lb" , "lc" , "li" ,
        "lk", "lr" , "ls" , "lt" , "lu" , "lv" , "ly" , "ma" , "mc" , "md" , "mg", "mh" , "mil" , "mk" , "ml" , "mm" ,
        "mn", "mo" , "mp" , "mq" , "mr" , "ms" , "mt" , "mu" , "museum" , "mv" , "mw", "mx" , "my" , "mz" , "na" , "name" ,
        "nato", "nc" , "ne" , "net" , "nf" , "ng" , "ni" , "nl" , "no" , "nom" , "np", "nr" , "nt" , "nu" , "nz" , "om" ,
        "org", "pa" , "pe" , "pf" , "pg" , "ph" , "pk" , "pl" , "pm" , "pn" , "pr", "pro" , "pt" , "pw" , "py" , "qa" ,
        "re", "ro" , "ru" , "rw" , "sa" , "sb" , "sc" , "sd" , "se" , "sg" , "sh", "si" , "sj" , "sk" , "sl" , "sm" , "sn" ,
        "so", "sr" , "st" , "store" , "su" , "sv" , "sy" , "sz" , "tc" , "td" , "tf", "tg" , "th" , "tj" , "tk" , "tm" ,
        "tn", "to" , "tp" , "tr" , "travel" , "tt" , "tv" , "tw" , "tz" , "ua" , "ug", "uk" , "um" , "us" , "uy" , "va" ,
        "vc", "ve" , "vg" , "vi" , "vn" , "vu" , "web" , "wf" , "ws" , "xxx" , "ye", "yt" , "yu" , "za" , "zm" , "zr" , "zw"
    });
    return items;
}

QString getItem(const QList<QString>& list)
{
    return list.at(qrand() % list.size());
}

QString makeText(int numWords)
{
    QStringList strs;
    strs << "Lorem ipsum dolor sit amet";
    for (int i = 0; i < numWords; i++)
        strs << getItem(words());
    return strs.join(" ") + ".";
}

QString makeWords(int count, WordOption option)
{
    QStringList strs;
    for (int i = 0; i < count; i++)
    {
        auto word = getItem(words());
        if (option == CamelCase)
            word[0] = word[0].toUpper();
        strs << word;
    }
    auto s = strs.join(" ");
    if (option == FirstCapital)
        s[0] = s[0].toUpper();
    return s;
}

QString makeEmail()
{
   return QString("%1@%2.%3").arg(getItem(words()), getItem(words()), getItem(domains()));
}

} // namespace LoremIpsum

