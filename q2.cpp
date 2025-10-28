#include <iostream>
#include <vector>
#include <omp.h>

int main() {
    const int N = 200000;
    std::vector<double> salarios(N);
    
    // BigTech: TechNova Solutions
    std::vector<std::string> departamentos = {
        "Engenharia", "Vendas", "Marketing", "RH", "Finanças", "Operações"
    };

    // Preenche com dados realistas
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        int dept_index = i % departamentos.size();
        
        // Salários base por departamento
        if (departamentos[dept_index] == "Engenharia") {
            salarios[i] = 8000.0 + (i % 50) * 200.0;
        } else if (departamentos[dept_index] == "Vendas") {
            salarios[i] = 5000.0 + (i % 40) * 150.0;
        } else if (departamentos[dept_index] == "Marketing") {
            salarios[i] = 4500.0 + (i % 35) * 120.0;
        } else if (departamentos[dept_index] == "RH") {
            salarios[i] = 4000.0 + (i % 30) * 100.0;
        } else if (departamentos[dept_index] == "Finanças") {
            salarios[i] = 7000.0 + (i % 45) * 180.0;
        } else { // Operações
            salarios[i] = 3500.0 + (i % 25) * 80.0;
        }
    }

    // Injeta dados problemáticos para auditoria
    salarios[1000] = 1400.0;      // Viola Regra 1 (abaixo do piso)
    salarios[20000] = -50.0;      // Viola Regra 3 (salário negativo)
    salarios[50000] = 25000.0;    // Viola Regra 2 (acima do teto)
    salarios[75000] = 0.0;        // Viola Regra 3 (salário zero)
    salarios[100000] = 1499.0;    // Viola Regra 1 (abaixo do piso)

    // Variáveis para auditoria
    bool piso_violado = false;     // Existe salário abaixo de R$ 1500,00?
    bool teto_violado = false;     // Existe salário acima de R$ 20000,00?
    bool dados_validos = true;     // Todos os salários são positivos?
    bool faixa_ideal = true;       // Todos entre R$ 3000 e R$ 15000?

    const double PISO_SALARIAL = 1500.0;
    const double TETO_SALARIAL = 20000.0;
    const double MIN_IDEAL = 3000.0;
    const double MAX_IDEAL = 15000.0;

    // Auditoria paralela com múltiplas reduções lógicas
    #pragma omp parallel for \
        reduction(||:piso_violado, teto_violado) \
        reduction(&&:dados_validos, faixa_ideal)
    for (int i = 0; i < N; ++i) {
        double s = salarios[i];

        // Regra 1: Piso salarial
        if (s < PISO_SALARIAL) {
            piso_violado = true;
        }

        // Regra 2: Teto salarial  
        if (s > TETO_SALARIAL) {
            teto_violado = true;
        }

        // Regra 3: Dados válidos (positivos)
        if (s <= 0) {
            dados_validos = false;
        }

        // Regra 4: Faixa salarial ideal
        if (s < MIN_IDEAL || s > MAX_IDEAL) {
            faixa_ideal = false;
        }
    }

    // Relatório de Auditoria
    std::cout << "==================================================" << std::endl;
    std::cout << "TECHNOVA SOLUTIONS - RELATÓRIO DE AUDITORIA" << std::endl;
    std::cout << "==================================================" << std::endl;
    std::cout << "Total de registros auditados: " << N << std::endl;
    std::cout << "Departamentos: Engenharia, Vendas, Marketing, RH, Finanças, Operações" << std::endl;
    std::cout << "--------------------------------------------------" << std::endl;

    if (piso_violado) {
        std::cout << "[ALERTA] Regra 1: Encontrado salário abaixo do piso de R$ " << PISO_SALARIAL << std::endl;
    } else {
        std::cout << "[OK] Regra 1: Nenhum salário abaixo do piso encontrado" << std::endl;
    }

    if (teto_violado) {
        std::cout << "[ALERTA] Regra 2: Encontrado salário acima do teto de R$ " << TETO_SALARIAL << std::endl;
    } else {
        std::cout << "[OK] Regra 2: Nenhum salário acima do teto encontrado" << std::endl;
    }

    if (!dados_validos) {
        std::cout << "[CRÍTICO] Regra 3: Encontrados salários inválidos (≤ 0)" << std::endl;
    } else {
        std::cout << "[OK] Regra 3: Todos os salários são válidos (> 0)" << std::endl;
    }

    if (faixa_ideal) {
        std::cout << "[EXCELENTE] Regra 4: Todos os salários na faixa ideal (R$ " 
                  << MIN_IDEAL << " - R$ " << MAX_IDEAL << ")" << std::endl;
    } else {
        std::cout << "[ATENÇÃO] Regra 4: Alguns salários fora da faixa ideal" << std::endl;
    }

    // Estatísticas adicionais
    int count_abaixo_piso = 0;
    int count_acima_teto = 0;
    int count_invalidos = 0;
    
    #pragma omp parallel for reduction(+:count_abaixo_piso, count_acima_teto, count_invalidos)
    for (int i = 0; i < N; ++i) {
        if (salarios[i] < PISO_SALARIAL) count_abaixo_piso++;
        if (salarios[i] > TETO_SALARIAL) count_acima_teto++;
        if (salarios[i] <= 0) count_invalidos++;
    }

    std::cout << "--------------------------------------------------" << std::endl;
    std::cout << "DETALHAMENTO:" << std::endl;
    std::cout << "Salários abaixo do piso: " << count_abaixo_piso << std::endl;
    std::cout << "Salários acima do teto: " << count_acima_teto << std::endl;
    std::cout << "Salários inválidos: " << count_invalidos << std::endl;

    return 0;
}
