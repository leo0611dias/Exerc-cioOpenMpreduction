#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <iomanip>

int main() {
    // TechNova Corporation - Estrutura organizacional
    const int DEPARTAMENTOS = 8;
    const int FUNCIONARIOS_POR_DEPT = 62500; // Total: 500,000 funcionários
    const int N = DEPARTAMENTOS * FUNCIONARIOS_POR_DEPT;
    
    std::vector<std::string> nomes_departamentos = {
        "Engenharia de Software", "Cloud Computing", "Data Science",
        "Cybersecurity", "DevOps", "AI Research", 
        "Mobile Development", "Quality Assurance"
    };
    
    std::vector<double> salarios(N);

    // Inicialização paralela com salários realistas por departamento
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        int dept_index = i / FUNCIONARIOS_POR_DEPT;
        int func_index = i % FUNCIONARIOS_POR_DEPT;
        
        double salario_base;
        
        // Salário base por departamento (em USD)
        switch(dept_index) {
            case 0: // Engenharia de Software
                salario_base = 120000.0; break;
            case 1: // Cloud Computing
                salario_base = 130000.0; break;
            case 2: // Data Science
                salario_base = 125000.0; break;
            case 3: // Cybersecurity
                salario_base = 135000.0; break;
            case 4: // DevOps
                salario_base = 128000.0; break;
            case 5: // AI Research
                salario_base = 150000.0; break;
            case 6: // Mobile Development
                salario_base = 115000.0; break;
            case 7: // Quality Assurance
                salario_base = 95000.0; break;
            default:
                salario_base = 100000.0;
        }
        
        // Variação baseada na senioridade (simulada pelo índice do funcionário)
        double senioridade = 1.0 + (func_index % 10) * 0.1; // 1.0 to 2.0
        double performance = 0.8 + (func_index % 20) * 0.02; // 0.8 to 1.2
        
        salarios[i] = salario_base * senioridade * performance;
    }

    // ---------------------------
    // 1) MÉDIA POPULACIONAL (μ)
    // ---------------------------
    double soma = 0.0;
    #pragma omp parallel for reduction(+:soma)
    for (int i = 0; i < N; ++i) {
        soma += salarios[i];
    }
    const double media_populacional = soma / static_cast<double>(N);

    // ----------------------------------------------------
    // 2) VARIÂNCIA POPULACIONAL (σ²) 
    //     σ² = (1/N) * Σ (xᵢ − μ)²
    // ----------------------------------------------------
    double soma_quadrados_desvios = 0.0;
    #pragma omp parallel for reduction(+:soma_quadrados_desvios)
    for (int i = 0; i < N; ++i) {
        const double desvio = salarios[i] - media_populacional;
        soma_quadrados_desvios += desvio * desvio;
    }
    double variancia_populacional = soma_quadrados_desvios / static_cast<double>(N);

    // Correção para imprecisões numéricas
    if (variancia_populacional < 0.0 && std::fabs(variancia_populacional) < 1e-12) {
        variancia_populacional = 0.0;
    }
    const double desvio_padrao_populacional = std::sqrt(variancia_populacional);

    // ----------------------------------------------------
    // 3) VARIÂNCIA AMOSTRAL (s²) para comparação
    //     s² = (1/(N-1)) * Σ (xᵢ − μ)²
    // ----------------------------------------------------
    double variancia_amostral = soma_quadrados_desvios / static_cast<double>(N - 1);
    const double desvio_padrao_amostral = std::sqrt(variancia_amostral);

    // ----------------------------------------------------
    // 4) ESTATÍSTICAS ADICIONAIS
    // ----------------------------------------------------
    double salario_maximo = 0.0;
    double salario_minimo = std::numeric_limits<double>::max();
    double soma_log_salarios = 0.0;
    
    #pragma omp parallel for reduction(max:salario_maximo) \
                             reduction(min:salario_minimo) \
                             reduction(+:soma_log_salarios)
    for (int i = 0; i < N; ++i) {
        if (salarios[i] > salario_maximo) salario_maximo = salarios[i];
        if (salarios[i] < salario_minimo) salario_minimo = salarios[i];
        soma_log_salarios += std::log(salarios[i]);
    }
    
    const double media_geometrica = std::exp(soma_log_salarios / N);

    // ----------------------------------------------------
    // APRESENTAÇÃO DOS RESULTADOS
    // ----------------------------------------------------
    std::cout << "============================================================" << std::endl;
    std::cout << "TECHNOVA CORPORATION - ANÁLISE SALARIAL POPULACIONAL" << std::endl;
    std::cout << "============================================================" << std::endl;
    std::cout << "Departamentos: " << DEPARTAMENTOS << std::endl;
    for (const auto& dept : nomes_departamentos) {
        std::cout << "  - " << dept << std::endl;
    }
    std::cout << "Total de funcionários: " << N << std::endl;
    std::cout << "------------------------------------------------------------" << std::endl;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "ESTATÍSTICAS POPULACIONAIS (USD):" << std::endl;
    std::cout << "Média Populacional (μ): $" << media_populacional << std::endl;
    std::cout << "Variância Populacional (σ²): $" << variancia_populacional << std::endl;
    std::cout << "Desvio Padrão Populacional (σ): $" << desvio_padrao_populacional << std::endl;
    std::cout << "Média Geométrica: $" << media_geometrica << std::endl;
    
    std::cout << "\nESTATÍSTICAS AMOSTRAIS (para comparação):" << std::endl;
    std::cout << "Variância Amostral (s²): $" << variancia_amostral << std::endl;
    std::cout << "Desvio Padrão Amostral (s): $" << desvio_padrao_amostral << std::endl;
    
    std::cout << "\nESTATÍSTICAS DESCRITIVAS:" << std::endl;
    std::cout << "Salário Mínimo: $" << salario_minimo << std::endl;
    std::cout << "Salário Máximo: $" << salario_maximo << std::endl;
    std::cout << "Amplitude: $" << (salario_maximo - salario_minimo) << std::endl;
    std::cout << "Coeficiente de Variação: " << std::setprecision(4) 
              << (desvio_padrao_populacional / media_populacional) * 100 << "%" << std::endl;

    return 0;
}
