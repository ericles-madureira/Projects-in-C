
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#define TAM_PALAVRA 32
#define RODADAS 12
#define TAM_CHAVE 16
#define TAM_BLOCO 8
#define TOTAL_SUBCHAVES (2 * (RODADAS + 1))

#define CONSTANTE_P 0xB7E15163u
#define CONSTANTE_Q 0x9E3779B9u

#define VERDE    "\x1b[32m"
#define VERMELHO "\x1b[31m"
#define CIANO    "\x1b[36m"
#define AMARELO  "\x1b[33m"
#define RESET    "\x1b[0m"

// Habilita cores ANSI no terminal do Windows.
void ativarCores(void) {
    HANDLE terminal = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD modo;

    if (terminal != INVALID_HANDLE_VALUE &&
        GetConsoleMode(terminal, &modo)) {
        SetConsoleMode(terminal,
                       modo | ENABLE_VIRTUAL_TERMINAL_PROCESSING);
    }
}

// Exibe uma barra de progresso apenas visual.
void animacao(const char *etapa) {
    printf(CIANO "\n%s\n" RESET, etapa);

    for (int i = 0; i <= 20; i++) {
        printf("\r[");

        for (int j = 0; j < 20; j++) {
            printf(j < i ? "#" : "-");
        }

        printf("] %3d%%", i * 5);
        fflush(stdout);
        Sleep(70);
    }

    printf(VERDE " CONCLUIDO!\n" RESET);
}

// Le uma linha completa, descartando entradas excessivas.
// Retorna 1 para sucesso, 0 para entrada longa e -1 para EOF.
int lerLinha(char *destino, size_t capacidade) {
    if (fgets(destino, (int)capacidade, stdin) == NULL) {
        return -1;
    }

    size_t tamanho = strlen(destino);

    if (tamanho > 0 && destino[tamanho - 1] == '\n') {
        destino[tamanho - 1] = '\0';
        return 1;
    }

    // Descarta o restante da linha quando necessario.
    int caractere;
    while ((caractere = getchar()) != '\n' &&
           caractere != EOF) {
    }

    return 0;
}

// Solicita novamente ate receber a quantidade correta de bytes.
int solicitarTexto(char *destino, size_t capacidade,
                   size_t tamanhoEsperado, const char *tipo) {

    while (1) {
        printf(CIANO "\nDigite %s (%zu bytes): " RESET,
               tipo, tamanhoEsperado);

        int leitura = lerLinha(destino, capacidade);

        if (leitura == -1) {
            return 0;
        }

        if (leitura == 0) {
            printf(VERMELHO
                   "Entrada muito longa. Tente novamente.\n"
                   RESET);
            continue;
        }

        size_t tamanho = strlen(destino);

        if (tamanho != tamanhoEsperado) {
            printf(VERMELHO
                   "Erro: esperado %zu bytes, recebido %zu.\n"
                   RESET, tamanhoEsperado, tamanho);
            continue;
        }

        // Aceita apenas caracteres ASCII imprimiveis.
        int valido = 1;

        for (size_t i = 0; i < tamanho; i++) {
            unsigned char c = (unsigned char)destino[i];

            if (c < 32 || c > 126) {
                valido = 0;
                break;
            }
        }

        if (!valido) {
            printf(VERMELHO
                   "Use apenas caracteres ASCII simples.\n"
                   RESET);
            continue;
        }

        printf(VERDE "Entrada aceita!\n" RESET);
        return 1;
    }
}

// Rotacao circular para a esquerda.
uint32_t rotacionarEsquerda(uint32_t valor, uint32_t bits) {
    bits &= 31u;

    return (valor << bits) |
           (valor >> ((32u - bits) & 31u));
}

// Rotacao circular para a direita.
uint32_t rotacionarDireita(uint32_t valor, uint32_t bits) {
    bits &= 31u;

    return (valor >> bits) |
           (valor << ((32u - bits) & 31u));
}

// Converte 4 bytes em uma palavra de 32 bits (little-endian).
uint32_t lerPalavra(const uint8_t *dados) {
    uint32_t valor = 0;

    for (int i = 0; i < 4; i++) {
        valor |= (uint32_t)dados[i] << (8 * i);
    }

    return valor;
}

// Armazena uma palavra de 32 bits em 4 bytes.
void escreverPalavra(uint8_t *dados, uint32_t valor) {
    for (int i = 0; i < 4; i++) {
        dados[i] = (uint8_t)(valor >> (8 * i));
    }
}

// Expande os 16 bytes da chave em 26 subchaves.
void expandirChave(const uint8_t *chave,
                   uint32_t *subchaves) {

    uint32_t palavrasChave[TAM_CHAVE / 4] = {0};
    uint32_t A = 0, B = 0;

    int indiceS = 0;
    int indiceL = 0;

    for (int i = TAM_CHAVE - 1; i >= 0; i--) {
        palavrasChave[i / 4] =
            (palavrasChave[i / 4] << 8) + chave[i];
    }

    subchaves[0] = CONSTANTE_P;

    for (int i = 1; i < TOTAL_SUBCHAVES; i++) {
        subchaves[i] = subchaves[i - 1] + CONSTANTE_Q;
    }

    // Mistura as palavras da chave com as subchaves.
    for (int i = 0; i < 3 * TOTAL_SUBCHAVES; i++) {

        A = subchaves[indiceS] = rotacionarEsquerda(
            subchaves[indiceS] + A + B, 3
        );

        B = palavrasChave[indiceL] = rotacionarEsquerda(
            palavrasChave[indiceL] + A + B, A + B
        );

        indiceS = (indiceS + 1) % TOTAL_SUBCHAVES;
        indiceL = (indiceL + 1) % (TAM_CHAVE / 4);
    }
}

// Criptografa um bloco de 8 bytes usando 12 rodadas.
void criptografar(const uint8_t *mensagem,
                  uint8_t *resultado,
                  const uint32_t *subchaves) {

    uint32_t A = lerPalavra(mensagem);
    uint32_t B = lerPalavra(mensagem + 4);

    A += subchaves[0];
    B += subchaves[1];

    for (int i = 1; i <= RODADAS; i++) {

        A = rotacionarEsquerda(A ^ B, B)
            + subchaves[2 * i];

        B = rotacionarEsquerda(B ^ A, A)
            + subchaves[2 * i + 1];
    }

    escreverPalavra(resultado, A);
    escreverPalavra(resultado + 4, B);
}

// Desfaz as rodadas em ordem inversa.
void descriptografar(const uint8_t *mensagem,
                     uint8_t *resultado,
                     const uint32_t *subchaves) {

    uint32_t A = lerPalavra(mensagem);
    uint32_t B = lerPalavra(mensagem + 4);

    for (int i = RODADAS; i >= 1; i--) {

        B = rotacionarDireita(
            B - subchaves[2 * i + 1], A
        ) ^ A;

        A = rotacionarDireita(
            A - subchaves[2 * i], B
        ) ^ B;
    }

    A -= subchaves[0];
    B -= subchaves[1];

    escreverPalavra(resultado, A);
    escreverPalavra(resultado + 4, B);
}

// Imprime os bytes em formato hexadecimal.
void mostrarHexadecimal(const uint8_t *dados, int tamanho) {
    for (int i = 0; i < tamanho; i++) {
        printf("%02X", dados[i]);
    }
    printf("\n");
}

int main(void) {
    ativarCores();

    char *chave = malloc(TAM_CHAVE + 2);
    char *mensagem = malloc(TAM_BLOCO + 2);

    uint8_t *criptografado = malloc(TAM_BLOCO);
    uint8_t *descriptografado = malloc(TAM_BLOCO);

    uint32_t *subchaves = malloc(
        TOTAL_SUBCHAVES * sizeof(uint32_t)
    );

    if (!chave || !mensagem || !criptografado ||
        !descriptografado || !subchaves) {

        printf(VERMELHO "Erro de memoria!\n" RESET);

        free(chave);
        free(mensagem);
        free(criptografado);
        free(descriptografado);
        free(subchaves);

        return 1;
    }

    int chaveConfigurada = 0;
    int executando = 1;
    char opcao[16];

    while (executando) {

        printf(CIANO
               "\n==================================\n"
               "        ALGORITMO RC5\n"
               "         RC5-32/12/16\n"
               "==================================\n"
               RESET);

        printf("\n1 - Informar / trocar chave");
        printf("\n2 - Criptografar mensagem");
        printf("\n3 - Sair");

        printf(AMARELO "\n\nEscolha: " RESET);

        int leitura = lerLinha(opcao, sizeof(opcao));

        if (leitura == -1) {
            break;
        }

        if (leitura == 0) {
            printf(VERMELHO "\nOpcao invalida!\n" RESET);
            continue;
        }

        switch (opcao[0]) {

            case '1':
                if (strcmp(opcao, "1") != 0) {
                    printf(VERMELHO
                           "\nOpcao invalida!\n" RESET);
                    break;
                }

                if (!solicitarTexto(
                        chave, TAM_CHAVE + 2,
                        TAM_CHAVE, "a chave")) {
                    executando = 0;
                    break;
                }

                animacao("Expandindo a chave RC5...");
                expandirChave((const uint8_t *)chave, subchaves);

                chaveConfigurada = 1;

                printf(VERDE
                       "\nChave configurada com sucesso!\n"
                       RESET);
                break;

            case '2':
                if (strcmp(opcao, "2") != 0) {
                    printf(VERMELHO
                           "\nOpcao invalida!\n" RESET);
                    break;
                }

                if (!chaveConfigurada) {
                    printf(VERMELHO
                           "\nConfigure uma chave primeiro!\n"
                           RESET);
                    break;
                }

                if (!solicitarTexto(
                        mensagem, TAM_BLOCO + 2,
                        TAM_BLOCO, "a mensagem")) {
                    executando = 0;
                    break;
                }

                animacao("Criptografando mensagem...");

                criptografar(
                    (const uint8_t *)mensagem,
                    criptografado, subchaves
                );

                animacao("Descriptografando mensagem...");

                descriptografar(
                    criptografado,
                    descriptografado, subchaves
                );

                printf(CIANO
                       "\n========== RESULTADOS ==========\n"
                       RESET);

                printf("Original:        %s\n", mensagem);

                printf("Criptografado:   ");
                mostrarHexadecimal(criptografado, TAM_BLOCO);

                printf("Descriptografado: %.*s\n",
                       TAM_BLOCO,
                       (const char *)descriptografado);

                if (memcmp(mensagem, descriptografado,
                           TAM_BLOCO) == 0) {
                    printf(VERDE
                           "\n[OK] Mensagem recuperada!\n"
                           RESET);
                } else {
                    printf(VERMELHO
                           "\n[ERRO] Verificacao falhou!\n"
                           RESET);
                }
                break;

            case '3':
                if (strcmp(opcao, "3") != 0) {
                    printf(VERMELHO
                           "\nOpcao invalida!\n" RESET);
                    break;
                }

                printf(AMARELO
                       "\nEncerrando programa...\n"
                       RESET);
                executando = 0;
                break;

            default:
                printf(VERMELHO
                       "\nOpcao invalida! Tente novamente.\n"
                       RESET);
        }
    }

    // Libera os blocos alocados dinamicamente.
    free(chave);
    free(mensagem);
    free(criptografado);
    free(descriptografado);
    free(subchaves);

    return 0;
}
