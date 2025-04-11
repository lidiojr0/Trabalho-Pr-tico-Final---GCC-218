#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <algorithm>
#include <iterator>

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
    vector<vector<pair<int, int>>> adjArestas; // lista de adjacência para arestas (não direcionadas)
    vector<vector<pair<int, int>>> adjArcos;   // lista de adjacência para arcos (direcionados)
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

    string nomeGrafo, linha;
    int valorOtimo = 0, numVeiculos = 0, capacidade = 0, deposito = 0, V = 0, E = 0, A = 0, ReqN = 0, ReqE = 0, ReqA = 0;
    Grafo g(V);

    // leitura do arquivo
    while (getline(arquivo, linha))
    {
        istringstream ss(linha);

        // Lê o nome do grafo
        if (linha.find("Name:") != string::npos)
        {
            ss.ignore(6); // Ignorar "Name: "
            ss >> nomeGrafo;
        }
        // Lê o valor ótimo
        else if (linha.find("Optimal value:") != string::npos)
        {
            ss.ignore(15); // Ignorar "Optimal value: "
            ss >> valorOtimo;
        }
        // Lê o número de veículos
        else if (linha.find("#Vehicles:") != string::npos)
        {
            ss.ignore(10); // Ignorar "#Vehicles: "
            ss >> numVeiculos;
        }
        // Lê a capacidade dos veículos
        else if (linha.find("Capacity:") != string::npos)
        {
            ss.ignore(9); // Ignorar "Capacity: "
            ss >> capacidade;
        }
        // Lê o nó do depósito
        else if (linha.find("Depot Node:") != string::npos)
        {
            ss.ignore(12); // Ignorar "Depot Node: "
            ss >> deposito;
        }
        else if (linha.find("#Nodes:") != string::npos)
        {
            ss.ignore(7); // Ignorar "#Nodes: "
            ss >> V;
            g = Grafo(V); // Após ler o número de vértices, recria o grafo
        }
        else if (linha.find("#Edges:") != string::npos)
        {
            ss.ignore(7); // Ignorar "#Edges: "
            ss >> E;      // Número de arestas
        }
        else if (linha.find("#Arcs:") != string::npos)
        {
            ss.ignore(6); // Ignorar "#Arcs: "
            ss >> A;      // Número de arcos
        }
        else if (linha.find("#Required N:") != string::npos)
        {
            ss.ignore(12); // Ignorar "#Required N: "
            ss >> ReqN;    // Número de nós requeridos
        }
        else if (linha.find("#Required E:") != string::npos)
        {
            ss.ignore(12); // Ignorar "#Required E: "
            ss >> ReqE;    // Número de arestas requeridas
        }
        else if (linha.find("#Required A:") != string::npos)
        {
            ss.ignore(12); // Ignorar "#Required A: "
            ss >> ReqA;    // Número de arcos requeridos
        }
        else if (linha.find("ReN.") != string::npos)
        {
            // Lê nós requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ss(linha);
                string id;
                int demanda, s_custo;
                ss >> id >> demanda >> s_custo;
                g.adicionarVertice(stoi(id.substr(1)), demanda, s_custo); // Pega o número do nó ignorando a letra
            }
        }
        else if (linha.find("ReE.") != string::npos)
        {
            // Lê arestas requeridas
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ss(linha);
                string id;
                int origem, destino, t_custo, demanda, s_custo;
                ss >> id >> origem >> destino >> t_custo >> demanda >> s_custo;
                g.adicionarAresta(origem, destino, t_custo, demanda, s_custo); // Adiciona uma nova aresta ao grafo
            }
        }
        else if (linha.find("ReA.") != string::npos)
        {
            // Lê arcos requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ss(linha);
                string id;
                int origem, destino, t_custo, demanda, s_custo;
                ss >> id >> origem >> destino >> t_custo >> demanda >> s_custo;
                g.adicionarArco(origem, destino, t_custo, demanda, s_custo); // Adiciona um novo arco ao grafo
            }
        }
        else if (linha.find("ARC") != string::npos)
        {
            // Lê arcos não-requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ss(linha);
                string id;
                int origem, destino, t_custo;
                ss >> id >> origem >> destino >> t_custo;
                g.adicionarArco(origem, destino, t_custo, 0, 0); // Demanda e s_custo zero para não-requeridos
            }
        }
    }

    arquivo.close();
    // Imprime as novas informações lidas
    cout << "Informações lidas do arquivo:\n";
    cout << "Nome do grafo: " << nomeGrafo << endl;
    cout << "Valor ótimo: " << valorOtimo << endl;
    cout << "Número de veículos: " << numVeiculos << endl;
    cout << "Capacidade: " << capacidade << endl;
    cout << "Nó do depósito: " << deposito << endl;
    cout << "Vértices: " << V << ", Arestas: " << E << ", Arcos: " << A << endl;
    cout << "Nós requeridos: " << ReqN << ", Arestas requeridas: " << ReqE << ", Arcos requeridos: " << ReqA << endl;

    return g;
}
