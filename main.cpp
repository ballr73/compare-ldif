#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <set>
#include <sstream>

using namespace std;

// Function to split a string into tokens
vector<string> tokenize(const string& str, char delimiter) {
    vector<string> tokens;
    string token;
    istringstream tokenStream(str);

    while (getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }

    return tokens;
}

// Function to read LDIF file and extract entries with attributes
map<string, map<string, set<string>>> extractEntries(const string& filename) {
    ifstream file(filename);
    map<string, map<string, set<string>>> entries;
    string line, dn, attribute;

    while (getline(file, line)) {
        if (line.substr(0, 4) == "dn: ") {
            dn = line.substr(4);
        } else if (line.find(':') != string::npos) {
            vector<string> tokens = tokenize(line, ':');
            attribute = tokens[0];
            string value = tokens[1].substr(1); // Remove leading space after colon
            entries[dn][attribute].insert(value);
        }
    }

    file.close();
    return entries;
}

// Function to compare LDIF files and identify differences in entries and attributes
void compareLDIFFiles(const string& file1, const string& file2, const string& outputFilename) {
    map<string, map<string, set<string>>> entries1 = extractEntries(file1);
    map<string, map<string, set<string>>> entries2 = extractEntries(file2);

    ofstream outputFile(outputFilename);

    for (const auto& entry : entries1) {
        if (entries2.find(entry.first) != entries2.end()) {
            const auto& attributes1 = entry.second;
            auto& attributes2 = entries2[entry.first];

            for (const auto& attribute : attributes1) {
                const string& attributeName = attribute.first;
                const set<string>& values1 = attribute.second;

                if (attributes2.find(attributeName) == attributes2.end()) {
                    // Attribute not present in the second file, add it with the values
                    outputFile << "dn: " << entry.first << "\n";
                    outputFile << "changetype: modify\n";
                    outputFile << "add: " << attributeName << "\n";

                    for (const string& value : values1) {
                        outputFile << attributeName << ": " << value << "\n";
                    }

                    outputFile << "-\n";
                } else {
                    auto& values2 = attributes2[attributeName];

                    for (const string& value : values1) {
                        if (values2.find(value) == values2.end()) {
                            // Value not present in the second file, add it
                            outputFile << "dn: " << entry.first << "\n";
                            outputFile << "changetype: modify\n";
                            outputFile << "add: " << attributeName << "\n";
                            outputFile << attributeName << ": " << value << "\n";
                            outputFile << "-\n";

                            values2.insert(value);
                        }
                    }

                    for (auto it = values2.begin(); it != values2.end();) {
                        if (values1.find(*it) == values1.end()) {
                            // Value present in the second file but not in the first file, delete it
                            outputFile << "dn: " << entry.first << "\n";
                            outputFile << "changetype: modify\n";
                            outputFile << "delete: " << attributeName << "\n";
                            outputFile << attributeName << ": " << *it << "\n";
                            outputFile << "-\n";

                            it = values2.erase(it);
                        } else {
                            ++it;
                        }
                    }

                    if (values2.empty()) {
                        // Attribute no longer has any values, delete it from the second file
                        outputFile << "dn: " << entry.first << "\n";
                        outputFile << "changetype: modify\n";
                        outputFile << "delete: " << attributeName << "\n";
                        outputFile << "-\n";

                        attributes2.erase(attributeName);
                    }
                }
            }

            for (const auto& attribute : attributes2) {
                const string& attributeName = attribute.first;
                const set<string>& values2 = attribute.second;

                if (attributes1.find(attributeName) == attributes1.end()) {
                    // Attribute not present in the first file, delete it from the second file
                    for (const string& value : values2) {
                        outputFile << "dn: " << entry.first << "\n";
                        outputFile << "changetype: modify\n";
                        outputFile << "delete: " << attributeName << "\n";
                        outputFile << attributeName << ": " << value << "\n";
                        outputFile << "-\n";
                    }

                    attributes2.erase(attributeName);
                }
            }
        } else {
            // Entry not present in the second file, add it with all attributes
            outputFile << "dn: " << entry.first << "\n";
            outputFile << "changetype: add\n";

            for (const auto& attribute : entry.second) {
                const string& attributeName = attribute.first;

                for (const string& value : attribute.second) {
                    outputFile << attributeName << ": " << value << "\n";
                }
            }

            outputFile << "-\n";
        }
    }

    for (const auto& entry : entries2) {
        if (entries1.find(entry.first) == entries1.end()) {
            // Entry present in the second file but not in the first file, delete it
            outputFile << "dn: " << entry.first << "\n";
            outputFile << "changetype: delete\n";
            outputFile << "-\n";
        }
    }

    outputFile.close();

    cout << "Changes saved to " << outputFilename << "\n";
}

int main(int argc, char* argv[]) {
    string arg1 = argv[1];
    // Check if the count and output file name arguments are provided
    if(argc == 2 && arg1 == "-v") {
        cout << "cldif version v0.0.6" << endl;
        return 1;
    }
    if (argc < 4) {
        cout << "Usage: ./compareldif <file1.ldif> <file2.ldif> <output.ldif>\n";
        return 1;
    }

    string file1 = argv[1];
    string file2 = argv[2];
    string outputFilename = argv[3];

    compareLDIFFiles(file1, file2, outputFilename);

    return 0;
}
