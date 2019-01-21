
#include <bdn/Uri.h>

#include <regex>
#include <sstream>

namespace bdn
{

    String Uri::unescape(const String &s)
    {
        std::regex re("%([0-9a-fA-F]{2})");
        auto matchBegin = std::sregex_iterator(s.begin(), s.end(), re);
        auto matchEnd = std::sregex_iterator();

        String result;

        for (std::sregex_iterator i = matchBegin; i != matchEnd;) {
            std::smatch match = *i;

            result += match.prefix();

            std::istringstream stream(match[1]);
            int value;
            stream >> std::hex >> value;

            result += (char)value;

            ++i;

            if (i == matchEnd) {
                result += match.suffix();
            }
        }

        if (matchBegin == matchEnd) {
            result = s;
        }

        return result;
    }
}
