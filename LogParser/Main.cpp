#include <stdexcept>
#include "LogParser.h"
#include "Utils.h"

int main(int argc, char* argv[])
{
    try
    {
        auto inverted = InvertedMatch(argc, argv);
        if (inverted)
        {
            LogParser parser(argv[1], argv[3], true);
            parser.ReverseSearch();
        }
        else
        {
            LogParser parser(argv[1], argv[2], false);
            parser.Search();
        }
    }
    catch (const std::invalid_argument& e)
    {
        std::cout << e.what() << std::endl;
    }

    return 0;
}
