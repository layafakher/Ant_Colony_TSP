import socket

PORT = 27030
SIZE = 32768
FORMAT ='utf-8'
CITY_COUNT = 31

class Graph(object):
    def __init__(self, cost_matrix: list, rank: int):
        self.matrix = cost_matrix
        self.rank = rank
        # noinspection PyUnusedLocal
        self.pheromone = [[1 / (rank * rank) for j in range(rank)] for i in range(rank)]

def connect_to_server():
    s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
    s.connect(('localhost', PORT))
    return s

def get_best_path_from_server(s):
    data = s.recv(SIZE).decode(FORMAT)
    total_cost = data.split('\n')[0]
    best_path = data.split('\n')[1].split('|')
    if len(best_path) > CITY_COUNT:
        best_path = best_path[:CITY_COUNT]
    return best_path,total_cost

def matrix_to_string(mat):
    data = []
    for i in range(len(mat)):
        for j in range(len(mat[i])):
            s = "{:.6f}".format(mat[i][j])
            data.append(s)
    return '|'.join(data)

class ACO(object):
    def __init__(self, ant_count: int, generations: int, alpha: float, beta: float, rho: float, q: int,
                 strategy: int):
        self.Q = q
        self.rho = rho
        self.beta = beta
        self.alpha = alpha
        self.ant_count = ant_count
        self.generations = generations
        self.update_strategy = strategy

    def solve(self, graph: Graph):
        data_full = ""
        for i in range(int(SIZE/4)):
            data_full += "0"
        best_cost = float('inf')
        best_solution = []
        s = connect_to_server()
        data = str(self.alpha) + '\n' + str(self.beta) + '\n' + str(graph.rank) + \
            '\n' + str(self.ant_count) + '\n' + str(self.generations) + '\n' + matrix_to_string(graph.matrix) 
        s.send(data.encode(FORMAT))
        s.recv(SIZE).decode(FORMAT)
        for gen in range(self.generations):
            data = matrix_to_string(graph.pheromone)
            s.send(data.encode(FORMAT))
            s.recv(SIZE).decode(FORMAT)
            p = []
            for i in range(self.ant_count):
                s.send(data_full.encode(FORMAT))
                best_path, total_cost = get_best_path_from_server(s)
                best_solution = []
                for x in best_path:
                    best_solution.append(int(x))
                best_cost = total_cost
                p_data = update_pheromone_temp(best_path,total_cost,self.update_strategy,self.Q,graph)
                p.append(p_data)
            update_pheromone(graph,p,self.rho)

        return best_solution, best_cost

def update_pheromone_temp(best_path,total_cost,update_strategy,Q,graph):
    pheromone_delta = [[0 for j in range(graph.rank)] for i in range(graph.rank)]
    for _ in range(1, len(best_path)):
        i = int(best_path[_ - 1])
        j = int(best_path[_])
        if update_strategy == 1:
            pheromone_delta[i][j] = Q
        elif update_strategy == 2:
            if graph.matrix[i][j] != 0.0:
                pheromone_delta[i][j] = Q / graph.matrix[i][j]
            else:
                pheromone_delta[i][j] = Q / 0.0001
        else:
            pheromone_delta[i][j] = Q / total_cost
    return pheromone_delta

def update_pheromone(graph: Graph, pheromone_list: list, rho):
    for i, row in enumerate(graph.pheromone):
        for j, col in enumerate(row):
            graph.pheromone[i][j] *= rho
            for pheromone in pheromone_list:
                graph.pheromone[i][j] += pheromone[i][j]
