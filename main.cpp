#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <random>
#include <cmath>
using namespace std;

double temperature(int t) {
    return 1000*pow(0.9987, t);
}

int tempCmax = 2147483647;
bool **neighbours;
int **weights;
int *rand_permutation;
int *permutation;

int main() {
    int nodes, edges, x, y, z, test_a, test_b;
    bool result = false;

    cout<<"Insert graph data:"<<endl;
    cin>>nodes>>edges;

    neighbours = new bool * [nodes];
    weights = new int * [nodes];
    rand_permutation = new int [nodes];
    permutation = new int [nodes+1];

    // Table initialization
    for(int i = 0; i < nodes; i++){
        neighbours[i] = new bool [nodes];
        weights[i] = new int [nodes];
        for(int j = 0; j < nodes; j++){
            neighbours[i][j] = false;
            weights[i][j] = 0;
        }
    }

    // Data loading
    for(int i = 0; i < edges; i++){
        cin >> x >> y >> z;
        neighbours[x][y] = neighbours[y][x] = true; // Connection between edges x-y is present, so the bool is true
        weights[x][y] = weights[y][x] = z; // Weights between x-y is set to z
    }

    while(result == false) {
        // Filling table with numbers
        for (int i = 0; i < nodes; i++) {
            rand_permutation[i] = i;
        }

        // Randomly shuffling a permutation
        random_device rd;
        mt19937 gen(rd());
        shuffle(rand_permutation, rand_permutation + nodes, gen);

        // Filling the table with correctly prepared random permutation
        permutation[0] = rand_permutation[0];
        permutation[nodes] = rand_permutation[0];

        for (int i = 1; i < nodes; i++) {
            permutation[i] = rand_permutation[i];
        }

        for (int i = 0; i < nodes + 1; i++) {
            cout << permutation[i] << ":";
        }
        cout << endl;

        int test_permutation[] = {0, 1, 6, 7, 3, 5, 2, 4, 0};

        // Checking if the random permutation is a valid Hamilton Cycle
        for (int i = 0; i < nodes; i++) {
            test_a = permutation[i];
            test_b = permutation[i + 1];

            if (not neighbours[test_a][test_b]) {
                cout << "Non Hamilton Cycle detected, breaking the loop" << endl;
                result = false;
                break;
            }
            result = true;
        }
    }

    cout << "Hamilton cycle found: ";
    for (int i = 0; i < nodes + 1; i++) {
        cout << permutation[i] << ":";
    }

    // Deleting dynamic tables
    for(int i = 0; i < nodes; i++){
        delete [] neighbours[i];
        delete [] weights[i];
    }

    delete [] permutation;
    delete [] rand_permutation;
    delete [] neighbours;
    delete [] weights;

    return 0;
}