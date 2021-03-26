#ifndef _director_h__
#define _director_h__

#include "movie.h"
#include <cstring>


class DirectorComparator
{
    public:
        // IMPLEMENT BELOW inline
        bool operator( ) (const Movie::SecondaryKey & key1, const Movie::SecondaryKey & key2) const {

            // comparing directors

            int i;
            char a, b;
            std::string str1 = key1.getDirector();
            std::string str2 = key2.getDirector();

            for (i = 0, a = str1[0], b = str2[0]; a && b; i++, a = str1[i], b = str2[i]) {

                a = (a >= 'A' && a <= 'Z') ? a + 32 : a;
                b = (b >= 'A' && b <= 'Z') ? b + 32 : b;

                if (a < b) return true;
                else if (a > b) return false;
            }

            if (a) return false;     // str1 is longer
            else if (b) return true; // str2 is longer

            // directors are the same, comparing titles

            str1 = key1.getTitle();
            str2 = key2.getTitle();

            for (i = 0, a = str1[0], b = str2[0]; a && b; i++, a = str1[i], b = str2[i]) {

                a = (a >= 'A' && a <= 'Z') ? a + 32 : a;
                b = (b >= 'A' && b <= 'Z') ? b + 32 : b;

                if (a < b) return true;
                else if (a > b) return false;
            }

            if (a) return false;     // str1 is longer
            else if (b) return true; // str2 is longer

            return false;
        }
};

#endif
