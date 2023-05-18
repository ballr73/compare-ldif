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
void compareLDIFFiles(const std::string& file1, const std::string& file2, const std::string& outputFilename) {
    std::map<std::string, std::map<std::string, std::set<std::string>>> entries1 = extractEntries(file1);
    std::map<std::string, std::map<std::string, std::set<std::string>>> entries2 = extractEntries(file2);

    std::ofstream outputFile(outputFilename);

    for (const auto& entry : entries1) {
        if (entries2.find(entry.first) != entries2.end()) {
            const auto& attributes1 = entry.second;
            const auto& attributes2 = entries2[entry.first];

            for (const auto& attribute : attributes1) {
                const std::string& attributeName = attribute.first;
                const std::set<std::string>& values1 = attribute.second;

                if (attributes2.find(attributeName) == attributes2.end()) {
                    // Attribute not present in the second file, add it with the values
                    outputFile << "dn: " << entry.first << "\n";
                    outputFile << "changetype: modify\n";
                    outputFile << "add: " << attributeName << "\n";

                    for (const std::string& value : values1) {
                        outputFile << attributeName << ": " << value << "\n";
                    }

                    outputFile << "-\n";
                } else {
                    const std::set<std::string>& values2 = attributes2.at(attributeName);

                    for (const std::string& value : values1) {
                        if (values2.find(value) == values2.end()) {
                            // Value not present in the second file, add it
                            outputFile << "dn: " << entry.first << "\n";
                            outputFile << "changetype: modify\n";
                            outputFile << "add: " << attributeName << "\n";
                            outputFile << attributeName << ": " << value << "\n";
                            outputFile << "-\n";
                        }
                    }
                }
            }
        } else {
            // Entry not present in the second file, add it with all attributes
            outputFile << "dn: " << entry.first << "\n";
            outputFile << "changetype: add\n";

            for (const auto& attribute : entry.second) {
                const std::string& attributeName = attribute.first;

                for (const std::string& value : attribute.second) {
                    outputFile << attributeName << ": " << value << "\n";
                }
            }

            outputFile << "-\n";
        }
    }

    outputFile.close();

    std::cout << "Changes saved to " << outputFilename << "\n";
}

int main(int argc, char* argv[]) {
    if (argc < 4) {
        std::cout << "Usage: ./compareldif <file1.ldif> <file2.ldif> <output.ldif>\n";
        return 1;
    }

    std::string file1 = argv[1];
    std::string file2 = argv[2];
    std::string outputFilename = argv[3];

    compareLDIFFiles(file1, file2, outputFilename);

    return 0;
}
