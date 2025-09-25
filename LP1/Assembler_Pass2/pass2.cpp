#include <bits/stdc++.h>
using namespace std;

struct Symbol {
    string name;
    int address;
};

struct Literal {
    string name;
    int address;
};

// ---------- Load Symbol Table ----------
map<int, Symbol> loadSymtab(string filename) {
    map<int, Symbol> symtab;
    ifstream file(filename);
    int index, addr;
    string name;
    while (file >> index >> name >> addr) {
        symtab[index] = {name, addr};
    }
    return symtab;
}

// ---------- Load Literal Table ----------
map<int, Literal> loadLittab(string filename) {
    map<int, Literal> littab;
    ifstream file(filename);
    int index, addr;
    string name;
    while (file >> index >> name >> addr) {
        littab[index] = {name, addr};
    }
    return littab;
}

// ---------- Pass-II Implementation ----------
void pass2() {
    map<int, Symbol> SYMTAB = loadSymtab("symtab.txt");
    map<int, Literal> LITTAB = loadLittab("littab.txt");

    ifstream ic("ic.txt");
    ofstream out("output.txt");

    string line;
    int LC = 0;

    while (getline(ic, line)) {
        if (line.find("(AD") != string::npos) {
            // Handle assembler directives
            if (line.find("C,") != string::npos) {
                int start = line.find("C,") + 2;
                int end = line.find(")", start);
                LC = stoi(line.substr(start, end - start));
            }
            continue; // Skip writing output
        }

        if (line.find("(DL,01)") != string::npos) { // DC
            int start = line.find("C,") + 2;
            int end = line.find(")", start);
            string val = line.substr(start, end - start);
            out << LC << " 00 00 " << val << "\n";
        }
        else if (line.find("(DL,02)") != string::npos) { // DS
            out << LC << " 00 00 000\n";
        }
        else if (line.find("(IS") != string::npos) { // Imperative Statement
            stringstream ss(line);
            string token;
            vector<string> tokens;
            while (ss >> token) {
                token.erase(remove(token.begin(), token.end(), '('), token.end());
                token.erase(remove(token.begin(), token.end(), ')'), token.end());
                tokens.push_back(token);
            }

            string opcode = tokens[0].substr(tokens[0].find(",") + 1); // e.g., "04"
            string reg = tokens[1].substr(tokens[1].find(",") + 1);   // e.g., "1"
            string operand = tokens[2];

            string addr = "000";
            if (operand[0] == 'S') {
                int index = stoi(operand.substr(2));
                addr = to_string(SYMTAB[index].address);
            }
            else if (operand[0] == 'L') {
                int index = stoi(operand.substr(2));
                addr = to_string(LITTAB[index].address);
            }
            else if (operand[0] == 'C') {
                addr = operand.substr(2);
            }

            out << LC << " " << opcode << " " << reg << " " << addr << "\n";
        }

        LC++;
    }

    cout << "✅ Pass-II completed. Output written to output.txt\n";
}

int main() {
    pass2();
    return 0;
}
