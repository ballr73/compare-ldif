#include <iostream>
#include <fstream>
#include <string>

bool compareFiles(const std::string& file1, const std::string& file2) {
    std::ifstream fileStream1(file1);
    std::ifstream fileStream2(file2);

    if (!fileStream1 || !fileStream2) {
        std::cout << "Error opening files." << std::endl;
        return false;
    }

    std::string line1, line2;
    int lineNumber = 1;
    bool filesEqual = true;

    while (std::getline(fileStream1, line1) && std::getline(fileStream2, line2)) {
        if (line1 != line2) {
            std::cout << "Files differ at line " << lineNumber << std::endl;
            std::cout << "File 1: " << line1 << std::endl;
            std::cout << "File 2: " << line2 << std::endl;
            filesEqual = false;
            break;
        }
        lineNumber++;
    }

    if (filesEqual) {
        if (fileStream1.eof() && !fileStream2.eof()) {
            std::cout << "File 2 has additional content at line " << lineNumber << std::endl;
            filesEqual = false;
        }
        else if (!fileStream1.eof() && fileStream2.eof()) {
            std::cout << "File 1 has additional content at line " << lineNumber << std::endl;
            filesEqual = false;
        }
    }

    fileStream1.close();
    fileStream2.close();

    return filesEqual;
}

int main() {
    std::string file1, file2;
    std::cout << "Enter the path to the first file: ";
    std::cin >> file1;
    std::cout << "Enter the path to the second file: ";
    std::cin >> file2;

    if (compareFiles(file1, file2)) {
        std::cout << "The files are identical." << std::endl;
    }
    else {
        std::cout << "The files are different." << std::endl;
    }

    return 0;
}
