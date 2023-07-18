#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "explorer.hpp"


void parsePath(const std::string& rawPath, std::vector<std::string>& path)
{
    std::stringstream sPath(rawPath);
    std::string word;
    while (std::getline(sPath, word, '/')) {
        path.push_back(word);
    }
}


int main(int argc, char* argv[])
{
    if (argc < 2) {
        return 0;
    }

    Explorer explorer(argv[1]);
    while (std::cin) {

        std::vector<std::string>& wd = *explorer.getWD();
        if (wd.size() == 1) {
            std::cout << "/" << wd[0];
        } else {
            for (size_t i = 1; i < wd.size(); i++) {
                std::cout << "/" << wd[i];
            }
        }
        std::cout << "> ";

        // get input
        std::string line;
        std::getline(std::cin, line);
        std::stringstream sline(line);
        std::string word;
        std::getline(sline, word, ' ');

        if (word.compare("cd") == 0) {
            std::vector<std::string> path;
            std::getline(sline, word, ' ');
            parsePath(word, path);
            explorer.cd(path);
            continue;
        }

        if (word.compare("ls") == 0) {
            std::vector<std::string> path;
            uint8_t flags = 0x00;
            while (std::getline(sline, word, ' ')) {
                if (word[0] == '-') {
                    for (size_t i = 1; i < word.size(); i++) {
                        if (word[i] == 'l') {
                            flags |= LS_LIST;
                        }
                    }
                } else {
                    parsePath(word, path);
                }
            }
            explorer.ls(path, flags);
            continue;
        }

        if (word.compare("mkdir") == 0) {
            std::vector<std::string> path;
            std::getline(sline, word, ' ');
            parsePath(word, path);
            std::string foldername = path.back();
            path.pop_back();
            explorer.mkdir(path, foldername);
            continue;
        }

        if (word.compare("touch") == 0) {
            std::vector<std::string> path;
            std::getline(sline, word, ' ');
            parsePath(word, path);
            std::string filename = path.back();
            path.pop_back();
            explorer.touch(path, filename);
            continue;
        }

        if (word.compare("mv") == 0) {
            std::vector<std::string> path1;
            std::getline(sline, word, ' ');
            parsePath(word, path1);
            std::string filename = path1.back();
            path1.pop_back();
            std::vector<std::string> path2;
            std::getline(sline, word, ' ');
            parsePath(word, path2);
            explorer.mv(path1, filename, path2);
            continue;
        }

        if (word.compare("cat") == 0) {
            std::vector<std::string> path;
            std::getline(sline, word, ' ');
            parsePath(word, path);
            explorer.cat(path);
            continue;
        }

        if (word.compare("quit") == 0) {
            break;
        }
    }

    return 0;
}
