#include <iostream>
#include <vector>
#include <string>
#include <omp.h>
#include <iomanip>
#include <cmath>

struct Funcionario {
    double salario;
    int departamento;
    int idade;
    double horas_trabalhadas;
    std::string nome;
    
    Funcionario(double s, int d, int i, double h, const std::string& n) 
        : salario(s), departamento(d), idade(i), horas_trabalhadas(h), nome(n) {}
};

// Função para gerar dados de exemplo mais realistas
std::vector<Funcionario> gerar_dados_funcionarios(int N) {
    std::vector<Funcionario> funcionarios;
    funcionarios.reserve(N);
    
    std::vector<std::string> nomes = {
        "Ana Silva", "Carlos Santos", "Maria Oliveira", "João Pereira", 
        "Fernanda Costa", "Ricardo Lima", "Juliana Alves", "Pedro Souza",
        "Amanda Rocha", "Lucas Barbosa", "Patrícia Martins", "Roberto Ferreira"
    };
    
    #pragma omp parallel for
    for (int i = 0; i < N; ++i) {
        double salario = 2000.0 + (i % 100) * 50.0;
        int departamento = (i % 5) + 1;
        int idade = 25 + (i % 40);
        double horas = 160.0 + (i % 80);
        std::string nome = nomes[i % nomes.size()] + " " + std::to_string(i);
        
        #pragma omp critical
        funcionarios.emplace_back(salario, departamento, idade, horas, nome);
    }
    
    return funcionarios;
}

int main() {
    const int N = 100000;
    auto funcionarios = gerar_dados_funcionarios(N);
    
    std::cout << "=== AUDITORIA AVANÇADA DE DADOS FUNCIONAIS ===" << std::endl;
    std::cout << "Total de funcionários: " << N << std::endl << std::endl;

    // CONSTANTES DE NEGÓCIO
    const double PISO_SALARIAL = 1500.0;
    const double TETO_SALARIAL = 20000.0;
    const int IDADE_MINIMA = 18;
    const int IDADE_MAXIMA = 70;
    const double HORAS_MINIMAS = 80.0;
    const double HORAS_MAXIMAS = 220.0;

    // 1. AUDITORIA BÁSICA (similar ao exemplo anterior)
    bool piso_violado = false;
    bool teto_violado = false;
    bool dados_validos = true;

    #pragma omp parallel for \
        reduction(||:piso_violado, teto_violado) \
        reduction(&&:dados_validos)
    for (int i = 0; i < N; ++i) {
        const auto& f = funcionarios[i];
        
        if (f.salario < PISO_SALARIAL) piso_violado = true;
        if (f.salario > TETO_SALARIAL) teto_violado = true;
        if (f.salario <= 0 || f.idade <= 0 || f.horas_trabalhadas <= 0) {
            dados_validos = false;
        }
    }

    std::cout << "1. AUDITORIA BÁSICA DE CONSISTÊNCIA:" << std::endl;
    std::cout << "   Piso violado: " << (piso_violado ? "SIM" : "NÃO") << std::endl;
    std::cout << "   Teto violado: " << (teto_violado ? "SIM" : "NÃO") << std::endl;
    std::cout << "   Dados válidos: " << (dados_validos ? "SIM" : "NÃO") << std::endl << std::endl;

    // 2. AUDITORIA AVANÇADA COM CONTAGEM DE VIOLAÇÕES
    int violacoes_piso = 0;
    int violacoes_teto = 0;
    int violacoes_idade = 0;
    int violacoes_horas = 0;
    int violacoes_multiplas = 0;

    #pragma omp parallel for \
        reduction(+:violacoes_piso, violacoes_teto, violacoes_idade, violacoes_horas, violacoes_multiplas)
    for (int i = 0; i < N; ++i) {
        const auto& f = funcionarios[i];
        int violacoes_local = 0;
        
        if (f.salario < PISO_SALARIAL) {
            violacoes_piso++;
            violacoes_local++;
        }
        if (f.salario > TETO_SALARIAL) {
            violacoes_teto++;
            violacoes_local++;
        }
        if (f.idade < IDADE_MINIMA || f.idade > IDADE_MAXIMA) {
            violacoes_idade++;
            violacoes_local++;
        }
        if (f.horas_trabalhadas < HORAS_MINIMAS || f.horas_trabalhadas > HORAS_MAXIMAS) {
            violacoes_horas++;
            violacoes_local++;
        }
        if (violacoes_local >= 2) {
            violacoes_multiplas++;
        }
    }

    std::cout << "2. ESTATÍSTICAS DETALHADAS DE VIOLAÇÕES:" << std::endl;
    std::cout << "   Violações de piso salarial: " << violacoes_piso << std::endl;
    std::cout << "   Violações de teto salarial: " << violacoes_teto << std::endl;
    std::cout << "   Violações de idade: " << violacoes_idade << std::endl;
    std::cout << "   Violações de horas: " << violacoes_horas << std::endl;
    std::cout << "   Funcionários com múltiplas violações: " << violacoes_multiplas << std::endl << std::endl;

    // 3. VERIFICAÇÃO DE CONSISTÊNCIA ENTRE DEPARTAMENTOS
    bool salarios_consistentes = true;
    bool horas_consistentes = true;
    double menor_salario_global = std::numeric_limits<double>::max();
    double maior_salario_global = std::numeric_limits<double>::lowest();

    #pragma omp parallel for \
        reduction(&&:salarios_consistentes, horas_consistentes) \
        reduction(min:menor_salario_global) \
        reduction(max:maior_salario_global)
    for (int i = 0; i < N; ++i) {
        const auto& f = funcionarios[i];
        
        // Verifica consistência salarial por departamento
        double salario_medio_esperado = 3000.0 + f.departamento * 500.0;
        double margem_erro = salario_medio_esperado * 0.3; // 30% de margem
        
        if (std::fabs(f.salario - salario_medio_esperado) > margem_erro) {
            salarios_consistentes = false;
        }
        
        // Verifica consistência de horas
        if (f.horas_trabalhadas < HORAS_MINIMAS || f.horas_trabalhadas > HORAS_MAXIMAS) {
            horas_consistentes = false;
        }
        
        // Atualiza min/max
        if (f.salario < menor_salario_global) menor_salario_global = f.salario;
        if (f.salario > maior_salario_global) maior_salario_global = f.salario;
    }

    std::cout << "3. ANÁLISE DE CONSISTÊNCIA:" << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "   Salários consistentes por departamento: " << (salarios_consistentes ? "SIM" : "NÃO") << std::endl;
    std::cout << "   Horas consistentes: " << (horas_consistentes ? "SIM" : "NÃO") << std::endl;
    std::cout << "   Menor salário: R$ " << menor_salario_global << std::endl;
    std::cout << "   Maior salário: R$ " << maior_salario_global << std::endl;
    std::cout << "   Amplitude salarial: R$ " << (maior_salario_global - menor_salario_global) << std::endl << std::endl;

    // 4. DETECÇÃO DE ANOMALIAS ESTATÍSTICAS
    double soma_salarios = 0.0;
    double soma_quadrados = 0.0;
    int contagem_anomalias = 0;

    #pragma omp parallel for \
        reduction(+:soma_salarios, soma_quadrados, contagem_anomalias)
    for (int i = 0; i < N; ++i) {
        soma_salarios += funcionarios[i].salario;
        soma_quadrados += funcionarios[i].salario * funcionarios[i].salario;
    }

    double media_salarios = soma_salarios / N;
    double desvio_padrao = std::sqrt((soma_quadrados / N) - (media_salarios * media_salarios));
    
    // Segunda passada para detectar anomalias
    #pragma omp parallel for reduction(+:contagem_anomalias)
    for (int i = 0; i < N; ++i) {
        double z_score = std::fabs((funcionarios[i].salario - media_salarios) / desvio_padrao);
        if (z_score > 3.0) { // Mais de 3 desvios padrão da média
            contagem_anomalias++;
        }
    }

    std::cout << "4. DETECÇÃO DE ANOMALIAS ESTATÍSTICAS:" << std::endl;
    std::cout << "   Média salarial: R$ " << media_salarios << std::endl;
    std::cout << "   Desvio padrão: R$ " << desvio_padrao << std::endl;
    std::cout << "   Anomalias detectadas (Z-score > 3): " << contagem_anomalias << std::endl << std::endl;

    // 5. RELATÓRIO FINAL DE QUALIDADE
    std::cout << "5. RELATÓRIO FINAL DE QUALIDADE DOS DADOS:" << std::endl;
    
    double taxa_erro_total = (violacoes_piso + violacoes_teto + violacoes_idade + violacoes_horas) / (4.0 * N);
    double qualidade_geral = (1.0 - taxa_erro_total) * 100.0;
    
    std::cout << std::fixed << std::setprecision(1);
    std::cout << "   Qualidade geral dos dados: " << qualidade_geral << "%" << std::endl;
    
    if (qualidade_geral >= 95.0) {
        std::cout << "   STATUS: EXCELENTE" << std::endl;
    } else if (qualidade_geral >= 90.0) {
        std::cout << "   STATUS: BOM" << std::endl;
    } else if (qualidade_geral >= 80.0) {
        std::cout << "   STATUS: REGULAR" << std::endl;
    } else {
        std::cout << "   STATUS: CRÍTICO - Necessita intervenção imediata" << std::endl;
    }

    return 0;
}
