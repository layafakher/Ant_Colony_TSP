#include "systemc.h"
#include <iostream>
#include <stdio.h>
#include <iomanip>
#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <sstream>
#include <iterator>
#undef UNICODE

#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <stdlib.h>
#include <stdio.h>
#include <iostream>
using namespace std;

// Need to link with Ws2_32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")

#define DEFAULT_BUFLEN 32768
#define DEFAULT_PORT "27030"

class Ant {
public:
    double alpha;
    double beta;
    int size;
    int current;
    double total_cost;
    vector<int> path;
    vector<int> allowed;
    vector<vector<double>> cost_matrix;
    vector<vector<double>> eta;
    vector<vector<double>> pheromone;

    Ant(double a, double b, int s, vector<vector<double>> p_data, vector<vector<double>> c_data) {
        alpha = a;
        beta = b;
        size = s;
        cost_matrix = c_data;
        pheromone = p_data;

        total_cost = 0;
        for (size_t i = 0; i < size; i++)
        {
            allowed.push_back(i);
        }
        for (size_t i = 0; i < size; i++)
        {
            vector<double> e(size);
            for (size_t j = 0; j < size; j++)
            {
                if (i == j) {

                    e[j] = 0;
                }
                else {
                    e[j] = 1 / cost_matrix[i][j];
                }
            }
            eta.push_back(e);
        }

        int start = rand() % size;
        path.push_back(start);
        current = start;
        allowed.erase(remove(allowed.begin(), allowed.end(), start), allowed.end());
    }

    void select_next() {
        double denominator = 0;
        for (size_t i = 0; i < size; i++)
        {
            denominator += pow(pheromone[current][i], alpha) * pow(eta[current][i], beta);
        }
        vector<double> probabilities(size);
        for (size_t i = 0; i < size; i++)
        {
            probabilities[i] = 0;
        }
        for (size_t i = 0; i < size; i++) {
            if (find(allowed.begin(), allowed.end(), i) != allowed.end()) {
                probabilities[i] = pow(pheromone[current][i], alpha) * pow(eta[current][i], beta) / denominator;
            }
        }

        int selected = -1;
        double r = ((double)rand() / (RAND_MAX));
        for (size_t i = 0; i < size; i++)
        {
            r -= probabilities[i];
            if (r <= 0) {
                selected = i;
                break;
            }
        }
        if (selected == -1) {
            int index = rand() % allowed.size();
            selected = allowed[index];
        }
        allowed.erase(remove(allowed.begin(), allowed.end(), selected), allowed.end());
        path.push_back(selected);
        total_cost += cost_matrix[current][selected];
        current = selected;
    }

};

