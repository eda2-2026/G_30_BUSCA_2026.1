# G_30_BUSCA_2026.1

## 📋 Informações do Projeto

| Campo | Detalhes |
|-------|----------|
| **Disciplina** | Estruturas de Dados 2 — Busca |
| **Atividade** | Trabalho 1  - Busca |
| **Linguagem** | C |
| **Tema** | Cadastro de matrículas usando Tabela Hash com tratamento de colisão por encadeamento |

## 👥 Alunos

| Nome | Matrícula | Responsabilidade |
|------|-----------|------------------|
| Patrick Anderson Carvalho dos Santos | 211030620 |  Estrutura base (`hash_table.c` / `hash_table.h`) |
| Vinicius Castelo Ferreira | 200062883 | Aplicação (`main.c` + `dados.txt`) |

---

## 📖 Sobre

Implementação de uma **Tabela Hash** para cadastro de matrículas de alunos, com o objetivo de demonstrar conceitos de **performance em buscas** e **tratamento de colisão**.

A tabela utiliza:
- **Função hash polinomial** (fator 31) com operação de módulo
- **Tratamento de colisão por encadeamento** (listas encadeadas em cada bucket)
- **Tamanho primo** (11 buckets) para melhor distribuição dos índices

---

## 🏗️ Arquitetura

```
trabalho1/
├── hash_table.h    ← Definições de structs, constantes e protótipos (Pessoa 1)
├── hash_table.c    ← Implementação das funções da tabela hash     (Pessoa 1)
├── main.c          ← Aplicação: leitura em lote, busca, relatório (Pessoa 2)
├── dados.txt       ← Arquivo com matrículas para inserção em lote (Pessoa 2)
└── README.md
```

---

## 🔧 Estrutura Base — Pessoa 1

### Estruturas de Dados

#### `Node` (nó da lista encadeada)
```c
typedef struct Node {
    char matricula[MAX_ID];   // Chave primária: matrícula do aluno
    char nome[MAX_NAME];      // Nome completo do aluno
    struct Node *next;        // Ponteiro para próximo nó (colisão)
} Node;
```

#### `HashTable` (tabela principal)
```c
typedef struct {
    Node *buckets[TABLE_SIZE]; // Vetor de 11 buckets (listas encadeadas)
    int total;                 // Total de registros inseridos
    int colisoes;              // Contador acumulado de colisões
} HashTable;
```

### Funções Implementadas

| Função | Descrição | Complexidade |
|--------|-----------|:------------:|
| `criar_tabela()` | Aloca e inicializa a tabela com todos os buckets vazios | O(TABLE_SIZE) |
| `hash(matricula)` | Converte matrícula em índice usando hash polinomial (fator 31) | O(n) |
| `inserir(ht, matricula, nome)` | Insere registro com verificação de duplicata e contagem de colisão | O(1) amortizado |
| `buscar(ht, matricula)` | Busca registro pela matrícula na lista encadeada do bucket | O(1 + α) |
| `remover_entrada(ht, matricula)` | Remove registro reajustando ponteiros da lista e liberando memória | O(n) pior caso |
| `imprimir_tabela(ht)` | Exibe todos os buckets e seus encadeamentos no terminal | O(TABLE_SIZE + n) |
| `liberar_tabela(ht)` | Desaloca todos os nós e a struct da tabela | O(TABLE_SIZE + n) |

> **α (alfa)** = fator de carga = `total / TABLE_SIZE`

### Como Funciona a Função Hash

```
Entrada: "211030620"

h = 0
h = 0 * 31 + '2'(50)   =   50
h = 50 * 31 + '1'(49)   = 1599
h = 1599 * 31 + '1'(49) = 49618
...  (continua para cada caractere)

Índice = h % 11   →   valor entre 0 e 10
```

### Como Funciona o Encadeamento (Chaining)

Quando duas matrículas geram o **mesmo índice hash**, os nós são encadeados em uma lista:

```
Bucket [3]: -> 211030620 (Patrick) -> 200062883 (Vinicius)
Bucket [7]: -> 190045321 (Ana)
Bucket [5]: (vazio)
```

---

## 🛠️ Compilação

**Pré-requisitos:**
- Compilador C (`gcc` recomendado)

**Compilar:**
```bash
gcc -o trabalho1 main.c hash_table.c -Wall -Wextra
```

**Executar:**
```bash
./trabalho1
```

---

## 📹 Apresentação

[Primeira Apresentação](https://youtu.be/xyz)

---

## 📝 Observações

- O tamanho da tabela (`TABLE_SIZE = 11`) é um número primo para melhorar a distribuição dos índices e reduzir agrupamentos.
- O contador de colisões é **cumulativo** — não é decrementado ao remover registros.
- A inserção é feita no **início da lista** (O(1)) para eficiência.
