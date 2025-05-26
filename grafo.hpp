#ifndef GRAFO_HPP
#define GRAFO_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>

using namespace std;

// Define a estrutura de uma Aresta (não direcionada) no grafo.
struct Aresta
{
    int origem, destino; // Nós conectados pela aresta
    int custo;           // Custo de travessia da aresta
    int demanda;         // Demanda a ser atendida se a aresta for um serviço
    int s_custo;         // Custo associado ao serviço da aresta
};

// Define a estrutura de um Arco (direcionado) no grafo.
struct Arco
{
    int origem, destino; // Nós de origem e destino do arco
    int custo;           // Custo de travessia do arco
    int demanda;         // Demanda a ser atendida se o arco for um serviço
    int s_custo;         // Custo associado ao serviço do arco
};

// Define a estrutura de um Vértice no grafo.
struct Vertice
{
    int id;      // Identificador único do vértice
    int demanda; // Demanda a ser atendida se o vértice for um serviço
    int s_custo; // Custo associado ao serviço do vértice
};

// Classe que representa o grafo e suas operações básicas.
class Grafo
{
public:
    int V;                                     // Número total de vértices no grafo
    vector<vector<pair<int, int>>> adjArestas; // Lista de adjacência para arestas
    vector<vector<pair<int, int>>> adjArcos;   // Lista de adjacência para arcos

    vector<Vertice> vertices; // Lista de vértices (geralmente os nós com serviços)
    vector<Aresta> arestas;   // Lista de todas as arestas do grafo
    vector<Arco> arcos;       // Lista de todos os arcos do grafo

    // Construtor: inicializa um grafo com um número opcional de vértices.
    Grafo(int num_vertices = 0) : V(num_vertices)
    {
        if (V > 0)
        {
            adjArestas.resize(V + 1); // Usa indexação baseada em 1
            adjArcos.resize(V + 1);
        }
    }

    // (Re)inicializa o grafo com um novo número de vértices, limpando dados antigos.
    void inicializarGrafoInterno(int num_vertices)
    {
        V = num_vertices;
        adjArestas.assign(V + 1, vector<pair<int, int>>());
        adjArcos.assign(V + 1, vector<pair<int, int>>());
        vertices.clear();
        arestas.clear();
        arcos.clear();
    }

    // Adiciona um vértice à lista de vértices do grafo.
    void adicionarVertice(int id_no, int demanda_no, int s_custo_no)
    {
        vertices.push_back({id_no, demanda_no, s_custo_no});
    }

    // Adiciona uma aresta ao grafo e atualiza as listas de adjacência (bidirecional).
    void adicionarAresta(int origem, int destino, int custo, int demanda, int s_custo)
    {
        arestas.push_back({origem, destino, custo, demanda, s_custo});
        // Adiciona às listas de adjacência se os nós estiverem dentro dos limites de V
        if (origem > 0 && origem <= V && destino > 0 && destino <= V)
        {
            adjArestas[origem].push_back({destino, custo});
            adjArestas[destino].push_back({origem, custo});
        }
    }

    // Adiciona um arco ao grafo e atualiza a lista de adjacência (direcional).
    void adicionarArco(int origem, int destino, int custo, int demanda, int s_custo)
    {
        arcos.push_back({origem, destino, custo, demanda, s_custo});
        // Adiciona à lista de adjacência se os nós estiverem dentro dos limites de V
        if (origem > 0 && origem <= V && destino > 0 && destino <= V)
        {
            adjArcos[origem].push_back({destino, custo});
        }
    }
};

// Estrutura unificada para representar um serviço requerido (nó, aresta ou arco).
struct Servico
{
    int idGlobal; // ID único sequencial para o serviço
    enum class Tipo
    {
        NO,
        ARESTA,
        ARCO
    }; // Tipo do serviço
    Tipo tipo;
    int idNoOriginal;
    int uOriginal, vOriginal; // Nós da aresta/arco original (se tipo ARESTA/ARCO)
    int demanda;
    int custoServicoProprio;
    int custoTravessia; // Custo t_custo para atravessar a aresta/arco (se aplicável)
    bool atendido;
    int p1Saida, p2Saida; // Nós de saída para formatação da solução (S id, p1, p2)
};

// Agrupa todos os dados relevantes lidos de um arquivo de instância.
struct DadosInstancia
{
    Grafo g;
    int idNoDeposito;
    int capacidadeVeiculo;
    string nomeInstancia;
};

// Extrai o nome base da instância a partir do caminho completo do arquivo.
// Ex: "C:/path/to/BHW1.dat" -> "BHW1"
string extrairNomeBaseInstancia(const string &caminhoArquivo)
{
    string nomeComExtensao = caminhoArquivo;
    size_t ultimaBarra = nomeComExtensao.find_last_of("/\\");
    if (ultimaBarra != string::npos)
    {
        nomeComExtensao = nomeComExtensao.substr(ultimaBarra + 1);
    }
    size_t ultimoPonto = nomeComExtensao.rfind('.');
    if (ultimoPonto != string::npos)
    {
        return nomeComExtensao.substr(0, ultimoPonto);
    }
    return nomeComExtensao;
}

// Lê um arquivo de instância no formato .dat e retorna uma estrutura DadosInstancia.
DadosInstancia lerGrafoDeArquivo(const string &nomeArquivo)
{
    DadosInstancia dadosRetorno;
    dadosRetorno.idNoDeposito = -1;
    dadosRetorno.capacidadeVeiculo = 0;
    dadosRetorno.nomeInstancia = extrairNomeBaseInstancia(nomeArquivo);

    ifstream arquivo(nomeArquivo);
    if (!arquivo.is_open())
    {
        cerr << "Erro ao abrir o arquivo: " << nomeArquivo << endl;
        return dadosRetorno;
    }

    string linha;
    int V_lido = 0;

    // Loop para ler cada linha do arquivo de instância
    while (getline(arquivo, linha))
    {
        istringstream ssLinha(linha);
        string chavePrimaria;
        ssLinha >> chavePrimaria; // Identifica o tipo de informação na linha

        // Processa o cabeçalho da instância
        if (chavePrimaria == "Name:")
        {
            string tempNomeGrafo;
            ssLinha >> tempNomeGrafo; // Lê o nome interno do grafo (opcional)
        }
        else if (chavePrimaria == "Capacity:")
        {
            ssLinha >> dadosRetorno.capacidadeVeiculo;
        }
        else if (chavePrimaria == "Depot" && linha.find("Depot Node:") != string::npos)
        {
            string temp;
            if (chavePrimaria == "Depot")
                ssLinha >> temp; // Consome "Node:"
            ssLinha >> dadosRetorno.idNoDeposito;
        }
        else if (chavePrimaria == "#Nodes:")
        {
            ssLinha >> V_lido;
            dadosRetorno.g.inicializarGrafoInterno(V_lido); // Inicializa o grafo com o número de nós lido
        }
        // Consome outras chaves de cabeçalho comuns para robustez, mesmo que não usadas diretamente
        else if (chavePrimaria == "Optimal" && linha.find("Optimal value:") != string::npos)
        { /* Consumir valor */
        }
        else if (chavePrimaria == "#Vehicles:")
        { /* Consumir valor */
        }
        else if (chavePrimaria == "#Edges:")
        { /* Consumir valor */
        }
        else if (chavePrimaria == "#Arcs:")
        { /* Consumir valor */
        }
        else if (chavePrimaria == "#Required" && (linha.find("#Required N:") != string::npos || linha.find("#Required E:") != string::npos || linha.find("#Required A:") != string::npos))
        { /* Consumir valor */
        }

        // Processa seções de dados de serviços requeridos
        else if (chavePrimaria == "ReN.")
        { // Nós requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ssDados(linha);
                string idOriginalNoStr;
                int idNoReal, demandaNo, sCustoNo;
                ssDados >> idOriginalNoStr >> demandaNo >> sCustoNo;
                if (ssDados.fail() || idOriginalNoStr.empty() || idOriginalNoStr[0] != 'N')
                    break; // Fim da seção ou erro
                try
                {
                    idNoReal = std::stoi(idOriginalNoStr.substr(1)); // Converte "NX" para X
                    dadosRetorno.g.adicionarVertice(idNoReal, demandaNo, sCustoNo);
                }
                catch (const std::exception &e)
                {
                    cerr << "Erro parse ReN: " << linha << " (" << e.what() << ")" << endl;
                }
            }
        }
        else if (chavePrimaria == "ReE.")
        { // Arestas requeridas
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ssDados(linha);
                string idArestaOriginalStr;
                int origem, destino, tCusto, demandaAresta, sCustoAresta;
                ssDados >> idArestaOriginalStr >> origem >> destino >> tCusto >> demandaAresta >> sCustoAresta;
                if (ssDados.fail() || idArestaOriginalStr.empty() || idArestaOriginalStr[0] != 'E')
                    break;
                dadosRetorno.g.adicionarAresta(origem, destino, tCusto, demandaAresta, sCustoAresta);
            }
        }
        else if (chavePrimaria == "ReA.")
        { // Arcos requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ssDados(linha);
                string idArcoOriginalStr;
                int origem, destino, tCusto, demandaArco, sCustoArco;
                ssDados >> idArcoOriginalStr >> origem >> destino >> tCusto >> demandaArco >> sCustoArco;
                if (ssDados.fail() || idArcoOriginalStr.empty() || idArcoOriginalStr[0] != 'A')
                    break;
                dadosRetorno.g.adicionarArco(origem, destino, tCusto, demandaArco, sCustoArco);
            }
        }
        else if (chavePrimaria == "ARCS" || (chavePrimaria == "ARC" && linha.length() == 3))
        { // Arcos não requeridos
            while (getline(arquivo, linha) && !linha.empty())
            {
                istringstream ssDados(linha);
                string idArcoNaoReqStr;
                int origem, destino, tCusto;
                ssDados >> idArcoNaoReqStr >> origem >> destino >> tCusto;
                if (ssDados.fail() || idArcoNaoReqStr.empty())
                    break;
                dadosRetorno.g.adicionarArco(origem, destino, tCusto, 0, 0); // Demanda e s_custo são 0
            }
        }
    }
    arquivo.close();
    return dadosRetorno;
}

#endif // GRAFO_HPP
