#include <iostream>
#include <filesystem>
#include <fstream>
#include <sstream>

#include "Parser.cpp"

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cerr << "No arguments passed. Please, specify a path to a .sal file";
        return -1;
    }

    for (int i = 1; i < 2; i++)
    {
        //std::cout << argv[i] << std::endl;
        if (std::filesystem::path(argv[i]).extension() != ".dopl" && std::filesystem::path(argv[i]).extension() != ".DOPL")
        {
            std::cerr << "File provided is not a .dopl file";
            return -1;
        }
        std::string path = argv[i];
        std::ifstream file(path);
        if (!file.is_open())
            return -1;
        stringstream sstream;
        sstream << file.rdbuf();
        Parser parser(sstream.str());
        bool res = parser.program();
        if (res)
        {
            std::cout << "ok" << std::endl;
            continue;
        }
        std::cout << "error" << std::endl;
    }
    return 0;
}