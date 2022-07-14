#include "Sources.h"

vector<vector<double>> init_pheromone(double rank) {

    vector<vector<double>> pheromone;
    for (size_t i = 0; i < rank; i++)
    {
        vector<double> p(rank);
        for (size_t j = 0; j < rank; j++)
        {
            double value = 1 / (rank * rank);
            p[j] = value;
        }
        pheromone.push_back(p);
    }
    return pheromone;
}

string convert_path_to_str(double best_cost, vector<int> best_solution, int size) {
    string data = to_string(best_cost).append("\n");
    for (size_t i = 0; i < size; i++)
    {
        data.append(to_string(best_solution[i]).append("|"));
    }
    return data;
}

string convertToString(char* a, int size)
{
    int i;
    string s = "";
    for (i = 0; i < size; i++) {
        s = s + a[i];
    }
    return s;
}

vector<string> get_init_data(string s) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(s);
    while (getline(ss, token, '\n')) {
        tokens.push_back(token);
    }
    return tokens;
}

vector<string> get_tokens(string s) {
    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(s);
    while (getline(ss, token, '|')) {
        tokens.push_back(token);
    }
    return tokens;
}

vector<vector<double>> parse_data_to_vector(string s, int size) {
    vector<vector<double>> d;

    vector<string> tokens = get_tokens(s);

    for (size_t i = 0; i < size; i++)
    {
        vector<double> temp;
        for (size_t j = 0; j < size; j++)
        {
            double value = stod(tokens[size * i + j]);
            temp.push_back(value);
        }
        d.push_back(temp);
    }
    return d;
}


int sc_main(int argc, char* argv[]) {

    srand((unsigned)time(0));

    double alpha;
    double beta;
    int rank;
    int ant_count;
    int generations;
    string c_data;
    vector<vector<double>> cost_matrix;
    vector<vector<double>> pheromone;

    WSADATA wsaData;
    int iResult;

    SOCKET ListenSocket = INVALID_SOCKET;
    SOCKET ClientSocket = INVALID_SOCKET;

    struct addrinfo* result = NULL;
    struct addrinfo hints;

    int iSendResult;
    char recvbuf[DEFAULT_BUFLEN];
    int recvbuflen = DEFAULT_BUFLEN;

    // Initialize Winsock
    iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);


    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;
    hints.ai_flags = AI_PASSIVE;

    // Resolve the server address and port
    iResult = getaddrinfo("127.0.0.1", DEFAULT_PORT, &hints, &result);

    // Create a SOCKET for connecting to server
    ListenSocket = socket(result->ai_family, result->ai_socktype, result->ai_protocol);

    // Setup the TCP listening socket
    iResult = bind(ListenSocket, result->ai_addr, (int)result->ai_addrlen);

    freeaddrinfo(result);

    iResult = listen(ListenSocket, SOMAXCONN);

    cout << "Listening for client\n";

    // Accept a client socket
    ClientSocket = accept(ListenSocket, NULL, NULL);

    cout << "Accepted A Client\n";

    // No longer need server socket
    closesocket(ListenSocket);

    iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
    printf("Bytes received: %d\n", iResult);
    string initial = convertToString(recvbuf, DEFAULT_BUFLEN);
    vector<string> init_data = get_init_data(initial);
    alpha = stod(init_data[0]);
    beta = stod(init_data[1]);
    rank = stoi(init_data[2]);
    ant_count = stoi(init_data[3]);
    generations = stoi(init_data[4]);
    cost_matrix = parse_data_to_vector(init_data[5], rank);

    memset(recvbuf, 0, DEFAULT_BUFLEN);
    iSendResult = send(ClientSocket, recvbuf, iResult, 0);

    double best_cost = INFINITY;
    vector<int> best_solution(rank);

    for (size_t k = 0; k < generations; k++)
    {
        iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
        if (k != 0) {
            string p_data = convertToString(recvbuf, DEFAULT_BUFLEN);
            pheromone = parse_data_to_vector(p_data, rank);
        }
        else {
            pheromone = init_pheromone(rank);
        }
        memset(recvbuf, 0, DEFAULT_BUFLEN);
        iSendResult = send(ClientSocket, recvbuf, iResult, 0);
        for (size_t i = 0; i < ant_count; i++)
        {
            Ant ant(alpha, beta, rank, pheromone, cost_matrix);
            for (size_t j = 0; j < static_cast<unsigned long long>(rank) - 1; j++)
            {
                ant.select_next();
            }
            ant.total_cost += cost_matrix[ant.path.at(ant.path.size() - 1)][ant.path.at(0)];
            if (ant.total_cost < best_cost) {
                best_cost = ant.total_cost;
                best_solution = ant.path;
            }
            iResult = recv(ClientSocket, recvbuf, recvbuflen, 0);
            string path = convert_path_to_str(best_cost, best_solution, rank);
            memset(recvbuf, 0, DEFAULT_BUFLEN);
            path.copy(recvbuf, DEFAULT_BUFLEN);
            iSendResult = send(ClientSocket, recvbuf, iResult, 0);
        }
    }

    iResult = shutdown(ClientSocket, SD_SEND);
    if (iResult == SOCKET_ERROR) {
        printf("shutdown failed with error: %d\n", WSAGetLastError());
        closesocket(ClientSocket);
        WSACleanup();
        return 1;
    }

    closesocket(ClientSocket);
    WSACleanup();

    return 0;
}















    sc_signal <bool> main_enable;
    sc_signal <bool> main_enable;
    sc_clock TestClk("TestClock", 10, SC_NS, 0.5, 1, SC_NS);

    driver d1("d1");
    d1.enable(main_enable);
    d1.Clk(TestClk);

    Data_Convertor dt("Car1");
    dt.enable(main_enable);

    monitor mo1("mo1");
    mo1.enable(main_enable);
    mo1.Clk(TestClk);


    sc_trace_file* Tf = sc_create_vcd_trace_file("CarSpeed");

    sc_trace(Tf, main_enable, "Enable");

    sc_start(40, SC_NS);

    sc_close_vcd_trace_file(Tf);