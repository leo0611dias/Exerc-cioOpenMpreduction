#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include <random>

// Estrutura para acumular estatísticas online
struct WelfordAccumulator {
    double mean;
    double M2;
    long long count;
    
    WelfordAccumulator() : mean(0.0), M2(0.0), count(0) {}
};

// Função para combinar acumuladores (necessária para reduction customizada)
void welford_combine(WelfordAccumulator& a, const WelfordAccumulator& b) {
    if (b.count == 0) return;
    if (a.count == 0) {
        a = b;
        return;
    }
    
    long long total_count = a.count + b.count;
    double delta = b.mean - a.mean;
    
    a.M2 += b.M2 + delta * delta * a.count * b.count / total_count;
    a.mean = (a.count * a.mean + b.count * b.mean) / total_count;
    a.count = total_count;
}

// Função para atualizar acumulador com novo valor
void welford_update(WelfordAccumulator& acc, double x) {
    acc.count++;
    double delta = x - acc.mean;
    acc.mean += delta / acc.count;
    double delta2 = x - acc.mean;
    acc.M2 += delta * delta2;
}

int main() {
    const int N = 1000000;
    std::vector<double> salarios(N);
    
    // Gerador de números aleatórios
    std::random_device rd;
    std::mt19937 gen(rd());
    std::normal_distribution<double> dist(5000.0, 1500.0);
    
    // Inicialização dos salários com distribuição normal
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        salarios[i] = std::max(1000.0, dist(gen)); // Salário mínimo de R$ 1000
    }
    
    std::cout << "=== CÁLCULO DE VARIÂNCIA - COMPARAÇÃO DE MÉTODOS ===" << std::endl;
    std::cout << "Tamanho do conjunto: " << N << " salários\n" << std::endl;
    
    // MÉTODO 1: Duas passadas (tradicional)
    std::cout << "1. MÉTODO TRADICIONAL (DUAS PASSADAS):" << std::endl;
    double inicio = omp_get_wtime();
    
    // Primeira passada: calcular média
    double soma = 0.0;
    #pragma omp parallel for reduction(+:soma)
    for (int i = 0; i < N; ++i) {
        soma += salarios[i];
    }
    double media = soma / N;
    
    // Segunda passada: calcular variância
    double soma_quadrados = 0.0;
    #pragma omp parallel for reduction(+:soma_quadrados)
    for (int i = 0; i < N; ++i) {
        double diff = salarios[i] - media;
        soma_quadrados += diff * diff;
    }
    
    double variancia_pop_2pass = soma_quadrados / N;
    double variancia_amostral_2pass = soma_quadrados / (N - 1);
    double tempo_2pass = omp_get_wtime() - inicio;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "   Média: R$ " << media << std::endl;
    std::cout << "   Variância populacional: R$ " << variancia_pop_2pass << std::endl;
    std::cout << "   Variância amostral: R$ " << variancia_amostral_2pass << std::endl;
    std::cout << "   Desvio padrão: R$ " << std::sqrt(variancia_pop_2pass) << std::endl;
    std::cout << "   Tempo: " << std::setprecision(4) << tempo_2pass << " segundos" << std::endl << std::endl;
    
    // MÉTODO 2: Uma passada com múltiplas reductions
    std::cout << "2. MÉTODO UMA PASSADA (MÚLTIPLAS REDUCTIONS):" << std::endl;
    inicio = omp_get_wtime();
    
    double soma1 = 0.0, soma2 = 0.0;
    #pragma omp parallel for reduction(+:soma1) reduction(+:soma2)
    for (int i = 0; i < N; ++i) {
        soma1 += salarios[i];
        soma2 += salarios[i] * salarios[i];
    }
    
    double media_1pass = soma1 / N;
    double variancia_pop_1pass = (soma2 - (soma1 * soma1) / N) / N;
    double variancia_amostral_1pass = (soma2 - (soma1 * soma1) / N) / (N - 1);
    double tempo_1pass = omp_get_wtime() - inicio;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "   Média: R$ " << media_1pass << std::endl;
    std::cout << "   Variância populacional: R$ " << variancia_pop_1pass << std::endl;
    std::cout << "   Variância amostral: R$ " << variancia_amostral_1pass << std::endl;
    std::cout << "   Desvio padrão: R$ " << std::sqrt(variancia_pop_1pass) << std::endl;
    std::cout << "   Tempo: " << std::setprecision(4) << tempo_1pass << " segundos" << std::endl << std::endl;
    
    // MÉTODO 3: Algoritmo de Welford (online, numericamente estável)
    std::cout << "3. ALGORITMO DE WELFORD (ONLINE, UMA PASSADA):" << std::endl;
    inicio = omp_get_wtime();
    
    // Para paralelizar Welford, precisamos de reduction customizada
    // Vamos usar uma abordagem com seções críticas para demonstração
    WelfordAccumulator global_acc;
    
    #pragma omp parallel
    {
        WelfordAccumulator local_acc;
        
        #pragma omp for
        for (int i = 0; i < N; ++i) {
            welford_update(local_acc, salarios[i]);
        }
        
        #pragma omp critical
        {
            welford_combine(global_acc, local_acc);
        }
    }
    
    double variancia_pop_welford = global_acc.M2 / global_acc.count;
    double variancia_amostral_welford = global_acc.M2 / (global_acc.count - 1);
    double tempo_welford = omp_get_wtime() - inicio;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "   Média: R$ " << global_acc.mean << std::endl;
    std::cout << "   Variância populacional: R$ " << variancia_pop_welford << std::endl;
    std::cout << "   Variância amostral: R$ " << variancia_amostral_welford << std::endl;
    std::cout << "   Desvio padrão: R$ " << std::sqrt(variancia_pop_welford) << std::endl;
    std::cout << "   Tempo: " << std::setprecision(4) << tempo_welford << " segundos" << std::endl << std::endl;
    
    // COMPARAÇÃO DOS MÉTODOS
    std::cout << "=== COMPARAÇÃO FINAL ===" << std::endl;
    std::cout << "Diferença na média: R$ " << std::fabs(media - global_acc.mean) << std::endl;
    std::cout << "Diferença na variância: R$ " << std::fabs(variancia_pop_2pass - variancia_pop_welford) << std::endl;
    std::cout << std::endl;
    
    std::cout << "DESEMPENHO RELATIVO:" << std::endl;
    std::cout << "2 passadas / 1 passada: " << (tempo_2pass / tempo_1pass) << "x" << std::endl;
    std::cout << "2 passadas / Welford: " << (tempo_2pass / tempo_welford) << "x" << std::endl;
    std::cout << "1 passada / Welford: " << (tempo_1pass / tempo_welford) << "x" << std::endl;
    
    // ANÁLISE DE PRECISÃO NUMÉRICA
    std::cout << std::endl << "PRECISÃO NUMÉRICA:" << std::endl;
    std::cout << std::scientific << std::setprecision(6);
    std::cout << "Erro relativo 1-passada: " << std::fabs(variancia_pop_1pass - variancia_pop_2pass) / variancia_pop_2pass << std::endl;
    std::cout << "Erro relativo Welford: " << std::fabs(variancia_pop_welford - variancia_pop_2pass) / variancia_pop_2pass << std::endl;
    
    return 0;
}

// Implementação alternativa com user-defined reduction (C++17)
#pragma omp declare reduction( \
    welford_combine_reduction : \
    WelfordAccumulator : \
    welford_combine(omp_out, omp_in) \
) initializer(omp_priv = WelfordAccumulator())

void exemplo_com_user_defined_reduction() {
    const int N = 100000;
    std::vector<double> dados(N, 1.0);
    
    WelfordAccumulator acc;
    
    // Esta versão requer suporte completo a user-defined reduction
    #pragma omp parallel for reduction(welford_combine_reduction:acc)
    for (int i = 0; i < N; ++i) {
        welford_update(acc, dados[i]);
    }
    
    std::cout << "User-defined reduction - Média: " << acc.mean << std::endl;
}
