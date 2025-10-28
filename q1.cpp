/*-------------------------------------------------------------------------------------------------------------------------
 * Author    : Leonardo Dias dos Passos Brito
 * Course    : PROGRAMAÇÃO PARALELA
 * Objective : Exercício OpenMp reduction
 * Semester  : 2025/2
 * Professor : Rodrigo Gonçalves Pinto
 ------------------------------------------------------------------------------------------------------------------------*/
#include <iostream>
#include <vector>
#include <cmath>
#include <limits>
#include <string>
#include <omp.h>

// Função auxiliar para gerar dados de exemplo
std::vector<double> gerar_dados_aleatorios(int tamanho) {
    std::vector<double> dados(tamanho);
    #pragma omp parallel for
    for (int i = 0; i < tamanho; ++i) {
        dados[i] = static_cast<double>(i + 1) * 1.5;
    }
    return dados;
}

int main() {
    const int N = 1000000;
    std::vector<double> dados = gerar_dados_aleatorios(N);
    
    std::cout << "=== DEMONSTRAÇÃO DAS OPERAÇÕES DE REDUCTION NO OpenMP ===" << std::endl;
    std::cout << "Tamanho do conjunto de dados: " << N << std::endl << std::endl;

    // 1. REDUCTION COM SOMA (+)
    double soma = 0.0;
    #pragma omp parallel for reduction(+:soma)
    for (int i = 0; i < N; ++i) {
        soma += dados[i];
    }
    std::cout << "1. REDUCTION COM SOMA (+):" << std::endl;
    std::cout << "   Soma total: " << soma << std::endl;
    std::cout << "   Média: " << soma / N << std::endl << std::endl;

    // 2. REDUCTION COM MULTIPLICAÇÃO (*)
    double produto = 1.0;
    // Usamos um subconjunto menor para evitar overflow
    const int N_mult = 100;
    #pragma omp parallel for reduction(*:produto)
    for (int i = 0; i < N_mult; ++i) {
        produto *= (dados[i] / 1000.0); // Escalamos para evitar overflow
    }
    std::cout << "2. REDUCTION COM MULTIPLICAÇÃO (*):" << std::endl;
    std::cout << "   Produto dos primeiros " << N_mult << " elementos (escalados): " << produto << std::endl << std::endl;

    // 3. REDUCTION COM MÁXIMO (max)
    double maximo = std::numeric_limits<double>::lowest();
    #pragma omp parallel for reduction(max:maximo)
    for (int i = 0; i < N; ++i) {
        if (dados[i] > maximo) {
            maximo = dados[i];
        }
    }
    std::cout << "3. REDUCTION COM MÁXIMO (max):" << std::endl;
    std::cout << "   Valor máximo: " << maximo << std::endl << std::endl;

    // 4. REDUCTION COM MÍNIMO (min)
    double minimo = std::numeric_limits<double>::max();
    #pragma omp parallel for reduction(min:minimo)
    for (int i = 0; i < N; ++i) {
        if (dados[i] < minimo) {
            minimo = dados[i];
        }
    }
    std::cout << "4. REDUCTION COM MÍNIMO (min):" << std::endl;
    std::cout << "   Valor mínimo: " << minimo << std::endl << std::endl;

    // 5. REDUCTION COM OPERAÇÕES LÓGICAS - AND (&&)
    bool todos_positivos = true;
    #pragma omp parallel for reduction(&&:todos_positivos)
    for (int i = 0; i < N; ++i) {
        todos_positivos = todos_positivos && (dados[i] > 0);
    }
    std::cout << "5. REDUCTION COM AND LÓGICO (&&):" << std::endl;
    std::cout << "   Todos os elementos são positivos? " << (todos_positivos ? "Sim" : "Não") << std::endl << std::endl;

    // 6. REDUCTION COM OPERAÇÕES LÓGICAS - OR (||)
    bool existe_negativo = false;
    #pragma omp parallel for reduction(||:existe_negativo)
    for (int i = 0; i < N; ++i) {
        existe_negativo = existe_negativo || (dados[i] < 0);
    }
    std::cout << "6. REDUCTION COM OR LÓGICO (||):" << std::endl;
    std::cout << "   Existe algum elemento negativo? " << (existe_negativo ? "Sim" : "Não") << std::endl << std::endl;

    // 7. REDUCTION COM BITWISE AND (&) - exemplo com inteiros
    int bitmask_and = ~0; // Todos os bits 1
    std::vector<int> inteiros(N);
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        inteiros[i] = i + 1;
    }
    
    #pragma omp parallel for reduction(&:bitmask_and)
    for (int i = 0; i < N; ++i) {
        bitmask_and &= inteiros[i];
    }
    std::cout << "7. REDUCTION COM BITWISE AND (&):" << std::endl;
    std::cout << "   Resultado do AND bit a bit: " << bitmask_and << std::endl << std::endl;

    // 8. REDUCTION COM BITWISE OR (|)
    int bitmask_or = 0;
    #pragma omp parallel for reduction(|:bitmask_or)
    for (int i = 0; i < N; ++i) {
        bitmask_or |= inteiros[i];
    }
    std::cout << "8. REDUCTION COM BITWISE OR (|):" << std::endl;
    std::cout << "   Resultado do OR bit a bit: " << bitmask_or << std::endl << std::endl;

    // 9. REDUCTION COM BITWISE XOR (^)
    int bitwise_xor = 0;
    #pragma omp parallel for reduction(^:bitwise_xor)
    for (int i = 0; i < N; ++i) {
        bitwise_xor ^= inteiros[i];
    }
    std::cout << "9. REDUCTION COM BITWISE XOR (^):" << std::endl;
    std::cout << "   Resultado do XOR bit a bit: " << bitwise_xor << std::endl << std::endl;

    // 10. EXEMPLO PRÁTICO: CÁLCULO DE VARIÂNCIA E DESVIO PADRÃO
    double media = soma / N;
    double variancia = 0.0;
    
    #pragma omp parallel for reduction(+:variancia)
    for (int i = 0; i < N; ++i) {
        double diff = dados[i] - media;
        variancia += diff * diff;
    }
    variancia /= (N - 1); // Variância amostral
    double desvio_padrao = std::sqrt(variancia);
    
    std::cout << "10. EXEMPLO PRÁTICO: CÁLCULO ESTATÍSTICO" << std::endl;
    std::cout << "    Média: " << media << std::endl;
    std::cout << "    Variância: " << variancia << std::endl;
    std::cout << "    Desvio Padrão: " << desvio_padrao << std::endl << std::endl;

    // 11. MULTIPLAS REDUCTIONS EM UM MESMO LOOP
    double soma_multipla = 0.0;
    double max_multiplo = std::numeric_limits<double>::lowest();
    double min_multiplo = std::numeric_limits<double>::max();
    
    #pragma omp parallel for reduction(+:soma_multipla) reduction(max:max_multiplo) reduction(min:min_multiplo)
    for (int i = 0; i < N; ++i) {
        soma_multipla += dados[i];
        if (dados[i] > max_multiplo) max_multiplo = dados[i];
        if (dados[i] < min_multiplo) min_multiplo = dados[i];
    }
    
    std::cout << "11. MÚLTIPLAS REDUCTIONS NO MESMO LOOP:" << std::endl;
    std::cout << "    Soma: " << soma_multipla << std::endl;
    std::cout << "    Máximo: " << max_multiplo << std::endl;
    std::cout << "    Mínimo: " << min_multiplo << std::endl;
    std::cout << "    Amplitude: " << (max_multiplo - min_multiplo) << std::endl << std::endl;

    // 12. COMPARAÇÃO DE DESEMPENHO: COM E SEM REDUCTION
    std::cout << "12. COMPARAÇÃO DE DESEMPENHO:" << std::endl;
    
    // Sem reduction (usando critical)
    double inicio = omp_get_wtime();
    double soma_sem_reduction = 0.0;
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        #pragma omp critical
        soma_sem_reduction += dados[i];
    }
    double tempo_sem_reduction = omp_get_wtime() - inicio;
    
    // Com reduction
    inicio = omp_get_wtime();
    double soma_com_reduction = 0.0;
    #pragma omp parallel for reduction(+:soma_com_reduction)
    for (int i = 0; i < N; ++i) {
        soma_com_reduction += dados[i];
    }
    double tempo_com_reduction = omp_get_wtime() - inicio;
    
    std::cout << "    Tempo sem reduction (critical): " << tempo_sem_reduction << " segundos" << std::endl;
    std::cout << "    Tempo com reduction: " << tempo_com_reduction << " segundos" << std::endl;
    std::cout << "    Speedup: " << (tempo_sem_reduction / tempo_com_reduction) << "x" << std::endl;
    std::cout << "    Resultados são iguais? " << (soma_sem_reduction == soma_com_reduction ? "Sim" : "Não") << std::endl;

    return 0;
}
