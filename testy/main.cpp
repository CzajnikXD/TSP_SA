#include <iostream>
#include <fstream>
#include <algorithm>
#include <chrono>
#include <random>
#include <cmath>
#include <windows.h>

using namespace std;

bool **neighbours;
int **weights;
int *new_permutation;
int *rand_permutation;
int *best_permutation;
int *permutation;

random_device rd;
mt19937 gen(rd());

int current_cost = 2147483647;
int best_cost = 2147483647;
int nodes, edges, x, y, z;
int test_a, test_b;

int calculate_cost(int* perm, int nodes)
{
    int cost = 0;
    for(int i = 0; i < nodes; i++) {
        cost += weights[perm[i]][perm[i+1]];
    }
    return cost;
}

void swap_random(int *perm, int nodes, double T) {
    uniform_int_distribution<int> start_dist(0, nodes-1);
    uniform_int_distribution<int> end_dist(1, nodes-1);

    int start_i = start_dist(gen);
    int start_j = start_dist(gen);
    if (start_i > start_j) swap(start_i,start_j);

    int end_i = end_dist(gen);
    int end_j = end_dist(gen);
    if (end_i > end_j) swap(end_i, end_j);

    if(T > 0.001) {
        if (start_i == 0) {
            swap(perm[start_i], perm[start_j]);
            perm[nodes] = perm[0];
        } else {
            swap(perm[start_i], perm[start_j]);
        }
    }
    else {
        swap(perm[end_i], perm[end_j]);
    }
}

bool is_valid_cycle(int *perm, int nodes, bool **neighbours) {
    for (int i = 0; i < nodes; i++) {
        if (!neighbours[perm[i]][perm[i + 1]]) return false;
    }
    return true;
}

int run_SA(double T_start, double cooling_rate, int verbose = 0) {
    double T = T_start;
    best_cost = current_cost = 2147483647;

    // Losowa poprawna permutacja początkowa
    bool result = false;
    while (!result) {
        for (int i = 0; i < nodes; i++) rand_permutation[i] = i;
        shuffle(rand_permutation, rand_permutation + nodes, gen);

        new_permutation[0] = rand_permutation[0];
        new_permutation[nodes] = rand_permutation[0];

        for (int i = 1; i < nodes; i++) new_permutation[i] = rand_permutation[i];

        result = is_valid_cycle(new_permutation, nodes, neighbours);
    }
    copy(new_permutation, new_permutation + nodes + 1, permutation);

    // Główna pętla SA
    while (T > 1e-5) {
        swap_random(new_permutation, nodes, T);
        while (!is_valid_cycle(new_permutation, nodes, neighbours)) {
            swap_random(new_permutation, nodes, T);
        }

        int new_cost = calculate_cost(new_permutation, nodes);

        if (new_cost < current_cost || exp((current_cost - new_cost) / T) > ((double) rand() / RAND_MAX)) {
            copy(new_permutation, new_permutation + nodes + 1, permutation);
            current_cost = new_cost;
        }

        if (new_cost < best_cost) {
            copy(new_permutation, new_permutation + nodes + 1, best_permutation);
            best_cost = new_cost;
        }

        T *= cooling_rate;
        if (verbose) cout << "Temp: " << T << " | Cost: " << best_cost << endl;
    }

    return best_cost;
}

void run_tests() {
    const int TESTS = 30;
    double T_start_values[] = {100000, 75000, 50000, 25000, 10000};
    double cooling_rates[] = {0.999, 0.995, 0.99, 0.95, 0.9};

    char current_path[MAX_PATH];
    GetCurrentDirectory(MAX_PATH, current_path);

    string file_path = string(current_path) + "/SA_results.csv";

    ofstream test_results(file_path);
    test_results << "T_start,cooling_rate,average_cost,average_time_ms\n";

    for (double T_start : T_start_values) {
        for (double cooling : cooling_rates) {
            double total_cost = 0;
            double total_time = 0;

            for (int i = 0; i < TESTS; ++i) {
                auto start = chrono::high_resolution_clock::now();
                int result_cost = run_SA(T_start, cooling, 0);
                auto end = chrono::high_resolution_clock::now();
                double duration = chrono::duration_cast<chrono::milliseconds>(end - start).count();

                total_cost += result_cost;
                total_time += duration;
            }

            test_results << T_start << "," << cooling << ","
                         << (total_cost / TESTS) << "," << (total_time / TESTS) << "\n";
        }
    }

    test_results.close();
    cout << "Testy zakończone. Wyniki zapisano do SA_results.csv" << endl;
}

int main() {
    cout << "Insert graph data:" << endl;
    cin >> nodes >> edges;

    neighbours = new bool * [nodes];
    weights = new int * [nodes];
    rand_permutation = new int [nodes];
    permutation = new int [nodes+1];
    new_permutation = new int [nodes+1];
    best_permutation = new int [nodes+1];

    for(int i = 0; i < nodes; i++){
        neighbours[i] = new bool [nodes];
        weights[i] = new int [nodes];
        for(int j = 0; j < nodes; j++){
            neighbours[i][j] = false;
            weights[i][j] = 0;
        }
    }

    for(int i = 0; i < edges; i++){
        cin >> x >> y >> z;
        neighbours[x][y] = neighbours[y][x] = true;
        weights[x][y] = weights[y][x] = z;
    }

    run_tests();

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

    return 0;
}
