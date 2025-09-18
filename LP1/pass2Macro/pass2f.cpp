#include <bits/stdc++.h>
using namespace std;

// Structures
struct MacroTableEntry {
    string name;
    int mdtIndex;
    int ppCount;
    int kpCount;
    int kpdtIndex;
};

struct KeywordParamEntry {
    string paramName;
    string defaultValue;
};

// Data structures
map<string, MacroTableEntry> mnt;
map<string, vector<string>> pntab;
map<string, map<string, int>> pntabIndex;
map<int, KeywordParamEntry> kpdt;
vector<string> mdt;

int main() {
    ifstream source("source.asm"); // macro calls
    ifstream mntFile("mnt.txt");
    ifstream pntabFile("pntab.txt");
    ifstream kpdtFile("kpdt.txt");
    ifstream mdtFile("mdt.txt");
    ofstream expanded("expanded.asm");

    // Load MNT
    string line;
    getline(mntFile, line); // skip header
    while (getline(mntFile, line)) {
        stringstream ss(line);
        int id, pp, kp, mdtp, kpdtp;
        string name;
        ss >> id >> name >> pp >> kp >> mdtp >> kpdtp;
        mnt[name] = {name, mdtp, pp, kp, (kpdtp == -1 ? -1 : kpdtp)};
    }

    // Load PNTAB
    getline(pntabFile, line); // skip header
    while (getline(pntabFile, line)) {
        size_t colon = line.find(':');
        string macroName = line.substr(0, colon - 1);
        string paramList = line.substr(colon + 2);
        stringstream ss(paramList);
        string token;
        while (ss >> token) {
            size_t open = token.find('(');
            string pname = token.substr(0, open);
            int index = stoi(token.substr(open + 1, token.find(')') - open - 1));
            pntab[macroName].push_back(pname);
            pntabIndex[macroName][pname] = index;
        }
    }

    // Load KPDTAB
    getline(kpdtFile, line); // skip header
    while (getline(kpdtFile, line)) {
        stringstream ss(line);
        int id;
        string pname, defval;
        ss >> id >> pname >> defval;
        kpdt[id] = {pname, defval};
    }

    // Load MDT
    getline(mdtFile, line); // skip header
    while (getline(mdtFile, line)) {
        size_t tab = line.find('\t');
        mdt.push_back(line.substr(tab + 1));
    }

    // Process source.asm
    while (getline(source, line)) {
        if (line.empty()) continue;

        stringstream ss(line);
        vector<string> tokens;
        string word;
        while (ss >> word) tokens.push_back(word);

        string macroName = tokens[0];
        if (mnt.find(macroName) == mnt.end()) {
            expanded << line << "\n"; // not a macro call
            continue;
        }

        // Macro call
        MacroTableEntry entry = mnt[macroName];
        vector<string> actualArgs;
        string joined;
        for (size_t i = 1; i < tokens.size(); i++) joined += tokens[i] + " ";
        stringstream argStream(joined);
        string arg;
        while (getline(argStream, arg, ',')) {
            arg.erase(remove_if(arg.begin(), arg.end(), ::isspace), arg.end());
            actualArgs.push_back(arg);
        }

        // Build APTAB
        map<int, string> aptab;
        int argIndex = 1;

        // Positional arguments
        for (int i = 0; i < entry.ppCount && i < actualArgs.size(); i++) {
            aptab[argIndex++] = actualArgs[i];
        }

        // Keyword arguments
        map<string, string> keywordArgs;
        for (size_t i = entry.ppCount; i < actualArgs.size(); i++) {
            size_t eq = actualArgs[i].find('=');
            if (eq != string::npos) {
                string pname = actualArgs[i].substr(0, eq);
                string val = actualArgs[i].substr(eq + 1);
                keywordArgs[pname] = val;
            }
        }

        for (int i = 0; i < entry.kpCount; i++) {
            KeywordParamEntry kp = kpdt[entry.kpdtIndex + i];
            string val = keywordArgs.count(kp.paramName) ? keywordArgs[kp.paramName] : kp.defaultValue;
            aptab[argIndex++] = val;
        }

        // Expand macro
        int mdtIndex = entry.mdtIndex - 1;
        while (mdt[mdtIndex] != "MEND") {
            string expandedLine = "";
            stringstream body(mdt[mdtIndex]);
            string token;
            while (body >> token) {
                size_t pStart = token.find("(P,");
                if (pStart != string::npos) {
                    size_t pEnd = token.find(')', pStart);
                    int idx = stoi(token.substr(pStart + 3, pEnd - pStart - 3));
                    string before = token.substr(0, pStart);
                    string after = token.substr(pEnd + 1);
                    expandedLine += before + aptab[idx] + after + " ";
                } else {
                    expandedLine += token + " ";
                }
            }
            expanded << expandedLine << "\n";
            mdtIndex++;
        }
    }

    cout << "Pass-II Completed. Expanded code written to expanded.asm\n";
    return 0;
}