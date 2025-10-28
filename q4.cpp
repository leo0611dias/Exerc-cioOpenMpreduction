#include <iostream>
#include <vector>
#include <string>
#include <random>
#include <algorithm>
#include <cmath>
#include <map>
#include <iomanip>

class BigTechSalaries {
private:
    std::string companyName;
    std::map<std::string, std::string> departments;
    std::map<std::string, std::vector<std::string>> positions;
    std::vector<std::string> countries;
    std::vector<std::string> regions;

    // Gerador de números aleatórios
    std::random_device rd;
    std::mt19937 gen;

public:
    BigTechSalaries() : gen(rd()) {
        companyName = "NexusTech Solutions";
        
        // Departamentos
        departments = {
            {"ENG", "Engenharia de Software"},
            {"DS", "Ciência de Dados"},
            {"MKT", "Marketing Digital"},
            {"FIN", "Finanças e Análise"},
            {"CLD", "Cloud Computing"},
            {"IA", "Inteligência Artificial"}
        };
        
        // Cargos por departamento
        positions["ENG"] = {"Engenheiro Júnior", "Engenheiro Pleno", "Engenheiro Sênior", "Tech Lead", "Arquiteto de Software"};
        positions["DS"] = {"Cientista de Dados Júnior", "Cientista de Dados Pleno", "Cientista de Dados Sênior", "Head of Data Science"};
        positions["MKT"] = {"Analista de Marketing", "Especialista em Growth", "Gerente de Marketing", "Diretor de Marketing"};
        positions["FIN"] = {"Analista Financeiro", "Contador Sênior", "Controller", "Diretor Financeiro"};
        positions["CLD"] = {"DevOps Engineer", "Cloud Architect", "SRE Engineer", "Cloud Solutions Lead"};
        positions["IA"] = {"Pesquisador de IA", "Machine Learning Engineer", "AI Specialist", "Chief AI Officer"};
        
        countries = {"Brasil", "Estados Unidos", "Canadá", "México", "Argentina", "Chile"};
        regions = {"Norte", "Sul", "Centro-Oeste", "Sudeste", "Noroeste"};
    }

    std::vector<double> generateSalaries(int numSalaries = 2000000) {
        std::cout << "Gerando " << numSalaries << " salários para " << companyName << "...\n";
        
        std::vector<double> salaries;
        salaries.reserve(numSalaries);
        
        // Distribuições para números aleatórios
        std::uniform_int_distribution<> countryDist(0, countries.size() - 1);
        std::uniform_int_distribution<> deptDist(0, departments.size() - 1);
        std::uniform_real_distribution<> variationDist(0.85, 1.15);
        
        // Multiplicadores por região
        std::map<std::string, double> regionMultiplier = {
            {"Estados Unidos", 1.0},
            {"Canadá", 0.85},
            {"Brasil", 0.45},
            {"México", 0.35},
            {"Argentina", 0.25},
            {"Chile", 0.30}
        };
        
        // Salários base por nível
        std::map<int, double> baseSalaries = {
            {0, 45000},   // Júnior
            {1, 75000},   // Pleno
            {2, 120000},  // Sênior
            {3, 160000},  // Liderança
            {4, 220000}   // Diretoria
        };
        
        // Multiplicadores por departamento
        std::map<std::string, double> deptMultiplier = {
            {"ENG", 1.1},
            {"DS", 1.2},
            {"IA", 1.3},
            {"CLD", 1.15},
            {"FIN", 0.9},
            {"MKT", 0.85}
        };
        
        // Obter chaves dos departamentos para acesso aleatório
        std::vector<std::string> deptKeys;
        for (const auto& dept : departments) {
            deptKeys.push_back(dept.first);
        }
        
        for (int i = 0; i < numSalaries; ++i) {
            std::string country = countries[countryDist(gen)];
            std::string department = deptKeys[deptDist(gen)];
            
            // Nível aleatório baseado no número de cargos disponíveis
            std::uniform_int_distribution<> levelDist(0, positions[department].size() - 1);
            int positionLevel = levelDist(gen);
            
            double baseSalary = baseSalaries[positionLevel];
            double variation = variationDist(gen);
            
            double finalSalary = baseSalary * deptMultiplier[department] 
                               * regionMultiplier[country] * variation;
            
            salaries.push_back(finalSalary);
        }
        
        return salaries;
    }

    double calculateSampleStandardDeviation(const std::vector<double>& salaries) {
        int n = salaries.size();
        if (n <= 1) {
            return 0.0;
        }
        
        // Calcular média
        double sum = 0.0;
        for (double salary : salaries) {
            sum += salary;
        }
        double mean = sum / n;
        
        // Calcular soma dos quadrados das diferenças
        double sumSquaredDiff = 0.0;
        for (double salary : salaries) {
            double diff = salary - mean;
            sumSquaredDiff += diff * diff;
        }
        
        // Desvio padrão amostral (n-1 no denominador)
        double variance = sumSquaredDiff / (n - 1);
        return std::sqrt(variance);
    }

    void analyzeSalaries(const std::vector<double>& salaries) {
        std::cout << "\n" << std::string(60, '=') << "\n";
        std::cout << "ANÁLISE DE SALÁRIOS - " << companyName << "\n";
        std::cout << std::string(60, '=') << "\n";
        
        int n = salaries.size();
        
        // Calcular média
        double sum = 0.0;
        for (double salary : salaries) {
            sum += salary;
        }
        double meanSalary = sum / n;
        
        // Calcular desvio padrão
        double stdDeviation = calculateSampleStandardDeviation(salaries);
        
        // Calcular percentis (necessário ordenar)
        std::vector<double> sortedSalaries = salaries;
        std::sort(sortedSalaries.begin(), sortedSalaries.end());
        
        double p25 = sortedSalaries[static_cast<int>(0.25 * n)];
        double p50 = sortedSalaries[static_cast<int>(0.50 * n)];  // Mediana
        double p75 = sortedSalaries[static_cast<int>(0.75 * n)];
        double p90 = sortedSalaries[static_cast<int>(0.90 * n)];
        
        // Coeficiente de variação
        double cv = (stdDeviation / meanSalary) * 100;
        
        std::cout << "Total de funcionários: " << n << "\n";
        std::cout << std::fixed << std::setprecision(2);
        std::cout << "Média salarial: USD " << meanSalary << "\n";
        std::cout << "Mediana salarial: USD " << p50 << "\n";
        std::cout << "Desvio padrão amostral: USD " << stdDeviation << "\n";
        std::cout << "Coeficiente de variação: " << cv << "%\n";
        
        std::cout << "\nDistribuição percentílica:\n";
        std::cout << "P25 (1º quartil): USD " << p25 << "\n";
        std::cout << "P50 (mediana): USD " << p50 << "\n";
        std::cout << "P75 (3º quartil): USD " << p75 << "\n";
        std::cout << "P90: USD " << p90 << "\n";
        
        // Faixas salariais
        std::vector<double> ranges = {0, 30000, 60000, 90000, 120000, 150000, 200000};
        std::vector<std::string> rangeLabels = {
            "Até USD 30k", "USD 30k-60k", "USD 60k-90k", "USD 90k-120k",
            "USD 120k-150k", "USD 150k-200k", "Acima de USD 200k"
        };
        
        std::cout << "\nDistribuição por faixas salariais:\n";
        for (size_t i = 0; i < rangeLabels.size(); ++i) {
            int count = 0;
            double lowerBound = ranges[i];
            double upperBound = (i == rangeLabels.size() - 1) ? 1e9 : ranges[i + 1];
            
            for (double salary : salaries) {
                if (salary >= lowerBound && salary < upperBound) {
                    count++;
                }
            }
            
            double percentage = (static_cast<double>(count) / n) * 100;
            std::cout << rangeLabels[i] << ": " << count << " funcionários (" 
                      << std::setprecision(1) << percentage << "%)\n";
        }
        
        // Análise do desvio padrão
        std::cout << "\nINTERPRETAÇÃO DO DESVIO PADRÃO:\n";
        std::cout << "O desvio padrão amostral de USD " << std::setprecision(2) << stdDeviation 
                  << " indica a dispersão média dos salários em relação à média.\n";
        
        if (cv < 30) {
            std::cout << "Baixa dispersão salarial (CV < 30%) - Salários relativamente homogêneos\n";
        } else if (cv < 60) {
            std::cout << "Média dispersão salarial (30% < CV < 60%) - Variação moderada nos salários\n";
        } else {
            std::cout << "Alta dispersão salarial (CV > 60%) - Grande variação nos salários\n";
        }
        
        std::cout << "\nIntervalo de confiança aproximado (68% dos dados):\n";
        std::cout << "USD " << (meanSalary - stdDeviation) << " a USD " 
                  << (meanSalary + stdDeviation) << "\n";
    }
};

// Função auxiliar para testar com um conjunto menor de dados
void testWithSmallSample() {
    BigTechSalaries bigtech;
    
    // Teste com amostra menor para demonstração
    std::vector<double> testSalaries = bigtech.generateSalaries(10000);
    bigtech.analyzeSalaries(testSalaries);
}

// Função principal com opção de escolher o tamanho da amostra
void runFullAnalysis(int sampleSize = 2000000) {
    BigTechSalaries bigtech;
    
    std::cout << "BIGTECH SALARY ANALYSIS SYSTEM\n";
    std::cout << "Empresa: " << bigtech.getCompanyName() << "\n\n";
    
    auto salaries = bigtech.generateSalaries(sampleSize);
    bigtech.analyzeSalaries(salaries);
}

int main() {
    std::cout << "=== SISTEMA DE ANÁLISE DE DESVIO PADRÃO SALARIAL ===\n\n";
    
    // Para demonstração, usar amostra menor
    // Para a análise completa com 2 milhões, descomente a linha abaixo:
    // runFullAnalysis(2000000);
    
    testWithSmallSample();
    
    return 0;
}
