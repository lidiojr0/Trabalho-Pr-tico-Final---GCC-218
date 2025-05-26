#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <limits>
#include <chrono>     // Para medição de tempo
#include <filesystem> // Para C++17 ou posterior (iteração de diretório)

namespace fs = std::filesystem;

#include "grafo.hpp"   // Definições de Grafo, Servico, DadosInstancia, lerGrafoDeArquivo
#include "funcoes.hpp" // Funções como floydWarshall, cálculo das métricas do grafo

using namespace std;

int main()
{
    // Caminho para a pasta contendo os arquivos de instância .dat
    // !!! ATENÇÃO: Este caminho deve ser ajustado para o seu ambiente local !!!
    string pastaInstancias = "C:\\Users\\lidio\\Downloads\\MCGRP";

    vector<string> arquivosDat; // Para armazenar os caminhos dos arquivos .dat encontrados
    try
    {
        // Itera sobre os arquivos na pasta de instâncias
        for (const auto &entry : fs::directory_iterator(pastaInstancias))
        {
            // Adiciona à lista se for um arquivo regular com extensão .dat
            if (entry.is_regular_file() && entry.path().extension() == ".dat")
            {
                arquivosDat.push_back(entry.path().string());
            }
        }
    }
    catch (const fs::filesystem_error &e)
    {
        cerr << "Erro ao acessar a pasta de instancias: " << pastaInstancias << " - " << e.what() << endl;
        return 1; // Caso não consiga acessar a pasta, termina o programa
    }

    sort(arquivosDat.begin(), arquivosDat.end()); // Ordenamento dos arquivos (é opcional para consistência do código)

    // Loop principal: processa cada arquivo de instância
    for (const auto &caminhoCompletoInstancia : arquivosDat)
    {
        cout << "\nProcessando instancia: " << caminhoCompletoInstancia << endl;

        // Inicia medição de tempo para o processamento desta instância
        auto inicio_processamento_instancia = std::chrono::high_resolution_clock::now();

        // 1. Leitura e parsing dos dados da instância
        DadosInstancia dadosInst = lerGrafoDeArquivo(caminhoCompletoInstancia);
        if (dadosInst.idNoDeposito == -1 || dadosInst.g.V == 0)
        {
            cerr << "Erro ao ler dados da instancia ou instancia invalida: " << dadosInst.nomeInstancia << endl;
            continue; // Em caso de erro, pula para a próxima instância
        }

        // 2. Cálculo da matriz de caminhos mínimos entre todos os pares de nós
        vector<vector<long long>> matDistancias = floydWarshall(dadosInst.g);

        // 3. Preparação da lista de todos os serviços requeridos (com demanda > 0)
        vector<Servico> listaTodosServicos;
        int idGlobalAtual = 1; // ID sequencial para cada serviço

        for (const auto &noReq : dadosInst.g.vertices)
        {
            if (noReq.demanda > 0)
            {
                listaTodosServicos.push_back({idGlobalAtual++, Servico::Tipo::NO,
                                              noReq.id, 0, 0,
                                              noReq.demanda, noReq.s_custo, 0,
                                              false,
                                              noReq.id, noReq.id});
            }
        }
        for (const auto &arestaReq : dadosInst.g.arestas)
        {
            if (arestaReq.demanda > 0)
            {
                listaTodosServicos.push_back({idGlobalAtual++, Servico::Tipo::ARESTA,
                                              0, arestaReq.origem, arestaReq.destino,
                                              arestaReq.demanda, arestaReq.s_custo, arestaReq.custo,
                                              false,
                                              0, 0});
            }
        }
        for (const auto &arcoReq : dadosInst.g.arcos)
        {
            if (arcoReq.demanda > 0)
            {
                listaTodosServicos.push_back({idGlobalAtual++, Servico::Tipo::ARCO,
                                              0, arcoReq.origem, arcoReq.destino,
                                              arcoReq.demanda, arcoReq.s_custo, arcoReq.custo,
                                              false,
                                              arcoReq.origem, arcoReq.destino});
            }
        }

        // 4. Algoritmo Construtivo Guloso para gerar as rotas
        long long custoTotalSolucaoAlgoritmo = 0;
        vector<string> stringsRotasSolucao;
        int idProximaRotaValida = 1; // ID para a próxima rota válida a ser gerada
        int servicosAtendidosCont = 0;
        int totalServicosRequeridos = listaTodosServicos.size();

        if (totalServicosRequeridos == 0)
        {
            cout << "Instancia " << dadosInst.nomeInstancia << " nao possui servicos requeridos." << endl;
        }
        else
        {
            int servicosAtendidosNaIteracaoAnterior; // Para salvaguarda contra loop infinito
            // Loop principal de construção de rotas: continua enquanto houver serviços não atendidos
            while (servicosAtendidosCont < totalServicosRequeridos)
            {
                servicosAtendidosNaIteracaoAnterior = servicosAtendidosCont;

                // Inicializa uma nova rota
                long long demandaRotaAtual = 0;
                long long custoRotaAtual = 0;
                vector<string> tuplasServicosRotaAtual;

                tuplasServicosRotaAtual.push_back("(D 0,1,1)"); // Início da rota no depósito
                int posAtualVeiculo = dadosInst.idNoDeposito;

                bool podeAdicionarMaisServicosARota = true;
                // Loop interno: adiciona serviços à rota atual enquanto possível
                while (podeAdicionarMaisServicosARota)
                {
                    int melhorIndiceServicoParaAdicionar = -1;     // Índice do melhor serviço encontrado
                    long long menorCustoParaServir = LONGLONG_INF; // Custo de avaliação do melhor serviço

                    // Detalhes do melhor serviço candidato
                    int p1CandidatoEscolhido = 0, p2CandidatoEscolhido = 0;
                    int proxPosVeiculoAposServicoEscolhido = -1;

                    // Itera sobre todos os serviços para encontrar o melhor para adicionar
                    for (int i = 0; i < listaTodosServicos.size(); ++i)
                    {
                        // Considera o serviço se não foi atendido e cabe na capacidade
                        if (!listaTodosServicos[i].atendido &&
                            (demandaRotaAtual + listaTodosServicos[i].demanda <= dadosInst.capacidadeVeiculo))
                        {

                            const Servico &servicoCandidato = listaTodosServicos[i];
                            long long custoTotalEsteServicoCandidato = LONGLONG_INF; // Custo de deslocamento + serviço
                            int p1Temp = 0, p2Temp = 0, posFinalTemp = 0;            // Detalhes de como servir este candidato

                            // Calcula o custo de servir e os pontos de passagem, dependendo do tipo de serviço
                            if (servicoCandidato.tipo == Servico::Tipo::NO)
                            {
                                int noAServir = servicoCandidato.idNoOriginal;
                                // Verifica validade dos nós e alcançabilidade
                                if (noAServir > 0 && noAServir <= dadosInst.g.V &&
                                    posAtualVeiculo > 0 && posAtualVeiculo <= dadosInst.g.V &&
                                    matDistancias[posAtualVeiculo][noAServir] != LONGLONG_INF)
                                {
                                    custoTotalEsteServicoCandidato = matDistancias[posAtualVeiculo][noAServir] + servicoCandidato.custoServicoProprio;
                                    p1Temp = noAServir;
                                    p2Temp = noAServir;
                                    posFinalTemp = noAServir;
                                }
                            }
                            else if (servicoCandidato.tipo == Servico::Tipo::ARESTA)
                            {
                                int u = servicoCandidato.uOriginal;
                                int v = servicoCandidato.vOriginal;
                                long long custoViaU = LONGLONG_INF, custoViaV = LONGLONG_INF;

                                if (u > 0 && u <= dadosInst.g.V && v > 0 && v <= dadosInst.g.V &&
                                    posAtualVeiculo > 0 && posAtualVeiculo <= dadosInst.g.V)
                                {
                                    // Opção 1: posAtual -> u -> v
                                    if (matDistancias[posAtualVeiculo][u] != LONGLONG_INF)
                                    {
                                        custoViaU = matDistancias[posAtualVeiculo][u] + servicoCandidato.custoTravessia + servicoCandidato.custoServicoProprio;
                                    }
                                    // Opção 2: posAtual -> v -> u
                                    if (matDistancias[posAtualVeiculo][v] != LONGLONG_INF)
                                    {
                                        custoViaV = matDistancias[posAtualVeiculo][v] + servicoCandidato.custoTravessia + servicoCandidato.custoServicoProprio;
                                    }

                                    // Escolhe a direção de serviço mais barata para a aresta
                                    if (custoViaU <= custoViaV && custoViaU != LONGLONG_INF)
                                    {
                                        custoTotalEsteServicoCandidato = custoViaU;
                                        p1Temp = u;
                                        p2Temp = v;
                                        posFinalTemp = v;
                                    }
                                    else if (custoViaV < custoViaU && custoViaV != LONGLONG_INF)
                                    {
                                        custoTotalEsteServicoCandidato = custoViaV;
                                        p1Temp = v;
                                        p2Temp = u;
                                        posFinalTemp = u;
                                    }
                                }
                            }
                            else
                            { // Tipo ARCO
                                int u = servicoCandidato.uOriginal;
                                int v = servicoCandidato.vOriginal;
                                if (u > 0 && u <= dadosInst.g.V && v > 0 && v <= dadosInst.g.V &&
                                    posAtualVeiculo > 0 && posAtualVeiculo <= dadosInst.g.V)
                                {
                                    if (matDistancias[posAtualVeiculo][u] != LONGLONG_INF)
                                    { // Deslocamento até a origem do arco
                                        custoTotalEsteServicoCandidato = matDistancias[posAtualVeiculo][u] + servicoCandidato.custoTravessia + servicoCandidato.custoServicoProprio;
                                        p1Temp = u;
                                        p2Temp = v;
                                        posFinalTemp = v;
                                    }
                                }
                            }

                            // Se este candidato for o melhor até agora, armazena seus detalhes
                            if (custoTotalEsteServicoCandidato < menorCustoParaServir)
                            {
                                menorCustoParaServir = custoTotalEsteServicoCandidato;
                                melhorIndiceServicoParaAdicionar = i;
                                p1CandidatoEscolhido = p1Temp;
                                p2CandidatoEscolhido = p2Temp;
                                proxPosVeiculoAposServicoEscolhido = posFinalTemp;
                            }
                        }
                    }

                    // Se um serviço viável foi encontrado, adiciona-o à rota
                    if (melhorIndiceServicoParaAdicionar != -1)
                    {
                        Servico &servEscolhido = listaTodosServicos[melhorIndiceServicoParaAdicionar];
                        servEscolhido.atendido = true;
                        servEscolhido.p1Saida = p1CandidatoEscolhido;
                        servEscolhido.p2Saida = p2CandidatoEscolhido;
                        servicosAtendidosCont++;

                        demandaRotaAtual += servEscolhido.demanda;
                        custoRotaAtual += menorCustoParaServir; // Adiciona custo total (deslocamento + serviço)

                        tuplasServicosRotaAtual.push_back("(S " + std::to_string(servEscolhido.idGlobal) + "," +
                                                          std::to_string(servEscolhido.p1Saida) + "," +
                                                          std::to_string(servEscolhido.p2Saida) + ")");
                        posAtualVeiculo = proxPosVeiculoAposServicoEscolhido; // Atualiza posição do veículo
                    }
                    else
                    {
                        // Nenhum serviço pode ser adicionado (capacidade, todos atendidos, inalcançável)
                        podeAdicionarMaisServicosARota = false;
                    }
                } // Fim do loop de adicionar serviços à rota atual

                // Finaliza a rota: calcula custo de retorno ao depósito
                if (posAtualVeiculo != dadosInst.idNoDeposito &&
                    (posAtualVeiculo > 0 && posAtualVeiculo <= dadosInst.g.V &&
                     dadosInst.idNoDeposito > 0 && dadosInst.idNoDeposito <= dadosInst.g.V &&
                     matDistancias[posAtualVeiculo][dadosInst.idNoDeposito] != LONGLONG_INF))
                {
                    custoRotaAtual += matDistancias[posAtualVeiculo][dadosInst.idNoDeposito];
                }
                else if (posAtualVeiculo != dadosInst.idNoDeposito)
                {
                    cerr << "ALERTA: Rota (ID proximo: " << idProximaRotaValida << ") para " << dadosInst.nomeInstancia
                         << " nao pode retornar ao deposito do no " << posAtualVeiculo << "." << endl;
                }
                tuplasServicosRotaAtual.push_back("(D 0,1,1)"); // Fim da rota no depósito

                // Armazena a rota na solução se ela atendeu pelo menos um serviço
                if (tuplasServicosRotaAtual.size() > 2)
                {
                    custoTotalSolucaoAlgoritmo += custoRotaAtual;
                    string strRota = "0 1 " + std::to_string(idProximaRotaValida) + " " +
                                     std::to_string(demandaRotaAtual) + " " +
                                     std::to_string(custoRotaAtual) + " " +
                                     std::to_string(tuplasServicosRotaAtual.size());
                    for (const auto &tupla : tuplasServicosRotaAtual)
                    {
                        strRota += " " + tupla;
                    }
                    stringsRotasSolucao.push_back(strRota);
                    idProximaRotaValida++; // Incrementa ID para a próxima rota válida
                }

                // Salvaguarda: se nenhum serviço foi adicionado nesta iteração e ainda há serviços pendentes, para.
                if (servicosAtendidosCont == servicosAtendidosNaIteracaoAnterior && servicosAtendidosCont < totalServicosRequeridos)
                {
                    cout << "Alerta: Nenhum servico adicional pode ser atendido para " << dadosInst.nomeInstancia
                         << ". Servicos atendidos: " << servicosAtendidosCont << "/" << totalServicosRequeridos
                         << ". Parando." << endl;
                    break; // Sai do loop 'while (servicosAtendidosCont < totalServicosRequeridos)'
                }
            } // Fim do loop principal de construção de rotas
        }

        // Mensagem final se nem todos os serviços foram atendidos
        if (servicosAtendidosCont < totalServicosRequeridos && totalServicosRequeridos > 0)
        {
            cout << "Alerta Final: Para " << dadosInst.nomeInstancia << ", nem todos os servicos foram atendidos. "
                 << "Atendidos: " << servicosAtendidosCont << "/" << totalServicosRequeridos << endl;
        }

        // Finaliza a medição de tempo e calcula a duração
        auto fim_processamento_instancia = std::chrono::high_resolution_clock::now();
        auto duracao_total_instancia = std::chrono::duration_cast<std::chrono::microseconds>(fim_processamento_instancia - inicio_processamento_instancia);
        long long clocks_do_seu_algoritmo = duracao_total_instancia.count(); // Tempo em microssegundos

        cout << "Tempo de processamento para " << dadosInst.nomeInstancia << ": " << clocks_do_seu_algoritmo << " microssegundos." << endl;

        // 5. Geração do arquivo de saída da solução
        string nomeArquivoSaida = "sol-" + dadosInst.nomeInstancia + ".dat";
        ofstream arqSaida(nomeArquivoSaida);
        if (arqSaida.is_open())
        {
            arqSaida << custoTotalSolucaoAlgoritmo << endl;
            arqSaida << stringsRotasSolucao.size() << endl;
            arqSaida << clocks_do_seu_algoritmo << endl; // Clocks do seu algoritmo
            arqSaida << clocks_do_seu_algoritmo << endl; // Clocks do seu algoritmo (tempo para melhor solução é o mesmo na Etapa 2)
            for (const auto &r_str : stringsRotasSolucao)
            {
                arqSaida << r_str << endl;
            }
            arqSaida.close();
            cout << "Solucao para " << dadosInst.nomeInstancia << " salva em " << nomeArquivoSaida << endl;
        }
        else
        {
            cerr << "Erro ao abrir arquivo de saida: " << nomeArquivoSaida << endl;
        }
    } // Fim do loop de processamento de instâncias
    return 0;
}
