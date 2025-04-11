#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <algorithm>
#include <iterator>

#include "grafo.hpp"

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
float calcularIntermediacao(const Grafo &g, const vector<vector<int>> &dist)
{
    int n = g.V;
    int totalCaminhos = 0;
    double somaIntermediacao = 0.0;

    // soma a intermediação para cada vértice
    for (int s = 1; s <= n; ++s)
    {
        for (int t = 1; t <= n; ++t)
        {
            if (s != t && dist[s][t] != INT_MAX)
            {
                // para cada caminho mínimo, conta quantas vezes um vértice está no caminho
                for (int v = 1; v <= n; ++v)
                {
                    if (v != s && v != t && dist[s][v] != INT_MAX && dist[v][t] != INT_MAX &&
                        dist[s][t] == dist[s][v] + dist[v][t])
                    {
                        somaIntermediacao++;
                    }
                }
                totalCaminhos++;
            }
        }
    }

    // se há pelo menos um caminho, calcula a média da intermediação
    if (totalCaminhos > 0)
    {
        return somaIntermediacao / totalCaminhos; // retorna a média das intermediações
    }

    return 0.0; // se não houver caminhos, retorna 0
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

// função para imprimir o grafo no arquivo
void imprimirGrafo(ofstream &outputFile, const Grafo &g)
{
    outputFile << "\n\nVertices (id, demanda, s.custo):\n";
    outputFile << "id,demanda,s.custo\n"; // cabeçalho para os vértices
    for (const auto &v : g.vertices)
    {
        outputFile << v.id << "," << v.demanda << "," << v.s_custo << "\n";
    }

    outputFile << "\nArestas (Tipo, Origem, Destino, Custo, Demanda, S.Custo):\n";
    outputFile << "Tipo,Origem,Destino,Custo,Demanda,S.Custo\n"; // cabeçalho para as arestas
    for (const auto &a : g.arestas)
    {
        outputFile << "Aresta," << a.origem << "," << a.destino << "," << a.custo
                   << "," << a.demanda << "," << a.s_custo << "\n";
    }

    outputFile << "\nArcos (Tipo, Origem, Destino, Custo, Demanda, S.Custo):\n";
    outputFile << "Tipo,Origem,Destino,Custo,Demanda,S.Custo\n"; // cabeçalho para os arcos
    for (const auto &ar : g.arcos)
    {
        outputFile << "Arco," << ar.origem << "," << ar.destino << "," << ar.custo
                   << "," << ar.demanda << "," << ar.s_custo << "\n";
    }
}

void imprimirMetricas(ofstream &outputFile, const string &nomeArquivo)
{
    Grafo g = lerGrafoDeArquivo(nomeArquivo);

    // calcula as métricas
    int verticesRequeridos = calcularVerticesRequeridos(g);
    int arestasRequeridas = calcularArestasRequeridas(g);
    int arcosRequeridos = calcularArcosRequeridos(g);
    double densidade = calcularDensidade(g);
    int grauMinimo = calcularGrauMinimo(g);
    int grauMaximo = calcularGrauMaximo(g);

    // calcula a matriz de distâncias e métricas relacionadas
    vector<vector<int>> dist = floydWarshall(g);
    int diametro = calcularDiametro(g, dist);
    float mediaIntermediacao = calcularIntermediacao(g, dist);
    double caminhoMedio = calcularCaminhoMedio(g, dist);

    // imprime os resultados no arquivo CSV
    outputFile << nomeArquivo << ","
               << verticesRequeridos << ","
               << arestasRequeridas << ","
               << arcosRequeridos << ","
               << densidade << ","
               << grauMinimo << ","
               << grauMaximo << ","
               << diametro << ","
               << caminhoMedio << ","
               << mediaIntermediacao
               << "\n";
}
