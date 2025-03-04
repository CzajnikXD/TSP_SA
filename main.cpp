#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <random>
#include <cmath>
using namespace std;

bool **neighbours;
int **weights;
int *new_permutation;
int *rand_permutation;
int *best_permutation;
int *permutation;

int current_cost = 2147483647;
int best_cost = 2147483647;
int nodes, edges, x, y, z, test_a, test_b;
double T = 1000;

int calculate_cost(int* perm, int nodes)
{
    int cost = 0;
    for(int i = 0; i < nodes; i++) {
        cost += weights[perm[i]][perm[i+1]];
    }
    return cost;
}

void swap_random(int *perm, int nodes) {
    random_device rd;
    mt19937 gen(rd());
    uniform_int_distribution<int> start_dist(0, nodes-1);
    uniform_int_distribution<int> end_dist(1, nodes-1);

    int start_i = start_dist(gen);
    int start_j = start_dist(gen);
    if (start_i>start_j) swap(start_i,start_j);

    int end_i = end_dist(gen);
    int end_j = end_dist(gen);
    if (end_i>end_j) swap(end_i, end_j);

    if(T>0.1) {
        if (start_i == 0) {
            swap(perm[start_i], perm[start_j]);
            perm[nodes] = perm[0];
        } else {
            swap(perm[start_i], perm[start_j]);
        }
    }
    else{
        swap(perm[end_i],perm[end_j]);
    }

}

bool is_valid_cycle(int *perm, int nodes, bool **neighbours) {
    for (int i = 0; i < nodes; i++) {
        if (!neighbours[perm[i]][perm[i + 1]]) {
            return false;  // If any edge is missing, return false immediately
        }
    }
    return true;  // All edges exist, it's a valid cycle
}

int main() {
    bool result = false;
    ofstream outfile("Output3.txt");

    cout<<"Insert graph data:"<<endl;
    cin>>nodes>>edges;

    neighbours = new bool * [nodes];
    weights = new int * [nodes];
    rand_permutation = new int [nodes];
    permutation = new int [nodes+1];
    new_permutation = new int [nodes+1];
    best_permutation = new int [nodes+1];

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
        new_permutation[0] = rand_permutation[0];
        new_permutation[nodes] = rand_permutation[0];

        for (int i = 1; i < nodes; i++) {
            new_permutation[i] = rand_permutation[i];
        }

        // Checking if the random permutation is a valid cycle
        for (int i = 0; i < nodes; i++) {
            test_a = new_permutation[i];
            test_b = new_permutation[i + 1];

            if (not neighbours[test_a][test_b]) {
                result = false;
                break;
            }
            result = true;
        }
    }
    result = false;
    copy(new_permutation, new_permutation + nodes + 1, permutation);

    // Starting the main SA loop that decreases the temperature and creates valid permutations
    while(T>1e-5){
        swap_random(new_permutation, nodes);
        while (!is_valid_cycle(new_permutation, nodes, neighbours)) {
            swap_random(new_permutation, nodes);
        }

        int new_cost = calculate_cost(new_permutation, nodes);

        // If statement that compares new permutation with the current one
        // If the new cost is better than current one it is immediately accepted
        // If the new cost is worse it is accepted with a probability based on temperature
        if (new_cost < current_cost || exp((current_cost - new_cost) / T) > ((double) rand() / RAND_MAX)) {
            copy(new_permutation, new_permutation + nodes + 1, permutation);
            current_cost = new_cost;
        }

        if (new_cost < best_cost){
            copy(new_permutation, new_permutation + nodes + 1, best_permutation);
            best_cost = new_cost;
        }

        // Outputting the current results into output file
        outfile << "Current cycle: ";
        for (int i = 0; i < nodes + 1; i++) {
            outfile << permutation[i] << ":";
        }
        outfile << ", Current total weight: " << current_cost << endl;
        outfile << ";New permutation: ";
        for (int i = 0; i < nodes + 1; i++) {
            outfile << new_permutation[i] << ":";
        }
        outfile << ", New total weight: " << new_cost << endl;

        result = false;
        T *= 0.99;
        cout<<T<<endl;
    }

    cout << "Last selected Cycle: ";
    for (int i = 0; i < nodes + 1; i++) {
        cout << permutation[i] << ":";
    }
    cout << endl;
    cout << "Total weight of the last selected Cycle: " << calculate_cost(permutation,nodes) << endl;

    cout << "Best Cycle: ";
    for (int i = 0; i < nodes + 1; i++) {
        cout << best_permutation[i] << ":";
    }
    cout << endl;
    cout << "Total weight of the best Cycle: " << calculate_cost(best_permutation,nodes) << endl;

    // Deleting dynamic tables
    for(int i = 0; i < nodes; i++){
        delete [] neighbours[i];
        delete [] weights[i];
    }

    delete [] permutation;
    delete [] rand_permutation;
    delete [] new_permutation;
    delete [] best_permutation;
    delete [] neighbours;
    delete [] weights;

    outfile.close();
    return 0;
}