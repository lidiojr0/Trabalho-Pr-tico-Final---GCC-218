#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <algorithm>

using namespace std;

// estruturas para representar os arcos, vértices e arestas
struct Aresta
{
    int origem, destino, custo, demanda, s_custo;
};

struct Arco
{
    int origem, destino, custo, demanda, s_custo;
};

struct Vertice
{
    int id, demanda, s_custo;
};

// classe representando um grafo
class Grafo
{
public:
    int V;
    vector<vector<pair<int, int>>> adjArestas; // lista de adjacência para arestas(não direcionadas)
    vector<vector<pair<int, int>>> adjArcos;   // lista de adjacência para arcos(direcionados)
    vector<Vertice> vertices;
    vector<Aresta> arestas;
    vector<Arco> arcos;

    // construtor que inicializa o grafo com V vértices
    Grafo(int V) : V(V)
    {
        adjArestas.resize(V + 1);
        adjArcos.resize(V + 1);
    }

    // função para adicionar vértices
    void adicionarVertice(int id, int demanda, int s_custo)
    {
        vertices.push_back({id, demanda, s_custo});
    }

    // função para adicionar arestas
    void adicionarAresta(int origem, int destino, int custo, int demanda, int s_custo)
    {
        arestas.push_back({origem, destino, custo, demanda, s_custo});
        adjArestas[origem].push_back({destino, custo});
        adjArestas[destino].push_back({origem, custo});
    }

    // função para adicionar arcos
    void adicionarArco(int origem, int destino, int custo, int demanda, int s_custo)
    {
        arcos.push_back({origem, destino, custo, demanda, s_custo});
        adjArcos[origem].push_back({destino, custo});
    }

    // função para realizar a impressão do grafo
    void imprimirGrafo()
    {
        cout << "Vertices:" << endl;
        for (const auto &v : vertices)
        {
            cout << "Vertice " << v.id << ", Demanda: " << v.demanda << ", S. Custo: " << v.s_custo << endl;
        }

        cout << "\nArestas:" << endl;
        for (const auto &a : arestas)
        {
            cout << "Aresta de " << a.origem << " para " << a.destino << " com custo " << a.custo
                 << ", Demanda: " << a.demanda << ", S. Custo: " << a.s_custo << endl;
        }

        cout << "\nArcos:" << endl;
        for (const auto &ar : arcos)
        {
            cout << "Arco de " << ar.origem << " para " << ar.destino << " com custo " << ar.custo
                 << ", Demanda: " << ar.demanda << ", S. Custo: " << ar.s_custo << endl;
        }
    }
};

// função para leitura do arquivo e criação do grafo
Grafo lerGrafoDeArquivo(const string &nomeArquivo)
{
    ifstream arquivo(nomeArquivo);
    if (!arquivo)
    {
        cerr << "Erro ao abrir o arquivo!" << endl;
        exit(1);
    }

    string linha;
    int V = 0, E = 0, A = 0, ReqN = 0, ReqE = 0, ReqA = 0;
    Grafo g(V);

    // leitura do arquivo
    while (getline(arquivo, linha))
    {
        istringstream ss(linha);

        if (linha.find("#Nodes:") != string::npos)
        {
            ss.ignore(7); // ignorar "#Nodes: "
            ss >> V;
            g = Grafo(V); // após ler o número de vertices, recria o grafo
        }
        else if (linha.find("#Edges:") != string::npos)
        {
            ss.ignore(7); // ignorar "#Edges: "
            ss >> E;      // número de arestas
        }
        else if (linha.find("#Arcs:") != string::npos)
        {
            ss.ignore(6); // ignorar "#Arcs: "
            ss >> A;      // número de arcos
        }
        else if (linha.find("#Required N:") != string::npos)
        {
            ss.ignore(12); // ignorar "#Required N: "
            ss >> ReqN;    // número de nós requeridos
        }
        else if (linha.find("#Required E:") != string::npos)
        {
            ss.ignore(12); // ignorar "#Required E: "
            ss >> ReqE;    // número de arestas requeridas
        }
        else if (linha.find("#Required A:") != string::npos)
        {
            ss.ignore(12); // ignorar "#Required A: "
            ss >> ReqA;    // número de arcos requeridos
        }
        else if (linha.find("ReN.") != string::npos)
        {
            // ler nós requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ss(linha);
                string id;
                int demanda, s_custo;
                ss >> id >> demanda >> s_custo;
                g.adicionarVertice(stoi(id.substr(1)), demanda, s_custo); // pega o número do nó ignorando a letra
            }
        }
        else if (linha.find("ReE.") != string::npos)
        {
            // ler arestas requeridas
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ss(linha);
                string id;
                int origem, destino, t_custo, demanda, s_custo;
                ss >> id >> origem >> destino >> t_custo >> demanda >> s_custo;
                g.adicionarAresta(origem, destino, t_custo, demanda, s_custo); // adiciona uma nova aresta ao grafo
            }
        }
        else if (linha.find("ReA.") != string::npos)
        {
            // ler arcos requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ss(linha);
                string id;
                int origem, destino, t_custo, demanda, s_custo;
                ss >> id >> origem >> destino >> t_custo >> demanda >> s_custo;
                g.adicionarArco(origem, destino, t_custo, demanda, s_custo); // adiciona um novo arco ao grafo
            }
        }
        else if (linha.find("ARC") != string::npos)
        {
            // ler arcos não-requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ss(linha);
                string id;
                int origem, destino, t_custo;
                ss >> id >> origem >> destino >> t_custo;
                g.adicionarArco(origem, destino, t_custo, 0, 0); // demanda e s_custo zero para não-requeridos
            }
        }
    }

    arquivo.close();
    cout << "Informacoes lidas do arquivo:\n";
    cout << "Vertices: " << V << ", Arestas: " << E << ", Arcos: " << A << endl;
    cout << "Nos requeridos: " << ReqN << ", Arestas requeridas: " << ReqE << ", Arcos requeridos: " << ReqA << endl;

    return g;
}

// função para calcular o número de vértices requeridos
int calcularVerticesRequeridos(const Grafo &g)
{
    int count = 0;
    for (const auto &v : g.vertices)
    {
        if (v.demanda > 0)
        {
            count++;
        }
    }
    return count;
}

// função para calcular o número de arestas requeridas
int calcularArestasRequeridas(const Grafo &g)
{
    int count = 0;
    for (const auto &a : g.arestas)
    {
        if (a.demanda > 0)
        {
            count++;
        }
    }
    return count;
}

// função para calcular o número de arcos requeridos
int calcularArcosRequeridos(const Grafo &g)
{
    int count = 0;
    for (const auto &a : g.arcos)
    {
        if (a.demanda > 0)
        {
            count++;
        }
    }
    return count;
}

// função para calcular a densidade do grafo
double calcularDensidade(const Grafo &g)
{
    double numArestas = g.arestas.size();
    double numVertices = g.V;
    return (2 * numArestas) / (numVertices * (numVertices - 1));
}

// função para calcular o grau de um vértice
int calcularGrau(const Grafo &g, int v)
{
    return g.adjArestas[v].size();
}

// função para calcular o grau mínimo dos vértices
int calcularGrauMinimo(const Grafo &g)
{
    int grauMin = INT_MAX;
    for (int i = 1; i <= g.V; i++)
    {
        int grau = calcularGrau(g, i);
        grauMin = min(grauMin, grau);
    }
    return grauMin;
}

// função para calcular o grau máximo dos vértices
int calcularGrauMaximo(const Grafo &g)
{
    int grauMax = INT_MIN;
    for (int i = 1; i <= g.V; i++)
    {
        int grau = calcularGrau(g, i);
        grauMax = max(grauMax, grau);
    }
    return grauMax;
}

// função para calcular a matriz de distâncias de Floyd-Warshall
vector<vector<int>> floydWarshall(const Grafo &g)
{
    int n = g.V;
    vector<vector<int>> dist(n + 1, vector<int>(n + 1, INT_MAX));

    // inicialização da matriz de distâncias
    for (int i = 1; i <= n; i++)
    {
        dist[i][i] = 0;
    }

    for (const auto &a : g.arestas)
    {
        if (dist[a.origem][a.destino] > a.custo)
            dist[a.origem][a.destino] = a.custo;

        if (dist[a.destino][a.origem] > a.custo)
            dist[a.destino][a.origem] = a.custo;
    }

    // algoritmo de Floyd-Warshall
    for (int k = 1; k <= n; k++)
    {
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                if (dist[i][k] != INT_MAX && dist[k][j] != INT_MAX)
                {
                    dist[i][j] = min(dist[i][j], dist[i][k] + dist[k][j]);
                }
            }
        }
    }
    return dist;
}

// função para calcular o diâmetro do grafo
int calcularDiametro(const Grafo &g, const vector<vector<int>> &dist)
{
    int diametro = 0;
    for (int i = 1; i <= g.V; i++)
    {
        for (int j = 1; j <= g.V; j++)
        {
            if (dist[i][j] != INT_MAX)
            {
                diametro = max(diametro, dist[i][j]);
            }
        }
    }
    return diametro;
}

// função para calcular a intermediação
vector<double> calcularIntermediacao(const Grafo &g, const vector<vector<int>> &dist)
{
    int n = g.V;
    vector<double> intermediacao(n + 1, 0.0);

    for (int s = 1; s <= n; ++s)
    {
        for (int t = 1; t <= n; ++t)
        {
            if (s != t && dist[s][t] != INT_MAX)
            {
                for (int v = 1; v <= n; ++v)
                {
                    if (v != s && v != t && dist[s][v] != INT_MAX && dist[v][t] != INT_MAX &&
                        dist[s][t] == dist[s][v] + dist[v][t])
                    {
                        intermediacao[v]++;
                    }
                }
            }
        }
    }

    return intermediacao;
}

// função para calcular o caminho médio
double calcularCaminhoMedio(const Grafo &g, const vector<vector<int>> &dist)
{
    int n = g.V;
    double soma = 0.0;
    int total = 0;

    for (int i = 1; i <= n; ++i)
    {
        for (int j = i + 1; j <= n; ++j)
        {
            if (dist[i][j] != INT_MAX)
            {
                soma += dist[i][j];
                total++;
            }
        }
    }

    return (total > 0) ? soma / total : 0.0;
}

int main()
{
    string nomeArquivo = "mgval_0.50_9B.dat"; // nome do arquivo de entrada com os dados do grafo
    Grafo g = lerGrafoDeArquivo(nomeArquivo);

    cout << "Grafo lido do arquivo:\n";
    g.imprimirGrafo();

    cout << "\nMetricas do grafo: " << endl;
    cout << "\nQuantidade de vertices requeridos: " << calcularVerticesRequeridos(g) << endl;
    cout << "Quantidade de arestas requeridas: " << calcularArestasRequeridas(g) << endl;
    cout << "Quantidade de arcos requeridos: " << calcularArcosRequeridos(g) << endl;
    cout << "Densidade do grafo: " << calcularDensidade(g) << endl;
    cout << "Grau minimo dos vertices: " << calcularGrauMinimo(g) << endl;
    cout << "Grau maximo dos vertices: " << calcularGrauMaximo(g) << endl;

    vector<vector<int>> dist = floydWarshall(g);
    cout << "Diametro do grafo: " << calcularDiametro(g, dist) << endl;

    // calcula a intermediação dos vértices
    vector<double> intermediacao = calcularIntermediacao(g, dist);
    cout << "Intermediacao dos vertices:\n";
    for (int i = 1; i <= g.V; i++)
    {
        cout << "Vertice " << i << ": " << intermediacao[i] << endl;
    }

    // caminho médio do grafo
    double caminhoMedio = calcularCaminhoMedio(g, dist);
    cout << "Caminho medio do grafo: " << caminhoMedio << endl;

    return 0;
}
