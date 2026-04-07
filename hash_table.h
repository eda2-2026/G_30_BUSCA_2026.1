/**
 * @file hash_table.h
 * @brief Definições de estruturas, constantes e protótipos para a Tabela Hash.
 *
 * Este header declara a interface pública do módulo de tabela hash com
 * tratamento de colisão por encadeamento (chaining). A tabela armazena
 * registros de alunos identificados por matrícula e nome.
 *
 * @author Patrick Anderson Carvalho dos Santos 
 * @date   2026
 */

#ifndef HASH_TABLE_H
#define HASH_TABLE_H

/* ============================================================
 *  CONSTANTES
 * ============================================================ */

/**
 * Tamanho da tabela (número de buckets).
 * Escolhemos um número primo para melhorar a distribuição dos
 * índices gerados pela função hash (reduz padrões de agrupamento).
 */
#define TABLE_SIZE 11

/** Comprimento máximo do nome do aluno (incluindo '\0'). */
#define MAX_NAME 50

/** Comprimento máximo da matrícula (incluindo '\0'). */
#define MAX_ID 15

/* ============================================================
 *  ESTRUTURAS
 * ============================================================ */

/**
 * @struct Node
 * @brief Nó da lista encadeada dentro de cada bucket.
 *
 * Cada nó armazena os dados de um aluno (matrícula + nome) e um
 * ponteiro para o próximo nó, formando uma lista encadeada simples
 * que resolve colisões por encadeamento (chaining).
 */
typedef struct Node {
  char matricula[MAX_ID];   /**< Chave primária: matrícula do aluno. */
  char nome[MAX_NAME];      /**< Nome completo do aluno.             */
  struct Node *next;        /**< Ponteiro para o próximo nó (colisão). */
} Node;

/**
 * @struct HashTable
 * @brief Estrutura principal da tabela hash.
 *
 * Contém um vetor de ponteiros (buckets) — cada posição aponta para
 * o início de uma lista encadeada — além de contadores auxiliares
 * para estatísticas de uso.
 */
typedef struct {
  Node *buckets[TABLE_SIZE]; /**< Vetor de buckets (listas encadeadas). */
  int total;                 /**< Quantidade total de registros inseridos. */
  int colisoes;              /**< Quantidade acumulada de colisões.        */
} HashTable;

/* ============================================================
 *  PROTÓTIPOS DE FUNÇÕES
 * ============================================================ */

/**
 * @brief Cria e inicializa uma nova tabela hash vazia.
 * @return Ponteiro para a HashTable alocada dinamicamente.
 */
HashTable *criar_tabela();

/**
 * @brief Calcula o índice hash para uma matrícula.
 *
 * Utiliza o método polinomial (fator 31) para converter a string em
 * um valor numérico e aplica módulo TABLE_SIZE para obter o índice.
 *
 * @param matricula String da matrícula do aluno.
 * @return Índice do bucket (0 a TABLE_SIZE-1).
 */
int hash(const char *matricula);

/**
 * @brief Insere um novo registro na tabela hash.
 *
 * Verifica duplicatas antes de inserir. Se o bucket já estiver
 * ocupado, o novo nó é inserido no início da lista (colisão tratada
 * por encadeamento) e o contador de colisões é incrementado.
 *
 * @param ht        Ponteiro para a tabela hash.
 * @param matricula Matrícula do aluno (chave).
 * @param nome      Nome do aluno.
 * @return Índice do bucket onde foi inserido, ou -1 se já existir.
 */
int inserir(HashTable *ht, const char *matricula, const char *nome);

/**
 * @brief Busca um registro pela matrícula.
 *
 * Percorre a lista encadeada do bucket correspondente comparando
 * as matrículas até encontrar ou esgotar os nós.
 *
 * @param ht        Ponteiro para a tabela hash.
 * @param matricula Matrícula a ser buscada.
 * @return Ponteiro para o Node encontrado, ou NULL se não existir.
 */
Node *buscar(HashTable *ht, const char *matricula);

/**
 * @brief Remove um registro da tabela pela matrícula.
 *
 * Localiza o nó na lista encadeada, reajusta os ponteiros e
 * libera a memória do nó removido.
 *
 * @param ht        Ponteiro para a tabela hash.
 * @param matricula Matrícula do registro a ser removido.
 * @return 1 se removeu com sucesso, 0 se não encontrou.
 */
int remover_entrada(HashTable *ht, const char *matricula);

/**
 * @brief Imprime todos os buckets da tabela no terminal.
 *
 * Para cada bucket, exibe seu índice e os nós encadeados;
 * buckets vazios são marcados como "(vazio)".
 *
 * @param ht Ponteiro para a tabela hash.
 */
void imprimir_tabela(HashTable *ht);

/**
 * @brief Libera toda a memória alocada pela tabela hash.
 *
 * Percorre cada bucket, desaloca todos os nós da lista encadeada
 * e, por fim, desaloca a própria struct HashTable.
 *
 * @param ht Ponteiro para a tabela hash.
 */
void liberar_tabela(HashTable *ht);

#endif