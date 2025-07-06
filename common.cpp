#include <string>
#include "common.h"

namespace edi {

    std::string replaceWhitespaces(std::string& s) {
        std::string r = "_";
        return replaceWhitespaces(s, r);
    }

    std::string replaceWhitespaces(std::string& s, std::string& r) {
        std::string toReplace = " ";
        size_t start = 0;
        while((start = s.find(toReplace, start)) != std::string::npos) {
            s.replace(start, toReplace.length(), r);
            start += r.length();
        }
        return s;
    }

} // edi
