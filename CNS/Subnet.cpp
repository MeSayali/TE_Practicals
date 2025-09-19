#include <iostream>
#include <sstream>
#include <cmath>

using namespace std;

// Function: Convert IP string (e.g., "192.168.1.1") into unsigned int format
unsigned int ipToInt(const string& ip) {
    unsigned int result = 0;
    int shift = 24;
    stringstream ss(ip);
    string token;
    while (getline(ss, token, '.')) {   // split by '.'
        result += stoi(token) << shift; // convert each part into int and shift
        shift -= 8;                     // move shift for next octet
    }
    return result;
}

// Function: Convert unsigned int back to dotted IP string
string intToIp(unsigned int ipInt) {
    return to_string((ipInt >> 24) & 0xFF) + "." +   // 1st octet
           to_string((ipInt >> 16) & 0xFF) + "." +   // 2nd octet
           to_string((ipInt >> 8) & 0xFF) + "." +    // 3rd octet
           to_string(ipInt & 0xFF);                  // 4th octet
}

// Structure to store IP Class details
struct IPClass {
    string name;          // Class name (A/B/C)
    unsigned int start;   // Starting IP of the class
    unsigned int end;     // Ending IP of the class
    int defaultMaskBits;  // Default mask bits
    string defaultMask;   // Default subnet mask in dotted decimal
};

// Predefined classes A, B, and C with ranges and masks
const IPClass classes[] = {
    {"A", ipToInt("0.0.0.0"), ipToInt("127.255.255.255"), 8, "255.0.0.0"},
    {"B", ipToInt("128.0.0.0"), ipToInt("191.255.255.255"), 16, "255.255.0.0"},
    {"C", ipToInt("192.0.0.0"), ipToInt("223.255.255.255"), 24, "255.255.255.0"}
};

// Function: Detect IP class based on IP value
const IPClass* getClass(unsigned int ip) {
    for (auto &c : classes) {
        if (ip >= c.start && ip <= c.end)  // check if in range
            return &c;
    }
    return nullptr;  // if not A/B/C
}

// Function: Generate subnet mask from number of mask bits
string maskFromBits(int bits) {
    unsigned int mask = 0xFFFFFFFF << (32 - bits); // create mask
    return intToIp(mask);
}

int main() {
    string ip;
    int choice;

    cout << "Enter IP Address (e.g., 192.168.1.0): ";
    cin >> ip;  // take IP from user

    unsigned int ipInt = ipToInt(ip);  // convert to int
    const IPClass* ipClass = getClass(ipInt);  // detect class

    if (!ipClass) {
        cout << "Invalid or unsupported IP address class." << endl;
        return 1;
    }

    // Show detected class and default mask
    cout << "Detected IP Class: " << ipClass->name << endl;
    cout << "Default subnet mask: " << ipClass->defaultMask << endl;

    // Ask user choice
    cout << "\nChoose option:\n";
    cout << "1. Subnetting based on number of subnets\n";
    cout << "2. Subnetting based on number of hosts per subnet\n";
    cout << "Enter choice: ";
    cin >> choice;

    int newMaskBits, noOfSubnets, hostsPerSubnet;

    switch (choice) {
        // ----------- Case 1: Based on required subnets -----------
        case 1: {
            int requiredSubnets;
            cout << "Enter number of required subnets: ";
            cin >> requiredSubnets;

            // find how many bits to borrow
            int n = 0;
            while (pow(2, n) < requiredSubnets) {
                n++;
            }
            newMaskBits = ipClass->defaultMaskBits + n;  // new mask length
            if (newMaskBits > 30) {  // sanity check
                cout << "Too many bits borrowed; less than 2 hosts possible." << endl;
                return 1;
            }

            int hostBits = 32 - newMaskBits;        // remaining host bits
            noOfSubnets = pow(2, n);                // subnets created
            hostsPerSubnet = pow(2, hostBits) - 2;  // usable hosts

            cout << "Bits borrowed: " << n << endl;
            cout << "New subnet mask: " << maskFromBits(newMaskBits) << " /" << newMaskBits << endl;
            cout << "Number of subnets: " << noOfSubnets << endl;
            cout << "Number of hosts per subnet: " << hostsPerSubnet << endl << endl;

            // Calculate network, broadcast, and range for each subnet
            unsigned int subnetSize = pow(2, hostBits);
            for (int i = 0; i < noOfSubnets; i++) {
                unsigned int subnetNetwork = (ipInt & (0xFFFFFFFF << (32 - newMaskBits))) + (i * subnetSize);
                unsigned int subnetBroadcast = subnetNetwork + subnetSize - 1;

                cout << "Subnet " << (i + 1) << ":\n";
                cout << "  Network Address:   " << intToIp(subnetNetwork) << "\n";
                cout << "  Broadcast Address: " << intToIp(subnetBroadcast) << "\n";
                cout << "  Host IP Range:     " << intToIp(subnetNetwork + 1) << " - " 
                     << intToIp(subnetBroadcast - 1) << "\n\n";
            }
            break;
        }

        // ----------- Case 2: Based on required hosts per subnet -----------
        case 2: {
            int requiredHosts;
            cout << "Enter number of required hosts per subnet: ";
            cin >> requiredHosts;

            // calculate required host bits
            int h = 0;
            while ((pow(2, h) - 2) < requiredHosts) {
                h++;
            }

            newMaskBits = 32 - h;                           // new subnet mask length
            int borrowedBits = newMaskBits - ipClass->defaultMaskBits;
            if (newMaskBits < ipClass->defaultMaskBits) {
                cout << "Not possible with this IP class." << endl;
                return 1;
            }

            hostsPerSubnet = pow(2, h) - 2;                 // usable hosts
            noOfSubnets = pow(2, borrowedBits);             // number of subnets

            cout << "Host bits (h): " << h << endl;
            cout << "New subnet mask: " << maskFromBits(newMaskBits) << " /" << newMaskBits << endl;
            cout << "Number of subnets: " << noOfSubnets << endl;
            cout << "Number of hosts per subnet: " << hostsPerSubnet << endl << endl;

            // Calculate subnet ranges
            unsigned int subnetSize = pow(2, h);
            for (int i = 0; i < noOfSubnets; i++) {
                unsigned int subnetNetwork = (ipInt & (0xFFFFFFFF << (32 - newMaskBits))) + (i * subnetSize);
                unsigned int subnetBroadcast = subnetNetwork + subnetSize - 1;

                cout << "Subnet " << (i + 1) << ":\n";
                cout << "  Network Address:   " << intToIp(subnetNetwork) << "\n";
                cout << "  Broadcast Address: " << intToIp(subnetBroadcast) << "\n";
                cout << "  Host IP Range:     " << intToIp(subnetNetwork + 1) << " - " 
                     << intToIp(subnetBroadcast - 1) << "\n\n";
            }
            break;
        }

        default:
            cout << "Invalid choice!" << endl;
    }

    return 0;
}
-
