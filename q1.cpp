/*-------------------------------------------------------------------------------------------------------------------------
 * Author    : Leonardo Dias dos Passos Brito
 * Course    : PROGRAMAÇÃO PARALELA
 * Objective : Exercício OpenMp reduction
 * Semester  : 2025/2
 * Professor : Rodrigo Gonçalves Pinto
 -------------------------------------------------------------------------------------------------------------------------*/

#include <iostream>
#include <vector>
#include <cmath>
#include <omp.h>
#include <iomanip>
#include <random>

int main() {
    const int TOTAL_FUNCIONARIOS = 2000000;
    std::vector<double> salarios(TOTAL_FUNCIONARIOS);
    
    // BigTech fictícia: TechNova Americas
    std::vector<std::string> departamentos = {
        "Engenharia de Software", "Vendas e Marketing", "Recursos Humanos", 
        "Finanças", "Operações", "Pesquisa e Desenvolvimento"
    };
    
    std::vector<std::string> cargos = {
        "Engenheiro Sênior", "Engenheiro Pleno", "Engenheiro Júnior",
        "Analista", "Estagiário", "Gerente", "Diretor", "Especialista"
    };
    
    std::vector<std::string> localizacoes = {
        "EUA", "Brasil", "México", "Canadá", "Argentina", "Chile", "Colômbia"
    };

    // Inicialização paralela com salários realistas baseados em localização e cargo
    #pragma omp parallel for
    for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
        int dept_index = i % departamentos.size();
        int cargo_index = i % cargos.size();
        int loc_index = i % localizacoes.size();
        
        double salario_base;
        
        // Salários base por localização (em USD)
        if (localizacoes[loc_index] == "EUA") {
            salario_base = 80000.0;
        } else if (localizacoes[loc_index] == "Canadá") {
            salario_base = 70000.0;
        } else if (localizacoes[loc_index] == "Brasil") {
            salario_base = 40000.0;
        } else {
            salario_base = 35000.0; // Outros países latino-americanos
        }
        
        // Ajuste por cargo
        if (cargos[cargo_index] == "Estagiário") {
            salario_base *= 0.4;
        } else if (cargos[cargo_index] == "Engenheiro Júnior") {
            salario_base *= 0.7;
        } else if (cargos[cargo_index] == "Engenheiro Pleno") {
            salario_base *= 1.0;
        } else if (cargos[cargo_index] == "Engenheiro Sênior") {
            salario_base *= 1.5;
        } else if (cargos[cargo_index] == "Analista") {
            salario_base *= 0.9;
        } else if (cargos[cargo_index] == "Gerente") {
            salario_base *= 1.8;
        } else if (cargos[cargo_index] == "Diretor") {
            salario_base *= 2.5;
        } else if (cargos[cargo_index] == "Especialista") {
            salario_base *= 1.3;
        }
        
        // Variação individual baseada no ID do funcionário
        double variacao = 1.0 + ((i % 100) - 50) * 0.01; // Variação de ±50%
        salarios[i] = salario_base * variacao;
    }

    // 1. CÁLCULO DA MÉDIA usando reduction
    double soma = 0.0;
    #pragma omp parallel for reduction(+:soma)
    for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
        soma += salarios[i];
    }
    double media = soma / TOTAL_FUNCIONARIOS;

    // 2. CÁLCULO DA VARIÂNCIA AMOSTRAL (dividindo por N-1)
    double soma_quadrados = 0.0;
    #pragma omp parallel for reduction(+:soma_quadrados)
    for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
        double diferenca = salarios[i] - media;
        soma_quadrados += diferenca * diferenca;
    }
    
    double variancia_amostral = soma_quadrados / (TOTAL_FUNCIONARIOS - 1);
    double desvio_padrao_amostral = std::sqrt(variancia_amostral);

    // 3. CÁLCULO DE ESTATÍSTICAS ADICIONAIS
    double salario_maximo = 0.0;
    double salario_minimo = std::numeric_limits<double>::max();
    
    #pragma omp parallel for reduction(max:salario_maximo) reduction(min:salario_minimo)
    for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
        if (salarios[i] > salario_maximo) salario_maximo = salarios[i];
        if (salarios[i] < salario_minimo) salario_minimo = salarios[i];
    }

    // APRESENTAÇÃO DOS RESULTADOS
    std::cout << "======================================================" << std::endl;
    std::cout << "TECHNOVA AMERICAS - RELATÓRIO SALARIAL" << std::endl;
    std::cout << "======================================================" << std::endl;
    std::cout << "Total de funcionários: " << TOTAL_FUNCIONARIOS << std::endl;
    std::cout << "Departamentos: " << departamentos.size() << std::endl;
    std::cout << "Cargos: " << cargos.size() << std::endl;
    std::cout << "Localizações: " << localizacoes.size() << " países" << std::endl;
    std::cout << "------------------------------------------------------" << std::endl;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "ESTATÍSTICAS SALARIAIS (USD):" << std::endl;
    std::cout << "Média: $" << media << std::endl;
    std::cout << "Desvio Padrão Amostral: $" << desvio_padrao_amostral << std::endl;
    std::cout << "Variância Amostral: $" << variancia_amostral << std::endl;
    std::cout << "Salário Mínimo: $" << salario_minimo << std::endl;
    std::cout << "Salário Máximo: $" << salario_maximo << std::endl;
    std::cout << "Coeficiente de Variação: " << std::setprecision(4) 
              << (desvio_padrao_amostral / media) * 100 << "%" << std::endl;

    return 0;
}
