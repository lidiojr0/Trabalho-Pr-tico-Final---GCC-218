#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <climits>
#include <algorithm>
#include <iterator>

#include "funcoes.hpp"

int main()
{
    // lista de arquivos .dat que precisam ser lidos
    vector<string> arquivos = {
        "BHW1.dat", "BHW2.dat", "BHW3.dat", "BHW4.dat", "BHW5.dat", "BHW6.dat", "BHW7.dat", "BHW8.dat", "BHW9.dat", "BHW10.dat",
        "CBMix11.dat", "CBMix12.dat", "CBMix13.dat", "CBMix14.dat", "CBMix15.dat", "CBMix16.dat", "CBMix17.dat", "CBMix18.dat", "CBMix19.dat", "CBMix20.dat",
        "DI-NEARP-n240-Q2k.dat", "DI-NEARP-n240-Q4k.dat", "DI-NEARP-n240-Q8k.dat", "DI-NEARP-n240-Q16k.dat",
        "DI-NEARP-n422-Q2k.dat", "DI-NEARP-n422-Q4k.dat", "DI-NEARP-n422-Q8k.dat", "DI-NEARP-n422-Q16k.dat",
        "DI-NEARP-n442-Q2k.dat", "DI-NEARP-n442-Q4k.dat",
        "mggdb_0.25_1.dat", "mggdb_0.25_2.dat", "mggdb_0.25_3.dat", "mggdb_0.25_4.dat", "mggdb_0.25_5.dat", "mggdb_0.25_6.dat", "mggdb_0.25_7.dat", "mggdb_0.25_8.dat", "mggdb_0.25_9.dat", "mggdb_0.25_10.dat",
        "mgval_0.50_8B.dat", "mgval_0.50_8C.dat", "mgval_0.50_9A.dat", "mgval_0.50_9B.dat", "mgval_0.50_9C.dat", "mgval_0.50_9D.dat",
        "mgval_0.50_10A.dat", "mgval_0.50_10B.dat", "mgval_0.50_10C.dat", "mgval_0.50_10D.dat"};

    ofstream outputFileCSV("metricas_grafos.csv");
    ofstream outputFileTxt("estrutura_grafos.txt");
    if (outputFileCSV.is_open() && outputFileTxt.is_open())
    {
        outputFileCSV << "Nome do Grafo,Quantidade de Vertices Requeridos,Quantidade de Arestas Requeridas,Quantidade de Arcos Requeridos,"
                      << "Densidade do Grafo,Grau Minimo dos Vertices,Grau Maximo dos Vertices,Diametro do Grafo,Caminho Medio do Grafo,"
                      << "Intermediacao dos Vertices\n"; // cabeçalho do CSV

        // itera sobre todos os arquivos .dat e chama a função para imprimir as métricas e o grafo
        for (const auto &nomeArquivo : arquivos)
        {
            cout << "Processando o arquivo: " << nomeArquivo << endl;

            imprimirMetricas(outputFileCSV, nomeArquivo);

            Grafo g = lerGrafoDeArquivo(nomeArquivo);
            outputFileTxt << "\n\nEstrutura do Grafo - " << nomeArquivo << "\n";
            imprimirGrafo(outputFileTxt, g);
        }

        outputFileCSV.close();
        outputFileTxt.close();
        cout << "Resultados salvos em 'metricas_grafos.csv' e 'estrutura_grafos.txt'.\n";
    }
    else
    {
        cerr << "Erro ao abrir os arquivos para escrita.\n";
    }

    return 0;
}
