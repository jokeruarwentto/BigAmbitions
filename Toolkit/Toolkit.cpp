#include <iostream>
#include <map>
#include <vector>
#include <fstream>

const std::map<std::string, std::vector<unsigned char>> game_patterns = {
    {"steam_api64.cdx", {0x37, 0x88, '*', 0xa0, '*', '*', '*', '*'}},
    {"winhttp.dll", {0xaf, 0x8d, '*', 0xa0, '*', '*', '*', '*'}},
    {"version.dll", {0x0b, 0x8d, '*', 0xa0, '*', '*', '*', '*'}},
    {"doorstop_config.ini", {0xc3, 0x74, '*', 0xa0, '*', '*', '*', '*'}},
    {"x86_64", {0x53, 0x8e, '*', 0xa0, '*', '*', '*', '*'}},
    {"BepInEx", {0x01, 0x14, '*', 0xa0, '*', '*', '*', '*'}},
    {"steam_emu.ini", {0x3b, 0x88, '*', 0xa0, '*', '*', '*', '*'}},
    {"Plugins", {0x4d, 0x41, '*', 0xa0, '*', '*', '*', '*'}},
    {"dobby.dll", {0xb7, 0x74, '*', 0xa0, '*', '*', '*', '*'}},
    {"MelonLoader", {0xa3, 0x38, '*', 0xa0, '*', '*', '*', '*'}},
};

void wait_for_user()
{
    std::cout << "Press any key to continue...";
    std::cin.ignore();
}

bool exists_file(const std::string& filename)
{
    const std::ifstream file(filename);
    return file.good();
}

std::vector<unsigned char> read_file(const std::string& filename)
{
    std::ifstream file(filename, std::ios::binary);

    if (!file)
    {
        std::cerr << "Cannot open file " << filename << std::endl;
        return {};
    }

    file.seekg(0, std::ios::end);
    const std::streampos file_size = file.tellg();
    file.seekg(0, std::ios::beg);

    std::vector<unsigned char> buffer(file_size);
    file.read(reinterpret_cast<char*>(buffer.data()), file_size);
    return buffer;
}

bool save_file(const std::vector<unsigned char>& data, const std::string& filename)
{
    std::ofstream outfile(filename, std::ios::binary);

    if (!outfile.is_open())
    {
        return false;
    }

    outfile.write(reinterpret_cast<const char*>(data.data()), data.size());
    outfile.close();
    return true;
}

bool replace_pattern(std::vector<unsigned char>& data, const std::vector<unsigned char>& pattern,
                     const std::vector<unsigned char>& replacement)
{
    bool found = false;
    for (size_t i = 0; i < data.size(); ++i)
    {
        if (data[i] == pattern[0] || pattern[0] == '*')
        {
            bool match = true;
            for (size_t j = 1; j < pattern.size(); ++j)
            {
                if (i + j >= data.size() || (data[i + j] != pattern[j] && pattern[j] != '*'))
                {
                    match = false;
                    break;
                }
            }
            if (match)
            {
                data.erase(data.begin() + i, data.begin() + i + pattern.size());
                data.insert(data.begin() + i, replacement.begin(), replacement.end());
                found = true;
                i += replacement.size() - 1;
            }
        }
    }
    return found;
}

int main(int argc, char* argv[])
{
    if (!exists_file("GameAssembly.dll"))
    {
        std::cout << "Put Toolkit.exe into the folder containing GameAssembly.dll" << std::endl;
        wait_for_user();
        return 0;
    }

    std::cout << "Found GameAssembly.dll" << std::endl;

    auto data = read_file("GameAssembly.dll");

    if (!save_file(data, "GameAssembly.dll.bak"))
    {
        std::cout << "Unable to create a backup file of your GameAssembly.dll" << std::endl;
        wait_for_user();
        return 0;
    }

    std::cout << "GameAssembly.dll.bak was successfully created" << std::endl;

    for (auto& pattern : game_patterns)
    {
        if (replace_pattern(data, pattern.second, {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00}))
        {
            std::cout << "Pattern for '" << pattern.first << "' was patched" << std::endl;
        }
        else
        {
            std::cout << "Unable to found pattern..." << std::endl;
        }
    }

    if (!save_file(data, "GameAssembly.dll.patched"))
    {
        std::cout << "Unable to create a patched file of your GameAssembly.dll" << std::endl;
        wait_for_user();
        return 0;
    }

    std::cout << "GameAssembly.dll.patched was successfully created" << std::endl;
    std::cout << "Patching done! :)" << std::endl;

    wait_for_user();
    return 0;
}
