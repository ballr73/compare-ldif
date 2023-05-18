#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

// Function to split a string into tokens
std::vector<std::string> tokenize(const std::string& str, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(str);

    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

// Function to read LDIF file and extract entries with attributes
std::map<std::string, std::map<std::string, std::set<std::string>>> extractEntries(const std::string& filename) {
    std::ifstream file(filename);
    std::map<std::string, std::map<std::string, std::set<std::string>>> entries;
    std::string line, dn, attribute;

    while (std::getline(file, line)) {
        if (line.substr(0, 4) == "dn: ") {
            dn = line.substr(4);
        } else if (line.find(':') != std::string::npos) {
            std::vector<std::string> tokens = tokenize(line, ':');
            attribute = tokens[0];
            std::string value = tokens[1].substr(1); // Remove leading space after colon
            entries[dn][attribute].insert(value);
        }
    }

    file.close();
    return entries;
}

// Function to compare LDIF files and identify differences in entries and attributes
void compareLDIFFiles(const std::string& file1, const std::string& file2) {
    std::map<std::string, std::map<std::string, std::set<std::string>>> entries1 = extractEntries(file1);
    std::map<std::string, std::map<std::string, std::set<std::string>>> entries2 = extractEntries(file2);

    bool differencesFound = false;

    std::cout << "Entries present in " << file1 << " but not in " << file2 << ":\n";
    for (const auto& entry : entries1) {
        if (entries2.find(entry.first) == entries2.end()) {
            std::cout << entry.first << "\n";
            differencesFound = true;
        } else {
            const auto& attributes1 = entry.second;
            const auto& attributes2 = entries2[entry.first];

            for (const auto& attribute : attributes1) {
                const std::string& attributeName = attribute.first;
                const std::set<std::string>& values1 = attribute.second;

                if (attributes2.find(attributeName) == attributes2.end()) {
                    std::cout << "Difference in attribute for entry " << entry.first << ":\n";
                    std::cout << "  Attribute " << attributeName << " not present in " << file2 << "\n";
                    differencesFound = true;
                } else {
                    const std::set<std::string>& values2 = attributes2.at(attributeName);

                    for (const std::string& value : values1) {
                        if (values2.find(value) == values2.end()) {
                            std::cout << "Difference in attribute value for entry " << entry.first << ":\n";
                            std::cout << "  Attribute " << attributeName << ": Value " << value << " present in " << file1 << " but not in " << file2 << "\n";
                            differencesFound = true;
                        }
                    }

                    for (const std::string& value : values2) {
                        if (values1.find(value) == values1.end()) {
                            std::cout << "Difference in attribute value for entry " << entry.first << ":\n";
                            std::cout << "  Attribute " << attributeName << ": Value " << value << " present in " << file2 << " but not in " << file1 << "\n";
                            differencesFound = true;
                        }
                    }
                }
            }
        }
    }

    if (!differencesFound) {
        std::cout << "No differences found between " << file1 << " and " << file2 << "\n";
    }
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cout << "Usage: ./compareldif <file1.ldif> <file2.ldif>\n";
        return 1;
    }

    std::string file1 = argv[1];
    std::string file2 = argv[2];

    compareLDIFFiles(file1, file2);

    return 0;
}
