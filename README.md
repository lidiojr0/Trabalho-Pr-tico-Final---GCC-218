# Trabalho Prático Final - Etapa 1

## Autores
- Lídio Júnior Pereira Batista  
- Pyetro Augusto Malaquias

## Descrição
- Este projeto corresponde à Etapa 1 do Trabalho Prático Final da disciplina de Algoritmos em Grafos e tem como objetivo analisar diferentes instâncias de grafos, calculando métricas essenciais e proporcionando visualizações para análise aprofundada. 
- O programa realiza a leitura de instâncias `.dat`, indica os grafos correspondentes no `.txt` gerado e calcula as métricas pedidas no `.csv` gerado. 
- Com os arquivos gerados é possível fazer a visualização com o `.ìpynb`.

## Como executar

- Para executar, é necessário: compilar o `main.cpp` utilizando um compilador C++ - como g++ - e fornecer como entrada os 50 arquivos `.dat`.
- Será gerado `metricas_grafos.csv` e `estrutura_grafos.txt`; para a sua visualização execute o `visualizacao_metricas.ipynb` e `visualizacao_estrutura.ipynb`, respectivamente, em ambiente como o Jupyter Notebook. Certifique-se de ter instalado o `pandas` (Python).

### Instâncias de entrada:

- As 50 instâncias de entrada podem ser obtidas no arquivo `202501_selected_instances.zip`.

## Estrutura do projeto

- `main.cpp`: Arquivo principal que executa a leitura, processamento e geração dos arquivos .csv e .txt.
- `grafo.hpp`: Contém a definição das estruturas de dados para representar grafos (vértices, arestas e arcos), bem como funções associadas a eles.
- `funcoes.hpp`: Contém funções auxiliares para o cálculo das métricas e processamento do grafo, como o algoritmo de Floyd-Warshall, cálculo de densidade, diâmetro, etc.
- A visualização do projeto é composta por dois arquivos: `visualizacao_metricas.ipynb` e `visualizacao_estrutura.ipynb`.

## Resultados esperados

- O arquivo `.csv` contém métricas que podem ser analisadas para avaliar a qualidade dos grafos.
- O arquivo `.txt` contém a descrição completa do grafo, útil para inspeção visual ou depuração.
