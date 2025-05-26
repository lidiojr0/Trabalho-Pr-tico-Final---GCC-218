#ifndef FUNCOES_HPP
#define FUNCOES_HPP

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>

#include "grafo.hpp" // Contém as definições de Grafo e seus componentes

using namespace std;

// Define um valor grande para representar "infinito" em distâncias do tipo long long.
const long long LONGLONG_INF = numeric_limits<long long>::max();

// Calcula o número de vértices que possuem demanda positiva (serviços em nós).
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

// Calcula o número de arestas que possuem demanda positiva (serviços em arestas).
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

// Calcula o número de arcos que possuem demanda positiva (serviços em arcos).
int calcularArcosRequeridos(const Grafo &g)
{
    int count = 0;
    for (const auto &ar : g.arcos)
    {
        if (ar.demanda > 0)
        {
            count++;
        }
    }
    return count;
}

// Calcula a densidade do grafo, considerando apenas as arestas (não direcionadas).
double calcularDensidade(const Grafo &g)
{
    if (g.V < 2)
    { // Densidade não definida ou 0 para grafos com menos de 2 nós
        return 0.0;
    }
    double numArestasConsideradas = static_cast<double>(g.arestas.size());
    double numVertices = static_cast<double>(g.V);
    return (2.0 * numArestasConsideradas) / (numVertices * (numVertices - 1.0));
}

// Calcula o grau de um vértice específico (número de arestas incidentes).
// Utiliza a lista de adjacência de arestas (g.adjArestas).
int calcularGrau(const Grafo &g, int v_idx)
{
    // Validação do índice do vértice
    if (v_idx <= 0 || v_idx > g.V || static_cast<size_t>(v_idx) >= g.adjArestas.size())
    {
        return 0;
    }
    return g.adjArestas[v_idx].size();
}

// Calcula o grau mínimo entre todos os vértices do grafo.
int calcularGrauMinimo(const Grafo &g)
{
    if (g.V == 0)
        return 0;
    int grauMin = numeric_limits<int>::max();
    bool algumGrauValido = false;
    for (int i = 1; i <= g.V; i++)
    {
        // Garante que o índice do vértice é válido para a lista de adjacência
        if (static_cast<size_t>(i) < g.adjArestas.size())
        {
            grauMin = min(grauMin, calcularGrau(g, i));
            algumGrauValido = true;
        }
    }
    return algumGrauValido ? grauMin : 0; // Retorna 0 se nenhum grau válido foi calculado
}

// Calcula o grau máximo entre todos os vértices do grafo.
int calcularGrauMaximo(const Grafo &g)
{
    if (g.V == 0)
        return 0;
    int grauMax = 0;
    for (int i = 1; i <= g.V; i++)
    {
        if (static_cast<size_t>(i) < g.adjArestas.size())
        {
            grauMax = max(grauMax, calcularGrau(g, i));
        }
    }
    return grauMax;
}

// Implementa o algoritmo de Floyd-Warshall para encontrar os caminhos mínimos entre todos os pares de nós.
// Considera um grafo misto (com arestas e arcos).
// Retorna uma matriz de distâncias onde dist[i][j] é o custo do caminho mínimo de i a j.
vector<vector<long long>> floydWarshall(const Grafo &g)
{
    int n = g.V;
    vector<vector<long long>> dist(n + 1, vector<long long>(n + 1, LONGLONG_INF));

    // Distância de um nó para ele mesmo é 0
    for (int i = 1; i <= n; i++)
    {
        dist[i][i] = 0;
    }

    // Inicializa distâncias com base nos custos diretos das ARESTAS
    for (const auto &aresta : g.arestas)
    {
        if (aresta.origem > 0 && aresta.origem <= n && aresta.destino > 0 && aresta.destino <= n)
        {
            // Considera o menor custo se houver múltiplas arestas entre os mesmos nós
            dist[aresta.origem][aresta.destino] = min(dist[aresta.origem][aresta.destino], static_cast<long long>(aresta.custo));
            dist[aresta.destino][aresta.origem] = min(dist[aresta.destino][aresta.origem], static_cast<long long>(aresta.custo)); // Bidirecional
        }
    }

    // Inicializa distâncias com base nos custos diretos dos ARCOS
    for (const auto &arco : g.arcos)
    {
        if (arco.origem > 0 && arco.origem <= n && arco.destino > 0 && arco.destino <= n)
        {
            dist[arco.origem][arco.destino] = min(dist[arco.origem][arco.destino], static_cast<long long>(arco.custo)); // Direcional
        }
    }

    // Etapa principal do algoritmo de Floyd-Warshall (iterações sobre nós intermediários k)
    for (int k = 1; k <= n; k++)
    {
        for (int i = 1; i <= n; i++)
        {
            for (int j = 1; j <= n; j++)
            {
                // Se caminhos i->k e k->j existem
                if (dist[i][k] != LONGLONG_INF && dist[k][j] != LONGLONG_INF)
                {
                    // Se o caminho via k é mais curto
                    if (dist[i][j] > dist[i][k] + dist[k][j])
                    {
                        dist[i][j] = dist[i][k] + dist[k][j];
                    }
                }
            }
        }
    }
    return dist;
}

// Calcula o diâmetro do grafo: o maior dos menores caminhos entre quaisquer dois nós distintos.
// Retorna LONGLONG_INF se o grafo não for fortemente conectado (algum par não alcançável).
long long calcularDiametro(const Grafo &g, const vector<vector<long long>> &dist)
{
    if (g.V < 2)
        return 0; // Diâmetro é 0 para grafos com 0 ou 1 nó

    long long max_dist_finita = 0;
    bool algum_caminho_infinito = false;

    for (int i = 1; i <= g.V; i++)
    {
        for (int j = 1; j <= g.V; j++)
        {
            if (i == j)
                continue; // Considera apenas pares distintos

            if (dist[i][j] == LONGLONG_INF)
            {
                algum_caminho_infinito = true; // Indica que o grafo não é fortemente conectado
            }
            else
            {
                if (dist[i][j] > max_dist_finita)
                {
                    max_dist_finita = dist[i][j];
                }
            }
        }
    }
    // Se existe algum par de nós (distintos) que não se conectam, o diâmetro é infinito.
    return algum_caminho_infinito ? LONGLONG_INF : max_dist_finita;
}

// Calcula uma métrica de "intermediação média":
// (Soma de nós intermediários em caminhos mais curtos s-v-t) / (Número de pares (s,t) com caminho finito).
float calcularIntermediacao(const Grafo &g, const vector<vector<long long>> &dist)
{
    if (g.V < 3)
        return 0.0f; // Intermediação não é significativa para menos de 3 nós
    int n = g.V;
    double soma_nos_intermediarios_em_caminhos_st = 0.0;
    int num_pares_st_com_caminho_finito = 0;

    for (int s = 1; s <= n; ++s)
    {
        for (int t = 1; t <= n; ++t)
        {
            if (s == t)
                continue; // Pares (s,t) distintos
            if (dist[s][t] != LONGLONG_INF)
            { // Se existe caminho finito entre s e t
                num_pares_st_com_caminho_finito++;
                // Verifica cada nó v_intermed como possível intermediário
                for (int v_intermed = 1; v_intermed <= n; ++v_intermed)
                {
                    if (v_intermed == s || v_intermed == t)
                        continue; // Intermediário deve ser diferente de s e t
                    // Se v_intermed está em um caminho mais curto entre s e t
                    if (dist[s][v_intermed] != LONGLONG_INF && dist[v_intermed][t] != LONGLONG_INF &&
                        dist[s][t] == (dist[s][v_intermed] + dist[v_intermed][t]))
                    {
                        soma_nos_intermediarios_em_caminhos_st += 1.0;
                    }
                }
            }
        }
    }
    if (num_pares_st_com_caminho_finito == 0)
        return 0.0f; // Evita divisão por zero
    return static_cast<float>(soma_nos_intermediarios_em_caminhos_st / num_pares_st_com_caminho_finito);
}

// Calcula o caminho médio do grafo: a média das distâncias mínimas
// entre todos os pares de nós distintos que são alcançáveis.
double calcularCaminhoMedio(const Grafo &g, const vector<vector<long long>> &dist)
{
    if (g.V < 2)
        return 0.0; // Não definido para menos de 2 nós
    int n = g.V;
    double soma_distancias_finitas = 0.0;
    int num_pares_alcancaveis = 0; // Conta pares distintos (i,j) com i < j

    for (int i = 1; i <= n; ++i)
    {
        for (int j = i + 1; j <= n; ++j)
        { // Considera cada par (i,j) uma vez
            if (dist[i][j] != LONGLONG_INF)
            {
                soma_distancias_finitas += static_cast<double>(dist[i][j]);
                num_pares_alcancaveis++;
            }
        }
    }
    if (num_pares_alcancaveis == 0)
        return 0.0; // Evita divisão por zero
    return soma_distancias_finitas / num_pares_alcancaveis;
}

// Imprime a estrutura do grafo (listas de vértices, arestas e arcos) em um arquivo de saída.
// Formato CSV para fácil visualização ou importação.
void imprimirGrafo(ofstream &outputFile, const Grafo &g)
{
    outputFile << "\n\nVertices (id, demanda, s.custo):\n";
    outputFile << "id,demanda,s.custo\n";
    for (const auto &v : g.vertices)
    {
        outputFile << v.id << "," << v.demanda << "," << v.s_custo << "\n";
    }
    outputFile << "\nArestas (Tipo, Origem, Destino, Custo, Demanda, S.Custo):\n";
    outputFile << "Tipo,Origem,Destino,Custo,Demanda,S.Custo\n";
    for (const auto &a : g.arestas)
    {
        outputFile << "Aresta," << a.origem << "," << a.destino << "," << a.custo
                   << "," << a.demanda << "," << a.s_custo << "\n";
    }
    outputFile << "\nArcos (Tipo, Origem, Destino, Custo, Demanda, S.Custo):\n";
    outputFile << "Tipo,Origem,Destino,Custo,Demanda,S.Custo\n";
    for (const auto &ar : g.arcos)
    {
        outputFile << "Arco," << ar.origem << "," << ar.destino << "," << ar.custo
                   << "," << ar.demanda << "," << ar.s_custo << "\n";
    }
}

// Calcula um conjunto de métricas do grafo e as imprime em uma linha de um arquivo CSV.
// Usada principalmente para a Etapa 1 do projeto.
void imprimirMetricas(ofstream &outputFileCSV, const string &nomeGrafoNoArquivo, const Grafo &g)
{
    // Cálcula métricas básicas
    int verticesReq = calcularVerticesRequeridos(g);
    int arestasReq = calcularArestasRequeridas(g);
    int arcosReq = calcularArcosRequeridos(g);
    double dens = calcularDensidade(g);
    int grauMin = calcularGrauMinimo(g);
    int grauMax = calcularGrauMaximo(g);

    // Calcula matriz de distâncias para métricas mais complexas
    vector<vector<long long>> matDist = floydWarshall(g);
    long long diam = calcularDiametro(g, matDist);
    float intermediacao = calcularIntermediacao(g, matDist);
    double camMedio = calcularCaminhoMedio(g, matDist);

    // Imprime a linha no arquivo CSV
    outputFileCSV << nomeGrafoNoArquivo << ","
                  << verticesReq << ","
                  << arestasReq << ","
                  << arcosReq << ","
                  << dens << ","
                  << grauMin << ","
                  << grauMax << ","
                  << (diam == LONGLONG_INF ? "INF" : std::to_string(diam)) << "," // Converte diâmetro para string, tratando INF
                  << camMedio << ","
                  << intermediacao
                  << "\n";
}

#endif // FUNCOES_HPP
