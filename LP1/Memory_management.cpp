#include <iostream>
#include <map>
#include <iomanip>
#include <algorithm>
using namespace std;

class Memory {
    int *blocks;
    int *process;
    int *flag;
    int b_size; //block size
    int p_size; //process size

public:
    Memory() {
        b_size = 0;
        p_size = 0;
    }
    void take_input();
    void first_fit();
    void best_fit();
    void worst_fit();
    void next_fit();
    void give_output(map<int, int>, string);
};

void Memory::take_input() {
    cout << "\nEnter number of blocks: ";
    cin >> b_size;
    cout << "\nEnter the number of processes: ";
    cin >> p_size;
    blocks = new int[b_size];
    process = new int[p_size];
    flag = new int[b_size];

    cout << "\nEnter size of the blocks: ";
    for (int i = 0; i < b_size; i++) {
        cin >> blocks[i];
    }

    cout << "\nEnter the size of processes: ";
    for (int i = 0; i < p_size; i++) {
        cin >> process[i];
    }
}

void Memory::give_output(map<int, int> mp, string str) {
    cout << "\n-------------------" << str << "------------------------\n";
    cout << "Block No" << setw(20) << "Size of Block" << setw(20)
         << "Size of Process " << setw(20) << "Waste memory" << endl;

    for (auto it = mp.begin(); it != mp.end(); it++) {
        cout << it->first << setw(20) << blocks[it->first] << setw(20)
             << it->second << setw(20)
             << blocks[it->first] - it->second << endl;
    }
}

void Memory::first_fit() {
    for (int i = 0; i < b_size; i++) {
        flag[i] = 0; // free
    }

    map<int, int> mp;

    for (int i = 0; i < p_size; i++) {
        for (int j = 0; j < b_size; j++) {
            if (process[i] <= blocks[j] && flag[j] == 0) {
                mp[j] = process[i];
                flag[j] = 1; // mark block as used
                break;
            }
        }
    }

    string str = "First Fit Algorithm";
    give_output(mp, str);
}

void Memory::best_fit() {
    for (int i = 0; i < b_size; i++) {
        flag[i] = 0; // free
    }
    int wastage[b_size];

    map<int, int> mp;

    for (int i = 0; i < p_size; i++) {
        for (int k = 0; k < b_size; k++) {
            wastage[k] = 9999; // default large value
        }

        for (int j = 0; j < b_size; j++) {
            if (process[i] <= blocks[j] && flag[j] == 0) {
                wastage[j] = blocks[j] - process[i];
            }
        }

        int min = *min_element(wastage, wastage + b_size);

        if (min == 9999) {
            cout << "Process " << process[i] << " cannot be allocated." << endl;
            continue;
        }

        int pos = find(wastage, wastage + b_size, min) - wastage;
        mp[pos] = process[i];
        flag[pos] = 1;
    }

    string str = "Best Fit Algorithm";
    give_output(mp, str);
}

void Memory::worst_fit() {
    for (int i = 0; i < b_size; i++) {
        flag[i] = 0; // free
    }

    map<int, int> mp;

    for (int i = 0; i < p_size; i++) {
        int max_waste = -1;
        int pos = -1;

        for (int j = 0; j < b_size; j++) {
            if (flag[j] == 0 && process[i] <= blocks[j]) {
                int waste = blocks[j] - process[i];
                if (waste > max_waste) {
                    max_waste = waste;
                    pos = j;
                }
            }
        }

        if (pos != -1) {
            mp[pos] = process[i];
            flag[pos] = 1;
        } else {
            cout << "Process " << process[i] << " cannot be allocated." << endl;
        }
    }

    string str = "Worst Fit Algorithm";
    give_output(mp, str);
}

void Memory::next_fit() {
    for (int i = 0; i < b_size; i++) {
        flag[i] = 0; // free
    }

    map<int, int> mp;
    int start_pos = 0;

    for (int i = 0; i < p_size; i++) {
        int proc_size = process[i];
        bool allocated = false;

        int count = 0;
        int idx = start_pos;

        while (count < b_size) {
            if (flag[idx] == 0 && blocks[idx] >= proc_size) {
                flag[idx] = 1;
                mp[idx] = proc_size;
                allocated = true;
                start_pos = (idx + 1) % b_size; // next search continues
                break;
            }
            idx = (idx + 1) % b_size;
            count++;
        }

        if (!allocated) {
            cout << "Process " << proc_size << " cannot be allocated." << endl;
        }
    }

    string str = "Next Fit Algorithm";
    give_output(mp, str);
}

int main() {
    Memory m;
    m.take_input();
    m.first_fit();
    m.best_fit();
    m.worst_fit();
    m.next_fit();
    return 0;
}
