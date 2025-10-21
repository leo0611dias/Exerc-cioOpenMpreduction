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
#include <algorithm>
#include <string>

// Exercício 1: Desvio Padrão Amostral
void exercicio1() {
    std::cout << "=== EXERCÍCIO 1: DESVIO PADRÃO AMOSTRAL ===" << std::endl;
    
    const int TOTAL_FUNCIONARIOS = 2000000;
    std::vector<double> salarios(TOTAL_FUNCIONARIOS);
    
    // Inicialização com salários realistas para uma BigTech
    #pragma omp parallel for
    for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
        // Salários variando de $30,000 a $250,000 USD
        double base = 30000.0 + (i % 220) * 1000.0;
        double bonus = (i % 13) * 500.0;
        salarios[i] = base + bonus;
    }
    
    // 1. Cálculo da média
    double soma = 0.0;
    #pragma omp parallel for reduction(+:soma)
    for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
        soma += salarios[i];
    }
    double media = soma / TOTAL_FUNCIONARIOS;
    
    // 2. Cálculo da variância amostral (dividindo por N-1)
    double soma_quadrados = 0.0;
    #pragma omp parallel for reduction(+:soma_quadrados)
    for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
        double diferenca = salarios[i] - media;
        soma_quadrados += diferenca * diferenca;
    }
    double variancia_amostral = soma_quadrados / (TOTAL_FUNCIONARIOS - 1);
    double desvio_padrao_amostral = std::sqrt(variancia_amostral);
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "TechNova Solutions - Análise Salarial" << std::endl;
    std::cout << "Total de funcionários: " << TOTAL_FUNCIONARIOS << std::endl;
    std::cout << "Média salarial: $" << media << std::endl;
    std::cout << "Variância amostral: $" << variancia_amostral << std::endl;
    std::cout << "Desvio padrão amostral: $" << desvio_padrao_amostral << std::endl;
    std::cout << std::endl;
}

// Exercício 2: Múltiplas Reduções Aritméticas
void exercicio2() {
    std::cout << "=== EXERCÍCIO 2: MÚLTIPLAS REDUÇÕES ARITMÉTICAS ===" << std::endl;
    
    const int DEPARTAMENTOS = 5;
    const int FUNCIONARIOS_POR_DEPT = 1000;
    const int TOTAL = DEPARTAMENTOS * FUNCIONARIOS_POR_DEPT;
    
    std::vector<double> vendas(TOTAL);
    std::vector<double> custos(TOTAL);
    
    // Inicialização com dados de vendas e custos
    #pragma omp parallel for
    for (int i = 0; i < TOTAL; ++i) {
        vendas[i] = 50000.0 + (i % 50) * 1000.0;  // $50,000 to $100,000
        custos[i] = 10000.0 + (i % 20) * 500.0;   // $10,000 to $20,000
    }
    
    double total_vendas = 0.0, total_custos = 0.0;
    double max_venda = 0.0, min_custo = std::numeric_limits<double>::max();
    
    #pragma omp parallel for reduction(+:total_vendas,total_custos) \
                             reduction(max:max_venda) reduction(min:min_custo)
    for (int i = 0; i < TOTAL; ++i) {
        total_vendas += vendas[i];
        total_custos += custos[i];
        
        if (vendas[i] > max_venda) max_venda = vendas[i];
        if (custos[i] < min_custo) min_custo = custos[i];
    }
    
    double lucro_total = total_vendas - total_custos;
    double margem_lucro = (lucro_total / total_vendas) * 100.0;
    
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Análise Financeira - TechNova Solutions" << std::endl;
    std::cout << "Total de Vendas: $" << total_vendas << std::endl;
    std::cout << "Total de Custos: $" << total_custos << std::endl;
    std::cout << "Lucro Total: $" << lucro_total << std::endl;
    std::cout << "Margem de Lucro: " << std::setprecision(1) << margem_lucro << "%" << std::endl;
    std::cout << "Maior Venda: $" << std::setprecision(2) << max_venda << std::endl;
    std::cout << "Menor Custo: $" << min_custo << std::endl;
    std::cout << std::endl;
}

// Exercício 3: Reduções com Operadores Lógicos Complexos
void exercicio3() {
    std::cout << "=== EXERCÍCIO 3: AUDITORIA COMPLEXA COM OPERADORES LÓGICOS ===" << std::endl;
    
    const int PROJETOS = 1000;
    std::vector<double> orcamentos(PROJETOS);
    std::vector<double> gastos(PROJETOS);
    std::vector<int> prazos(PROJETOS);
    
    // Inicialização com dados de projetos
    #pragma omp parallel for
    for (int i = 0; i < PROJETOS; ++i) {
        orcamentos[i] = 100000.0 + (i % 50) * 5000.0;
        gastos[i] = orcamentos[i] * (0.7 + (i % 40) * 0.01); // 70% to 110% do orçamento
        prazos[i] = 30 + (i % 90); // 30 a 120 dias
    }
    
    // Injeta alguns problemas
    gastos[100] = orcamentos[100] * 1.5;  // Estouro de orçamento
    gastos[200] = -5000.0;               // Gasto negativo
    prazos[300] = 365;                   // Prazo muito longo
    
    bool algum_estouro_orcamento = false;
    bool algum_gasto_invalido = false;
    bool algum_prazo_excessivo = false;
    bool todos_projetos_validos = true;
    
    const double LIMITE_ESTOURO = 1.2;    // 120% do orçamento
    const int PRAZO_MAXIMO = 180;         // 6 meses
    
    #pragma omp parallel for reduction(||:algum_estouro_orcamento) \
                             reduction(||:algum_gasto_invalido) \
                             reduction(||:algum_prazo_excessivo) \
                             reduction(&&:todos_projetos_validos)
    for (int i = 0; i < PROJETOS; ++i) {
        // Verifica estouro de orçamento
        if (gastos[i] > orcamentos[i] * LIMITE_ESTOURO) {
            algum_estouro_orcamento = true;
        }
        
        // Verifica gastos inválidos
        if (gastos[i] < 0) {
            algum_gasto_invalido = true;
        }
        
        // Verifica prazos excessivos
        if (prazos[i] > PRAZO_MAXIMO) {
            algum_prazo_excessivo = true;
        }
        
        // Verifica se todos os dados são válidos
        if (gastos[i] < 0 || prazos[i] <= 0 || orcamentos[i] <= 0) {
            todos_projetos_validos = false;
        }
    }
    
    std::cout << "Relatório de Auditoria de Projetos - TechNova Solutions" << std::endl;
    std::cout << "Total de projetos analisados: " << PROJETOS << std::endl;
    
    if (algum_estouro_orcamento) {
        std::cout << "[ALERTA] Encontrados projetos com estouro de orçamento (> " << (LIMITE_ESTOURO * 100) << "%)" << std::endl;
    } else {
        std::cout << "[OK] Nenhum projeto com estouro excessivo de orçamento" << std::endl;
    }
    
    if (algum_gasto_invalido) {
        std::cout << "[ALERTA] Encontrados gastos inválidos (valores negativos)" << std::endl;
    } else {
        std::cout << "[OK] Todos os gastos são válidos" << std::endl;
    }
    
    if (algum_prazo_excessivo) {
        std::cout << "[ALERTA] Encontrados prazos excessivos (> " << PRAZO_MAXIMO << " dias)" << std::endl;
    } else {
        std::cout << "[OK] Todos os prazos dentro do limite" << std::endl;
    }
    
    if (todos_projetos_validos) {
        std::cout << "[EXCELENTE] Todos os projetos possuem dados válidos" << std::endl;
    } else {
        std::cout << "[CRÍTICO] Encontrados projetos com dados inválidos" << std::endl;
    }
    std::cout << std::endl;
}

// Exercício 4: Análise de Desempenho por Departamento
void exercicio4() {
    std::cout << "=== EXERCÍCIO 4: ANÁLISE DE DESEMPENHO POR DEPARTAMENTO ===" << std::endl;
    
    struct Departamento {
        std::string nome;
        int funcionarios;
        double total_vendas;
        double total_custos;
        int projetos_concluidos;
        int projetos_atrasados;
    };
    
    std::vector<Departamento> departamentos = {
        {"Engenharia", 500, 0, 0, 0, 0},
        {"Vendas", 300, 0, 0, 0, 0},
        {"Marketing", 150, 0, 0, 0, 0},
        {"RH", 80, 0, 0, 0, 0},
        {"Finanças", 120, 0, 0, 0, 0}
    };
    
    const int TOTAL_DEPTS = departamentos.size();
    
    // Dados detalhados por funcionário
    const int TOTAL_FUNCIONARIOS = 1150; // Soma de todos os funcionários
    std::vector<double> vendas_por_funcionario(TOTAL_FUNCIONARIOS);
    std::vector<double> custos_por_funcionario(TOTAL_FUNCIONARIOS);
    std::vector<int> projetos_funcionario(TOTAL_FUNCIONARIOS);
    std::vector<int> atrasos_funcionario(TOTAL_FUNCIONARIOS);
    std::vector<int> dept_funcionario(TOTAL_FUNCIONARIOS);
    
    // Inicialização com dados realistas
    #pragma omp parallel for
    for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
        int dept_index = i % TOTAL_DEPTS;
        dept_funcionario[i] = dept_index;
        
        // Dados específicos por departamento
        switch(dept_index) {
            case 0: // Engenharia
                vendas_por_funcionario[i] = 0; // Engenharia não tem vendas diretas
                custos_por_funcionario[i] = 80000.0 + (i % 20) * 5000.0;
                projetos_funcionario[i] = 2 + (i % 4);
                atrasos_funcionario[i] = (i % 10 == 0) ? 1 : 0; // 10% com atrasos
                break;
            case 1: // Vendas
                vendas_por_funcionario[i] = 200000.0 + (i % 30) * 10000.0;
                custos_por_funcionario[i] = 50000.0 + (i % 10) * 5000.0;
                projetos_funcionario[i] = 5 + (i % 6);
                atrasos_funcionario[i] = (i % 15 == 0) ? 1 : 0; // 6.7% com atrasos
                break;
            // ... outros departamentos
            default:
                vendas_por_funcionario[i] = 50000.0 + (i % 10) * 5000.0;
                custos_por_funcionario[i] = 30000.0 + (i % 8) * 3000.0;
                projetos_funcionario[i] = 3 + (i % 5);
                atrasos_funcionario[i] = (i % 20 == 0) ? 1 : 0; // 5% com atrasos
        }
    }
    
    // Reduções para cada departamento
    #pragma omp parallel
    {
        // Cada thread tem suas próprias cópias dos acumuladores
        std::vector<Departamento> dept_local = departamentos;
        
        #pragma omp for
        for (int i = 0; i < TOTAL_FUNCIONARIOS; ++i) {
            int dept_idx = dept_funcionario[i];
            dept_local[dept_idx].total_vendas += vendas_por_funcionario[i];
            dept_local[dept_idx].total_custos += custos_por_funcionario[i];
            dept_local[dept_idx].projetos_concluidos += projetos_funcionario[i];
            dept_local[dept_idx].projetos_atrasados += atrasos_funcionario[i];
        }
        
        // Combina os resultados de todas as threads
        #pragma omp critical
        for (int i = 0; i < TOTAL_DEPTS; ++i) {
            departamentos[i].total_vendas += dept_local[i].total_vendas;
            departamentos[i].total_custos += dept_local[i].total_custos;
            departamentos[i].projetos_concluidos += dept_local[i].projetos_concluidos;
            departamentos[i].projetos_atrasados += dept_local[i].projetos_atrasados;
        }
    }
    
    // Apresentação dos resultados
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Desempenho por Departamento - TechNova Solutions" << std::endl;
    std::cout << "================================================" << std::endl;
    
    for (const auto& dept : departamentos) {
        double lucro = dept.total_vendas - dept.total_custos;
        double taxa_atraso = (dept.projetos_concluidos > 0) ? 
            (static_cast<double>(dept.projetos_atrasados) / dept.projetos_concluidos) * 100.0 : 0.0;
        
        std::cout << "\nDepartamento: " << dept.nome << std::endl;
        std::cout << "Funcionários: " << dept.funcionarios << std::endl;
        std::cout << "Vendas Totais: $" << dept.total_vendas << std::endl;
        std::cout << "Custos Totais: $" << dept.total_custos << std::endl;
        std::cout << "Lucro: $" << lucro << std::endl;
        std::cout << "Projetos Concluídos: " << dept.projetos_concluidos << std::endl;
        std::cout << "Taxa de Atraso: " << std::setprecision(1) << taxa_atraso << "%" << std::endl;
    }
}

int main() {
    std::cout << "TECHNOVA SOLUTIONS - EXERCÍCIOS DE REDUCTION OPENMP" << std::endl;
    std::cout << "====================================================" << std::endl;
    
    exercicio1(); // Desvio padrão amostral
    exercicio2(); // Múltiplas reduções aritméticas
    exercicio3(); // Reduções com operadores lógicos
    exercicio4(); // Análise de desempenho por departamento
    
    return 0;
}
