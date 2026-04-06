/**
 * @file hash_table.c
 * @brief Implementação das funções da Tabela Hash com encadeamento.
 *
 * Este arquivo contém a lógica completa da estrutura base:
 *   - Criação e destruição da tabela
 *   - Função hash (método polinomial / fator 31)
 *   - Inserção com tratamento de colisão por encadeamento
 *   - Busca por matrícula
 *   - Remoção de registro
 *   - Impressão visual da tabela
 *
 * @author Patrick Anderson Carvalho dos Santos (Pessoa 1)
 * @date   2026
 */

#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
 *  criar_tabela
 * ============================================================ */

/**
 * @brief Aloca e inicializa a tabela hash.
 *
 * 1. Aloca memória para a struct HashTable.
 * 2. Zera os contadores (total de registros e colisões).
 * 3. Inicializa todos os buckets como NULL (vazios).
 *
 * Complexidade: O(TABLE_SIZE) — percorre o vetor de buckets.
 *
 * @return Ponteiro para a nova tabela pronta para uso.
 */
HashTable *criar_tabela() {
  HashTable *ht = malloc(sizeof(HashTable));
  ht->total = 0;
  ht->colisoes = 0;
  for (int i = 0; i < TABLE_SIZE; i++)
    ht->buckets[i] = NULL;
  return ht;
}

/* ============================================================
 *  hash  (Função de Dispersão)
 * ============================================================ */

/**
 * @brief Converte uma string de matrícula em um índice da tabela.
 *
 * Algoritmo — Hash Polinomial (fator 31):
 *   h = 0
 *   Para cada caractere c da matrícula:
 *       h = h * 31 + c
 *   retorna h % TABLE_SIZE
 *
 * O fator 31 é um primo pequeno amplamente utilizado (ex.: Java String)
 * pois distribui bem os valores e minimiza colisões.
 *
 * O módulo TABLE_SIZE (11, também primo) garante que o resultado
 * fique dentro dos limites do vetor de buckets [0..10].
 *
 * Complexidade: O(n), onde n = comprimento da string.
 *
 * @param matricula String com a matrícula do aluno.
 * @return Índice do bucket correspondente (0 a TABLE_SIZE-1).
 */
int hash(const char *matricula) {
  unsigned long h = 0;
  for (int i = 0; matricula[i] != '\0'; i++)
    h = h * 31 + matricula[i];
  return h % TABLE_SIZE;
}

/* ============================================================
 *  inserir
 * ============================================================ */

/**
 * @brief Insere um novo aluno na tabela hash.
 *
 * Fluxo:
 *   1. Calcula o índice com a função hash.
 *   2. Percorre a lista encadeada do bucket para verificar se a
 *      matrícula já existe (evita duplicatas).
 *   3. Aloca um novo nó e copia matrícula + nome.
 *   4. Insere o novo nó NO INÍCIO da lista (O(1)):
 *        novo->next = bucket_atual
 *        bucket = novo
 *   5. Se o bucket já tinha pelo menos um nó, incrementa colisões.
 *   6. Incrementa o total de registros.
 *
 * Tratamento de colisão: ENCADEAMENTO (chaining)
 *   → Múltiplas chaves que mapeiam para o mesmo bucket são
 *     armazenadas em uma lista encadeada simples.
 *
 * Complexidade:
 *   - Melhor caso:  O(1) — bucket vazio.
 *   - Pior caso:    O(n) — todos os elementos no mesmo bucket.
 *   - Caso médio:   O(1 + α), onde α = fator de carga (total/TABLE_SIZE).
 *
 * @param ht        Ponteiro para a tabela.
 * @param matricula Matrícula do aluno (chave de busca).
 * @param nome      Nome completo do aluno.
 * @return Índice do bucket utilizado, ou -1 se a matrícula já existir.
 */
int inserir(HashTable *ht, const char *matricula, const char *nome) {
  int idx = hash(matricula);
  Node *atual = ht->buckets[idx];

  /* --- Verificação de duplicata --- */
  while (atual != NULL) {
    if (strcmp(atual->matricula, matricula) == 0)
      return -1; // matrícula já cadastrada
    atual = atual->next;
  }

  /* --- Criação do novo nó --- */
  Node *novo = malloc(sizeof(Node));
  strncpy(novo->matricula, matricula, MAX_ID);
  strncpy(novo->nome, nome, MAX_NAME);

  /* --- Inserção no início da lista (encadeamento) --- */
  novo->next = ht->buckets[idx];

  /* Se o bucket já estava ocupado, é uma colisão */
  if (ht->buckets[idx] != NULL)
    ht->colisoes++;

  ht->buckets[idx] = novo;
  ht->total++;
  return idx;
}

/* ============================================================
 *  buscar
 * ============================================================ */

/**
 * @brief Busca um registro na tabela pela matrícula.
 *
 * Fluxo:
 *   1. Calcula o índice com a função hash.
 *   2. Percorre a lista encadeada do bucket comparando matrículas.
 *   3. Retorna o ponteiro do nó encontrado ou NULL.
 *
 * Complexidade:
 *   - Melhor caso:  O(1) — primeiro nó do bucket.
 *   - Pior caso:    O(n) — percorre toda a lista encadeada.
 *   - Caso médio:   O(1 + α).
 *
 * @param ht        Ponteiro para a tabela.
 * @param matricula Matrícula a buscar.
 * @return Ponteiro para o Node encontrado, ou NULL se não encontrado.
 */
Node *buscar(HashTable *ht, const char *matricula) {
  int idx = hash(matricula);
  Node *atual = ht->buckets[idx];

  while (atual != NULL) {
    if (strcmp(atual->matricula, matricula) == 0)
      return atual; // encontrou
    atual = atual->next;
  }
  return NULL; // não encontrado
}

/* ============================================================
 *  remover_entrada
 * ============================================================ */

/**
 * @brief Remove um registro da tabela pela matrícula.
 *
 * Fluxo:
 *   1. Calcula o índice com a função hash.
 *   2. Percorre a lista encadeada mantendo um ponteiro "anterior"
 *      para reajustar os ponteiros ao remover o nó.
 *   3. Caso o nó a ser removido seja o primeiro da lista:
 *        bucket[idx] = nó->next
 *      Caso contrário:
 *        anterior->next = nó->next
 *   4. Libera a memória do nó removido.
 *   5. Decrementa o total de registros.
 *
 * Nota: o contador de colisões NÃO é decrementado na remoção,
 * pois ele é cumulativo (registra o total histórico de colisões).
 *
 * Complexidade: O(n) no pior caso (percorre toda a lista do bucket).
 *
 * @param ht        Ponteiro para a tabela.
 * @param matricula Matrícula do registro a remover.
 * @return 1 se removeu com sucesso, 0 se não encontrou.
 */
int remover_entrada(HashTable *ht, const char *matricula) {
  int idx = hash(matricula);
  Node *atual = ht->buckets[idx];
  Node *anterior = NULL;

  while (atual != NULL) {
    if (strcmp(atual->matricula, matricula) == 0) {
      /* Reajusta os ponteiros da lista encadeada */
      if (anterior == NULL)
        ht->buckets[idx] = atual->next; // removendo o primeiro nó
      else
        anterior->next = atual->next;   // removendo nó do meio/fim

      free(atual);
      ht->total--;
      return 1; // sucesso
    }
    anterior = atual;
    atual = atual->next;
  }
  return 0; // matrícula não encontrada
}

/* ============================================================
 *  imprimir_tabela
 * ============================================================ */

/**
 * @brief Exibe o conteúdo completo da tabela hash no terminal.
 *
 * Formato de saída:
 *   [ 0] (vazio)
 *   [ 1] -> 211030620 (Patrick) -> 200062883 (Vinicius)
 *   [ 2] (vazio)
 *   ...
 *
 * Cada bucket mostra sua lista encadeada com setas "->".
 *
 * Complexidade: O(TABLE_SIZE + n), onde n = total de registros.
 *
 * @param ht Ponteiro para a tabela.
 */
void imprimir_tabela(HashTable *ht) {
  printf("\n--- Tabela Hash ---\n");
  for (int i = 0; i < TABLE_SIZE; i++) {
    printf("[%2d] ", i);
    Node *atual = ht->buckets[i];
    if (atual == NULL) {
      printf("(vazio)\n");
    } else {
      while (atual != NULL) {
        printf("-> %s (%s) ", atual->matricula, atual->nome);
        atual = atual->next;
      }
      printf("\n");
    }
  }
}

/* ============================================================
 *  liberar_tabela
 * ============================================================ */

/**
 * @brief Libera toda a memória alocada pela tabela.
 *
 * Fluxo:
 *   1. Para cada bucket, percorre a lista encadeada liberando
 *      cada nó individualmente (usa ponteiro temporário para
 *      não perder a referência ao próximo nó).
 *   2. Libera a própria struct HashTable.
 *
 * IMPORTANTE: após chamar esta função, o ponteiro da tabela
 * se torna inválido e não deve mais ser utilizado.
 *
 * Complexidade: O(TABLE_SIZE + n), onde n = total de registros.
 *
 * @param ht Ponteiro para a tabela a ser liberada.
 */
void liberar_tabela(HashTable *ht) {
  for (int i = 0; i < TABLE_SIZE; i++) {
    Node *atual = ht->buckets[i];
    while (atual != NULL) {
      Node *tmp = atual;
      atual = atual->next;
      free(tmp);
    }
  }
  free(ht);
}