#include <bits/stdc++.h>
using namespace std;

// Structures
struct MacroTableEntry {
    int id;
    string macroName;
    int positionalCount, keywordCount, mdtIndex, kpdtIndex;
};

struct KeywordParamEntry {
    int id;
    string paramName;
    string defaultValue;
};

// Data structures
map<string, vector<string>> paramTable;       // Macro → list of parameters
map<string, map<string, int>> pntabIndex;     // Macro → (paramName → index)
vector<MacroTableEntry> macroTable;           // MNT
vector<KeywordParamEntry> keywordParamTable;  // KPDTAB
vector<string> macroDefTable;                 // MDT

int main() {
    ifstream input("input.asm");
    ofstream mntFile("mnt.txt");
    ofstream pntabFile("pntab.txt");
    ofstream kpdtFile("kpdt.txt");
    ofstream mdtFile("mdt.txt");

    string line;
    bool insideMacro = false;
    bool headerProcessed = false;
    string currentMacro;
    int mntCounter = 1, kpdtCounter = 1, mdtCounter = 1;

    while (getline(input, line)) {
        if (line.empty()) continue; // Skip empty lines

        stringstream ss(line);
        vector<string> tokens;
        string word;
        while (ss >> word) tokens.push_back(word); // Tokenize line by spaces

        // Start of a macro
        if (tokens[0] == "MACRO") {
            insideMacro = true;
            headerProcessed = false;
            currentMacro.clear(); // Reset for new macro
            continue;
        }

        // Macro header
        if (insideMacro && !headerProcessed) {
            currentMacro = tokens[0]; // Macro name (e.g., INCR, PRINT)

            // Collect full parameter string (spaces may split them)
            string joined;
            for (size_t i = 1; i < tokens.size(); i++) joined += tokens[i] + " ";

            // Split parameters by commas
            vector<string> params;
            string param;
            stringstream pss(joined);
            while (getline(pss, param, ',')) {
                if (!param.empty()) {
                    // Remove spaces
                    param.erase(remove_if(param.begin(), param.end(), ::isspace), param.end());
                    params.push_back(param);
                }
            }

            int positional = 0, keyword = 0, kpdtStart = 0;
            paramTable[currentMacro] = {};
            pntabIndex[currentMacro] = {};

            // Process each parameter
            for (auto &p : params) {
                if (p[0] == '&') {
                    size_t eq = p.find('=');
                    if (eq == string::npos) { // Positional parameter
                        string pname = p.substr(1);
                        paramTable[currentMacro].push_back(pname);
                        pntabIndex[currentMacro][pname] = positional + keyword + 1; // Store index for MDT
                        positional++;
                    } else { // Keyword parameter
                        string pname = p.substr(1, eq - 1);
                        string defval = (eq + 1 < p.size()) ? p.substr(eq + 1) : "---";
                        paramTable[currentMacro].push_back(pname);
                        pntabIndex[currentMacro][pname] = positional + keyword + 1;
                        keyword++;
                        if (keyword == 1) kpdtStart = kpdtCounter; // First keyword param position
                        keywordParamTable.push_back({kpdtCounter++, pname, defval});
                    }
                }
            }

            // Insert into MNT
            macroTable.push_back({mntCounter++, currentMacro, positional, keyword, mdtCounter, (keyword ? kpdtStart : -1)});
            headerProcessed = true; // Macro header processed
            continue;
        }

        // Macro body
        if (insideMacro) {
            if (tokens[0] == "MEND") { // End of macro
                macroDefTable.push_back("MEND");
                mdtCounter++;
                insideMacro = false;
                currentMacro.clear();
                continue;
            } else { // Inside macro body instructions
                string processed = "";
                for (auto &tok : tokens) {
                    if (tok.back() == ',') tok.pop_back(); // Remove trailing comma
                    if (tok[0] == '&') { // Replace parameter with PNTAB index
                        string pname = tok.substr(1);
                        int pos = pntabIndex[currentMacro][pname];
                        processed += "(P," + to_string(pos) + ") ";
                    } else {
                        processed += tok + " ";
                    }
                }
                macroDefTable.push_back(processed);
                mdtCounter++;
            }
        }
    }

    // Write MNT
    mntFile << "Index\tName\tPP\tKP\tMDTP\tKPDTP\n";
    for (auto &e : macroTable) {
        mntFile << e.id << "\t" << e.macroName << "\t" << e.positionalCount << "\t" << e.keywordCount
                << "\t" << e.mdtIndex << "\t" << (e.kpdtIndex == -1 ? string("---") : to_string(e.kpdtIndex)) << "\n";
    }

    // Write PNTAB
    pntabFile << "PNTAB (Macro → Parameters)\n";
    for (auto &kv : paramTable) {
        pntabFile << kv.first << " : ";
        for (auto &p : kv.second) pntabFile << p << " ";
        pntabFile << "\n";
    }

    // Write KPDTAB
    kpdtFile << "Index\tParam\tDefault\n";
    for (auto &e : keywordParamTable) {
        kpdtFile << e.id << "\t" << e.paramName << "\t" << e.defaultValue << "\n";
    }

    // Write MDT
    mdtFile << "MDT\n";
    for (int i = 0; i < macroDefTable.size(); i++) {
        mdtFile << (i + 1) << "\t" << macroDefTable[i] << "\n";
    }

    cout << "Pass-I Completed. Files generated: mnt.txt, pntab.txt, kpdt.txt, mdt.txt\n";
    return 0;
}