#include <iostream>
#include <vector>
using namespace std;

#define INF 999

int main() {
    int n;
    cout << "Enter the number of routers: ";
    cin >> n;

    vector<vector<int>> graph(n, vector<int>(n));
    vector<vector<int>> distance(n, vector<int>(n));
    vector<vector<int>> nextHop(n, vector<int>(n));

    cout << "Enter the adjacency matrix (use " << INF << " for no direct connection):\n";
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            cin >> graph[i][j];
        }
    }

    // Initialize distance and nextHop
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            distance[i][j] = graph[i][j];
            if (i == j)
                nextHop[i][j] = i;
            else if (graph[i][j] != INF)
                nextHop[i][j] = j;
            else
                nextHop[i][j] = -1;
        }
    }

    // DVR Algorithm
    for (int k = 0; k < n; k++) {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < n; j++) {
                if (distance[i][k] != INF && distance[k][j] != INF &&
                    distance[i][j] > distance[i][k] + distance[k][j]) {
                    distance[i][j] = distance[i][k] + distance[k][j];
                    nextHop[i][j] = nextHop[i][k];
                }
            }
        }
    }

    // Print routing tables
    for (int i = 0; i < n; i++) {
        cout << "\nRouting table for Router " << i << ":\n";
        cout << "Destination\tDistance\tNext Hop\n";
        for (int j = 0; j < n; j++) {
            cout << j << "\t\t";
            if (distance[i][j] != INF)
                cout << distance[i][j] << "\t\t";
            else
                cout << "INF\t\t";

            if (nextHop[i][j] != -1)
                cout << nextHop[i][j];
            else
                cout << "-";
            cout << endl;
        }
    }

    return 0;
}

//Each router only knows direct costs to its neighbors:
