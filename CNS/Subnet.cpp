#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

// Convert IP string to unsigned int
unsigned int ipToInt(const string& ip) {
    unsigned int result = 0;
    int shift = 24;
    stringstream ss(ip);
    string token;
    while (getline(ss, token, '.')) {
        result += stoi(token) << shift;
        shift -= 8;
    }
    return result;
}

// Convert unsigned int to IP string
string intToIp(unsigned int ipInt) {
    return to_string((ipInt >> 24) & 0xFF) + "." +
           to_string((ipInt >> 16) & 0xFF) + "." +
           to_string((ipInt >> 8) & 0xFF) + "." +
           to_string(ipInt & 0xFF);
}

// IP Class info with static ranges and default masks
struct IPClass {
    string name;
    unsigned int start;
    unsigned int end;
    int defaultMaskBits;
    string defaultMask;
};

const IPClass classes[] = {
    {"A", ipToInt("0.0.0.0"), ipToInt("127.255.255.255"), 8, "255.0.0.0"},
    {"B", ipToInt("128.0.0.0"), ipToInt("191.255.255.255"), 16, "255.255.0.0"},
    {"C", ipToInt("192.0.0.0"), ipToInt("223.255.255.255"), 24, "255.255.255.0"}
};

// Determine IP class based on IP integer value
const IPClass* getClass(unsigned int ip) {
    for (auto &c : classes) {
        if (ip >= c.start && ip <= c.end)
            return &c;
    }
    return nullptr;
}

// Calculate subnet mask from bits
string maskFromBits(int bits) {
    unsigned int mask = 0xFFFFFFFF << (32 - bits);
    return intToIp(mask);
}

int main() {
    string ip;
    int requiredSubnets;

    cout << "Enter IP Address (e.g., 192.168.1.0): ";
    cin >> ip;
    cout << "Enter number of required subnets: ";
    cin >> requiredSubnets;

    unsigned int ipInt = ipToInt(ip);
    const IPClass* ipClass = getClass(ipInt);

    if (!ipClass) {
        cout << "Invalid or unsupported IP address class." << endl;
        return 1;
    }

    cout << "Detected IP Class: " << ipClass->name << endl;
    cout << "Default subnet mask: " << ipClass->defaultMask << endl;

    // Calculate bits to borrow
    int n = 0;
    while (pow(2, n) < requiredSubnets) {
        n++;
    }

    int newMaskBits = ipClass->defaultMaskBits + n;
    if (newMaskBits > 30) {
        cout << "Too many bits borrowed; less than 2 hosts per subnet possible." << endl;
        return 1;
    }

    int hostBits = 32 - newMaskBits;
    int noOfSubnets = pow(2, n);
    int hostsPerSubnet = pow(2, hostBits) - 2;

    cout << "Bits borrowed: " << n << endl;
    cout << "New subnet mask: " << maskFromBits(newMaskBits) << " /" << newMaskBits << endl;
    cout << "Number of subnets: " << noOfSubnets << endl;
    cout << "Number of hosts per subnet: " << hostsPerSubnet << endl << endl;

    unsigned int subnetSize = pow(2, hostBits);

    for (int i = 0; i < noOfSubnets; i++) {
        unsigned int subnetNetwork = (ipInt & (0xFFFFFFFF << (32 - newMaskBits))) + (i * subnetSize);
        unsigned int subnetBroadcast = subnetNetwork + subnetSize - 1;

        string network = intToIp(subnetNetwork);
        string broadcast = intToIp(subnetBroadcast);
        string hostRangeStart = intToIp(subnetNetwork + 1);
        string hostRangeEnd = intToIp(subnetBroadcast - 1);

        cout << "Subnet " << (i + 1) << ":\n";
        cout << "  Network Address:   " << network << "\n";
        cout << "  Broadcast Address: " << broadcast << "\n";
        cout << "  Host IP Range:     " << hostRangeStart << " - " << hostRangeEnd << "\n\n";
    }

    return 0;
}
