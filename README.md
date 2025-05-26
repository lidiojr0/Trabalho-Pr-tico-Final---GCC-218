# Trabalho Prático Final - Algoritmos em Grafos (Etapas 1 e 2)

## Autores
- Lídio Júnior Pereira Batista  
- Pyetro Augusto Malaquias

## Descrição
Este projeto é parte do Trabalho Prático Final da disciplina de Algoritmos em Grafos.
- A **Etapa 1** (fundação do projeto) focou na análise de diferentes instâncias de grafos, calculando métricas essenciais e proporcionando visualizações para análise aprofundada. Ela realiza a leitura de instâncias `.dat`, indica os grafos correspondentes no `.txt` gerado e calcula as métricas pedidas no `.csv` gerado.
- A **Etapa 2** (implementada atualmente) desenvolve um algoritmo construtivo para o Problema de Coleta de Lixo Capacitado com Múltiplos Veículos (MCGRP), considerando serviços requeridos em nós, arestas e arcos. O programa lê as instâncias, aplica uma heurística gulosa para construir rotas viáveis e gera arquivos de solução no formato especificado.

O objetivo é encontrar um conjunto de rotas de custo mínimo que atendam a todos os serviços requeridos, respeitando a capacidade dos veículos.

## Estrutura do Projeto
O código está organizado nos seguintes arquivos principais:

* `main.cpp`: Contém a função principal do programa. Para a Etapa 2, ele é responsável por:
    * Iterar sobre os arquivos de instância `.dat` em uma pasta especificada.
    * Chamar as funções de leitura e parsing da instância.
    * Executar o algoritmo de Floyd-Warshall para obter os caminhos mínimos.
    * Implementar a heurística construtiva gulosa para gerar as rotas da solução.
    * Medir o tempo de execução do algoritmo para cada instância.
    * Gerar os arquivos de solução (`sol-NOME_DA_INSTANCIA.dat`).
* `grafo.hpp`: Define as estruturas de dados essenciais para representar o grafo:
    * `struct Aresta`, `struct Arco`, `struct Vertice`.
    * `class Grafo`: Representa o grafo com suas listas de adjacência e elementos.
    * `struct Servico`: Estrutura unificada para representar nós, arestas ou arcos requeridos.
    * `struct DadosInstancia`: Agrega o objeto `Grafo` e outros dados relevantes da instância (depósito, capacidade do veículo, nome).
    * Função `lerGrafoDeArquivo`: Realiza o parsing dos arquivos de instância `.dat`.
    * Função `extrairNomeBaseInstancia`: Utilitário para obter o nome da instância.
* `funcoes.hpp`: Contém funções auxiliares e algoritmos sobre grafos:
    * Constante `LONGLONG_INF` para representar infinito.
    * Funções para calcular métricas do grafo (relevantes para a Etapa 1): `calcularVerticesRequeridos`, `calcularArestasRequeridas`, `calcularArcosRequeridos`, `calcularDensidade`, `calcularGrauMinimo`, `calcularGrauMaximo`, `calcularDiametro`, `calcularIntermediacao`, `calcularCaminhoMedio`.
    * Função `floydWarshall`: Calcula os caminhos mínimos entre todos os pares de vértices (essencial para a Etapa 2).
    * Funções de impressão para a Etapa 1: `imprimirGrafo` e `imprimirMetricas`.

## Como Executar (Etapa 2)

1.  **Configuração do Ambiente:**
    * Certifique-se de ter um compilador C++ (como g++) que suporte C++17 (necessário para `std::filesystem`).
2.  **Compilação:**
    * Compile o arquivo `main.cpp` (que deve incluir os outros `.hpp` necessários).
3.  **Preparação das Instâncias:**
    * As instâncias de entrada (arquivos `.dat`) devem estar localizadas em uma pasta.
    * **Importante:** Você precisará ajustar o caminho para esta pasta dentro do arquivo `main.cpp`, na variável `pastaInstancias`.
        ```cpp
        // Em main.cpp, ajuste esta linha:
        std::string pastaInstancias = "C:\\Users\\lidio\\Downloads\\MCGRP"; // Exemplo de caminho
        ```
4.  **Execução:**
    * Execute o programa compilado a partir de um terminal.

5.  **Saída:**
    * O programa processará cada arquivo `.dat` encontrado na `pastaInstancias`.
    * Para cada instância, será gerado um arquivo de solução no formato `sol-NOME_DA_INSTANCIA.dat` no mesmo diretório onde o executável foi rodado.
    * O console exibirá informações sobre o processamento de cada instância, incluindo o tempo de execução.

### Instâncias de Entrada:

* As instâncias de entrada podem ser obtidas no arquivo `MCGRP.zip`.

## Funcionalidades Implementadas (Etapa 2)

* Leitura e parsing de instâncias de grafos no formato `.dat`, identificando nós, arestas, arcos, serviços requeridos, depósito e capacidade dos veículos.
* Implementação do algoritmo de Floyd-Warshall para cálculo de caminhos mínimos entre todos os pares de vértices.
* Desenvolvimento de uma **heurística construtiva gulosa** para gerar uma solução inicial para o problema de roteamento:
    * Criação de rotas que iniciam e terminam no depósito.
    * Atendimento de serviços requeridos (nós, arestas ou arcos com demanda positiva).
    * Respeito à capacidade máxima de cada veículo.
    * Garantia de que cada serviço requerido seja atendido exatamente uma vez.
    * Contabilização correta de custos e demandas.
* Medição do tempo de processamento (em microssegundos) do algoritmo para cada instância.
* Geração de arquivos de solução (`sol-*.dat`) no formato especificado, detalhando:
    * Custo total da solução encontrada pelo algoritmo.
    * Número total de rotas na solução.
    * Tempo de execução do algoritmo.
    * Descrição de cada rota, incluindo os serviços atendidos.

**Formato dos Arquivos de Saída (`sol-NOME_DA_INSTANCIA.dat`)**

## Próximas Etapas (Conforme o Plano do Trabalho Prático)
* Etapa 3: Implementação de métodos de melhoria para a solução inicial.
* Etapa 4: Elaboração do relatório final e entrevista.
