# Projects in C / Projetos em C

[🇺🇸 English](#english) · [🇧🇷 Português](#português)

---

## English

A collection of low-level **C** projects organized into dedicated folders, focused on performance, hardware-aware programming, and fine-grained resource control.

### Core Focus Areas

- **Dynamic Memory Allocation:** Manual heap management (`malloc`, `calloc`, `realloc`, `free`), dynamic buffers, and strict memory lifecycle control without leaks.
- **CPU Register Utilization:** Low-level optimization techniques aimed at maximizing register usage and minimizing unnecessary memory access overhead.
- **Data Structures:** Custom implementations of fundamental and advanced data structures built from scratch for speed and predictable memory footprints.
- **Fast & Optimized File I/O:** High-throughput file reading and parsing using buffered operations, binary streams, and minimal-overhead processing.

### Repository Structure

Each project is self-contained inside its own directory:

```text
Projects-in-C/
├── README.md
└── RC5-Dynamic/
    └── RC5 Dynamic.c
```

| Folder | Description | Key Concepts |
| :--- | :--- | :--- |
| **`RC5-Dynamic/`** | Dynamic implementation of the RC5 symmetric block cipher. | Dynamic memory allocation, bitwise operations, buffer handling |

### Building & Running

All projects are written in standard C and can be compiled with `GCC` or `Clang` using optimization flags:

```bash
gcc -O3 -Wall -Wextra "RC5-Dynamic/RC5 Dynamic.c" -o rc5_dynamic
./rc5_dynamic
```

---

## Português

Uma coleção de projetos de baixo nível em **C** organizados em pastas dedicadas, com foco em desempenho, programação próxima ao hardware e controle preciso de recursos.

### Principais Áreas de Foco

- **Alocação Dinâmica de Memória:** Gerenciamento manual da *heap* (`malloc`, `calloc`, `realloc`, `free`), *buffers* dinâmicos e controle rigoroso do ciclo de vida da memória sem vazamentos (*memory leaks*).
- **Uso de Registradores:** Técnicas de otimização em baixo nível voltadas para maximizar o aproveitamento de registradores da CPU e reduzir acessos desnecessários à memória.
- **Estruturas de Dados:** Implementações próprias de estruturas de dados fundamentais e avançadas construídas do zero para alta velocidade e uso previsível de memória.
- **Arquivos de Leitura Rápida e Otimizada:** Leitura e processamento de arquivos de alto desempenho utilizando operações com *buffer*, fluxos binários e baixo *overhead*.

### Estrutura do Repositório

Cada projeto é mantido de forma independente dentro de sua própria pasta:

```text
Projects-in-C/
├── README.md
└── RC5-Dynamic/
    └── RC5 Dynamic.c
```

| Pasta | Descrição | Conceitos-Chave |
| :--- | :--- | :--- |
| **`RC5-Dynamic/`** | Implementação dinâmica da cifra simétrica de blocos RC5. | Alocação dinâmica de memória, operações bit a bit, manipulação de *buffers* |

### Compilação e Execução

Todos os projetos são escritos em C padrão e podem ser compilados com `GCC` ou `Clang` utilizando flags de otimização:

```bash
gcc -O3 -Wall -Wextra "RC5-Dynamic/RC5 Dynamic.c" -o rc5_dynamic
./rc5_dynamic
```