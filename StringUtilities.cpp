#include "StringUtilities.hpp"

#include <algorithm>
#include <locale>

namespace StringUtilities
{
    void to_lower(std::string& toConvert) {
        //A simple version of boost::to_lower.
        //To do: Test effect on internationalisation

        //convert to lower case
        //std::transform(toConvert.begin(), toConvert.end(), toConvert.begin(),
        //               std::bind2nd(std::ptr_fun(&std::tolower<char>), std::locale("")));

        std::transform(toConvert.begin(), toConvert.end(), toConvert.begin(), ::tolower);

    }
}
