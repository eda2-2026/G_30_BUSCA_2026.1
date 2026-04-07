/**
 * @file main.c
 * @brief Aplicação principal — Tabela Hash de Matrículas.
 *
 *
 * Formato esperado de dados.txt (uma entrada por linha):
 *   <matricula> <nome_completo_com_espacos>
 *   Exemplo:
 *   211030620 Patrick Anderson Carvalho dos Santos
 *
 * @author Vinicius Castelo Ferreira 
 * @date   2026
 */

#include "hash_table.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* ============================================================
 *  CONSTANTES
 * ============================================================ */

/** Nome padrão do arquivo de dados. */
#define ARQUIVO_DADOS "dados.txt"

/** Tamanho máximo de uma linha do arquivo. */
#define MAX_LINHA 128

/* ============================================================
 *  Protótipos internos
 * ============================================================ */

static int  carregar_arquivo(HashTable *ht, const char *caminho);
static void menu_busca_interativa(HashTable *ht);
static void exibir_relatorio(HashTable *ht);
static void limpar_buffer();

/* ============================================================
 *  main
 * ============================================================ */

/**
 * @brief Ponto de entrada do programa.
 *
 * Fluxo:
 *   1. Cria a tabela hash.
 *   2. Carrega registros do arquivo dados.txt.
 *   3. Imprime a tabela visual.
 *   4. Abre o menu de busca interativa.
 *   5. Exibe o relatório final.
 *   6. Libera a memória.
 *
 * @return 0 em caso de sucesso, 1 em caso de erro crítico.
 */
int main(void) {
    printf("============================================\n");
    printf("   TABELA HASH — Cadastro de Matrículas    \n");
    printf("============================================\n\n");

    /* 1. Cria a tabela */
    HashTable *ht = criar_tabela();

    /* 2. Carrega registros do arquivo */
    int carregados = carregar_arquivo(ht, ARQUIVO_DADOS);
    if (carregados < 0) {
        fprintf(stderr, "[ERRO] Encerrando o programa.\n");
        liberar_tabela(ht);
        return 1;
    }
    printf("[OK] %d registro(s) carregado(s) de '%s'.\n\n", carregados, ARQUIVO_DADOS);

    /* 3. Imprime a tabela visual */
    imprimir_tabela(ht);

    /* 4. Menu de busca interativa */
    menu_busca_interativa(ht);

    /* 5. Relatório final */
    exibir_relatorio(ht);

    /* 6. Libera memória */
    liberar_tabela(ht);
    printf("\nMemória liberada. Encerrando.\n");
    return 0;
}

/* ============================================================
 *  carregar_arquivo
 * ============================================================ */

/**
 * @brief Lê o arquivo de dados e insere os registros na tabela.
 *
 * Cada linha do arquivo deve conter:
 *   <matricula> <nome completo>
 *
 * Linhas em branco e linhas que começam com '#' são ignoradas
 * (útil para comentários no arquivo de dados).
 *
 * Fluxo por linha:
 *   1. Faz sscanf para extrair a matrícula (primeiro token).
 *   2. O restante da linha é tratado como nome do aluno.
 *   3. Chama inserir(); reporta duplicatas como aviso.
 *
 * @param ht      Ponteiro para a tabela hash.
 * @param caminho Caminho do arquivo .txt.
 * @return Número de registros inseridos, ou -1 em erro de abertura.
 */
static int carregar_arquivo(HashTable *ht, const char *caminho) {
    FILE *f = fopen(caminho, "r");
    if (!f) {
        fprintf(stderr, "[ERRO] Não foi possível abrir o arquivo '%s'.\n", caminho);
        return -1;
    }

    char linha[MAX_LINHA];
    char matricula[MAX_ID];
    char nome[MAX_NAME];
    int  inseridos = 0;
    int  num_linha = 0;

    printf("--- Lendo '%s' ---\n", caminho);

    while (fgets(linha, sizeof(linha), f)) {
        num_linha++;

        /* Remove newline final */
        linha[strcspn(linha, "\n")] = '\0';

        /* Ignora linhas em branco ou comentários */
        if (linha[0] == '\0' || linha[0] == '#')
            continue;

        /* Extrai matrícula (primeiro token) */
        if (sscanf(linha, "%14s", matricula) != 1) {
            fprintf(stderr, "[AVISO] Linha %d ignorada (formato inválido): %s\n",
                    num_linha, linha);
            continue;
        }

        /* Nome = tudo após a matrícula + espaço */
        const char *pos_nome = linha + strlen(matricula);
        while (*pos_nome == ' ') pos_nome++; /* pula espaços entre matrícula e nome */

        if (*pos_nome == '\0') {
            fprintf(stderr, "[AVISO] Linha %d sem nome — matrícula '%s' ignorada.\n",
                    num_linha, matricula);
            continue;
        }

        strncpy(nome, pos_nome, MAX_NAME - 1);
        nome[MAX_NAME - 1] = '\0';

        /* Insere na tabela */
        int idx = inserir(ht, matricula, nome);
        if (idx == -1) {
            printf("  [DUP]  Matrícula '%s' já existe — ignorada.\n", matricula);
        } else {
            printf("  [+] Bucket[%2d] ← %s (%s)\n", idx, matricula, nome);
            inseridos++;
        }
    }

    fclose(f);
    return inseridos;
}

/* ============================================================
 *  menu_busca_interativa
 * ============================================================ */

/**
 * @brief Loop de menu para operações interativas via terminal.
 *
 * Opções disponíveis:
 *   1 — Buscar por matrícula
 *   2 — Remover por matrícula
 *   3 — Inserir novo registro manualmente
 *   4 — Reimprimir tabela
 *   0 — Sair do menu
 *
 * @param ht Ponteiro para a tabela hash.
 */
static void menu_busca_interativa(HashTable *ht) {
    int opcao;
    char matricula[MAX_ID];
    char nome[MAX_NAME];

    do {
        printf("\n============================================\n");
        printf("  MENU\n");
        printf("  1. Buscar matrícula\n");
        printf("  2. Remover matrícula\n");
        printf("  3. Inserir novo registro\n");
        printf("  4. Reimprimir tabela\n");
        printf("  0. Encerrar e ver relatório\n");
        printf("============================================\n");
        printf("Opção: ");

        if (scanf("%d", &opcao) != 1) {
            limpar_buffer();
            printf("[AVISO] Entrada inválida.\n");
            opcao = -1;
            continue;
        }
        limpar_buffer();

        switch (opcao) {

            /* ---- Busca ---- */
            case 1:
                printf("Matrícula para buscar: ");
                if (fgets(matricula, sizeof(matricula), stdin)) {
                    matricula[strcspn(matricula, "\n")] = '\0';
                    Node *found = buscar(ht, matricula);
                    if (found) {
                        printf("[ENCONTRADO] Matrícula: %-15s | Nome: %s\n",
                               found->matricula, found->nome);
                        printf("             Bucket: %d\n", hash(found->matricula));
                    } else {
                        printf("[NÃO ENCONTRADO] Matrícula '%s' não está na tabela.\n",
                               matricula);
                    }
                }
                break;

            /* ---- Remoção ---- */
            case 2:
                printf("Matrícula para remover: ");
                if (fgets(matricula, sizeof(matricula), stdin)) {
                    matricula[strcspn(matricula, "\n")] = '\0';
                    int ok = remover_entrada(ht, matricula);
                    if (ok)
                        printf("[REMOVIDO] Matrícula '%s' removida com sucesso.\n", matricula);
                    else
                        printf("[NÃO ENCONTRADO] Matrícula '%s' não existe na tabela.\n", matricula);
                }
                break;

            /* ---- Inserção manual ---- */
            case 3:
                printf("Nova matrícula: ");
                if (fgets(matricula, sizeof(matricula), stdin)) {
                    matricula[strcspn(matricula, "\n")] = '\0';
                }
                printf("Nome do aluno : ");
                if (fgets(nome, sizeof(nome), stdin)) {
                    nome[strcspn(nome, "\n")] = '\0';
                }
                {
                    int idx = inserir(ht, matricula, nome);
                    if (idx == -1)
                        printf("[DUP] Matrícula '%s' já existe na tabela.\n", matricula);
                    else
                        printf("[INSERIDO] Bucket[%d] ← %s (%s)\n", idx, matricula, nome);
                }
                break;

            /* ---- Reimprimir ---- */
            case 4:
                imprimir_tabela(ht);
                break;

            case 0:
                printf("Saindo do menu...\n");
                break;

            default:
                printf("[AVISO] Opção inválida. Tente novamente.\n");
        }

    } while (opcao != 0);
}

/* ============================================================
 *  exibir_relatorio
 * ============================================================ */

/**
 * @brief Exibe o relatório final de uso da tabela hash.
 *
 * Informações exibidas:
 *   - Total de registros inseridos
 *   - Número acumulado de colisões
 *   - Fator de carga  α = total / TABLE_SIZE
 *   - Buckets ocupados vs. vazios
 *   - Bucket com maior cadeia (pior caso de busca)
 *
 * @param ht Ponteiro para a tabela hash.
 */
static void exibir_relatorio(HashTable *ht) {
    int ocupados      = 0;
    int maior_cadeia  = 0;
    int idx_maior     = -1;

    for (int i = 0; i < TABLE_SIZE; i++) {
        int tamanho = 0;
        Node *atual = ht->buckets[i];
        while (atual != NULL) {
            tamanho++;
            atual = atual->next;
        }
        if (tamanho > 0) ocupados++;
        if (tamanho > maior_cadeia) {
            maior_cadeia = tamanho;
            idx_maior    = i;
        }
    }

    float fator_carga = (TABLE_SIZE > 0)
                        ? (float)ht->total / TABLE_SIZE
                        : 0.0f;

    printf("\n============================================\n");
    printf("             RELATÓRIO FINAL                \n");
    printf("============================================\n");
    printf("  Tamanho da tabela (buckets) : %d\n",    TABLE_SIZE);
    printf("  Total de registros          : %d\n",    ht->total);
    printf("  Colisões acumuladas         : %d\n",    ht->colisoes);
    printf("  Fator de carga  (α)         : %.4f\n",  fator_carga);
    printf("  Buckets ocupados            : %d / %d\n", ocupados, TABLE_SIZE);
    printf("  Buckets vazios              : %d / %d\n", TABLE_SIZE - ocupados, TABLE_SIZE);
    if (idx_maior >= 0)
        printf("  Maior cadeia                : %d nó(s) — bucket[%d]\n",
               maior_cadeia, idx_maior);
    else
        printf("  Tabela vazia.\n");
    printf("============================================\n");
}

/* ============================================================
 *  limpar_buffer  (utilitário interno)
 * ============================================================ */

/**
 * @brief Descarta caracteres restantes no buffer de entrada (stdin).
 *
 * Chamada após scanf() para evitar que '\n' ou lixo residual
 * contaminem a próxima leitura com fgets().
 */
static void limpar_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}
