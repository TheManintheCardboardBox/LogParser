#include <filesystem>
#include <stdexcept>
#include <string>
#include <windows.h>

void ValidatePath(const std::filesystem::path& path)
{
    try
    {
        if (!std::filesystem::exists(path))
        {
            throw std::invalid_argument("Path does not exist");
        }

        if (!std::filesystem::is_regular_file(path))
        {
            throw std::invalid_argument("Path is not a regular file");
        }
    }
    catch (const std::filesystem::filesystem_error& e)
    {
        throw std::invalid_argument(std::string(
            "Could not access path\n"
            "%s", e.what()));
    }
}

void ValidateOption(const std::string& option)
{
    if (option == "-v" || option == "--invert-match")
    {
        return;
    }
    else
    {
        throw std::invalid_argument(
            "Invalid option\n"
            "Usage: LogParser.exe <path> [-v][--invert-match] <patterns>");
    }
}

void ValidateArgs(int argc, char* argv[])
{
    switch (argc)
    {
    case 3:
        ValidatePath(argv[1]);
        ValidatePath(argv[2]);
        break;

    case 4:
        ValidatePath(argv[1]);
        ValidateOption(argv[2]);
        ValidatePath(argv[3]);
        break;

    default:
        throw std::invalid_argument(
            "Invalid number of arguments\n"
            "Usage: LogParser.exe <path> [-v][--invert-match] <patterns>");
    }
}

bool InvertedMatch(int argc, char* argv[])
{
    ValidateArgs(argc, argv);
    return argc == 4;
}

unsigned long PageSize() noexcept
{
    SYSTEM_INFO systemInfo;
    ::GetSystemInfo(&systemInfo);
    return systemInfo.dwPageSize;
}
