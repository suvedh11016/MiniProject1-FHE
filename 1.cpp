#include <iostream>
#include <vector>
#include <map>
#include <cmath>
#include <string>
#include <sstream>
#include <iterator>
#include <algorithm>

const int LOG_THRESHOLD = 6; // log2(8)

class Expression 
{
public:
    std::vector<std::pair<int, std::string>> terms_;

    Expression() {}

    Expression(const std::vector<std::pair<int, std::string>>& terms) 
    {
        std::map<std::string, int> simplified;
        for (auto [coeff, var] : terms) 
        {
            simplified[var] += coeff;
        }
        for (auto [var, coeff] : simplified) 
        {
            if (coeff != 0) terms_.emplace_back(coeff, var);
        }
    }

    Expression operator+(const Expression& other) const 
    {
        std::vector<std::pair<int, std::string>> combined = terms_;
        combined.insert(combined.end(), other.terms_.begin(), other.terms_.end());
        return Expression(combined);
    }

    std::string balanced_tree() const 
    {
        std::vector<std::string> terms;

        for (const auto& [coeff, var] : terms_) 
        {
            if (coeff == 0) continue;

            std::cout << "Expanding " << coeff << "*" << var << " → Binary: ";
            bool started = false;
            for (int i = 31; i >= 0; --i) 
            {
                if ((coeff >> i) & 1) 
                {
                    std::cout << "1";
                    started = true;
                } else if (started) 
                {
                    std::cout << "0";
                }
            }
            std::cout << "\n";

            int logval = std::log2(coeff);
            if (logval > LOG_THRESHOLD) 
            {
                std::cout << "  → exceeds threshold, using direct multiplication: " << coeff << "*" << var << "\n";
                terms.push_back(std::to_string(coeff) + "*" + var);
            } 
            else 
            {
                for (int i = 0; (1 << i) <= coeff; ++i) 
                {
                    if ((coeff >> i) & 1) 
                    {
                        int power = 1 << i;
                        if (power == 1)
                            terms.push_back(var);
                        else
                            terms.push_back(std::to_string(power) + "*" + var);
                    }
                }
            }
        }

        // Build balanced binary tree
        while (terms.size() > 1) 
        {
            std::vector<std::string> next;
            for (size_t i = 0; i + 1 < terms.size(); i += 2) 
            {
                next.push_back("(" + terms[i] + " + " + terms[i + 1] + ")");
            }
            if (terms.size() % 2 == 1) 
            {
                next.push_back(terms.back());
            }
            terms = std::move(next);
        }

        return terms.empty() ? "0" : terms[0];
    }
};

class fhe_number 
{
public:
    Expression expr;
    std::string name;

    static int counter;

    fhe_number(std::string var) : name(var) 
    {
        expr = Expression({{1, var}});
    }

    fhe_number(Expression e) 
    {
        std::ostringstream oss;
        oss << "tmp" << counter++;
        name = oss.str();
        expr = e;
    }

    friend fhe_number fheADD(const fhe_number& a, const fhe_number& b) 
    {
        return fhe_number(a.expr + b.expr);
    }

    void print_balanced_tree() const 
    {
        std::cout << "Expression for " << name << ": " << expr.balanced_tree() << "\n";
    }
};

int fhe_number::counter = 0;

std::vector<std::pair<int, std::string>> parse_input(const std::string& input) 
{
    std::vector<std::pair<int, std::string>> terms;
    std::istringstream ss(input);
    std::string token;
    
    while (ss >> token) 
    {
        if (token == "+") continue;

        int coeff = 1;
        std::string var = token;

        if (token.find('*') != std::string::npos) 
        {
            size_t pos = token.find('*');
            coeff = std::stoi(token.substr(0, pos));
            var = token.substr(pos + 1);
        }

        terms.push_back({coeff, var});
    }

    return terms;
}

int main() 
{
    std::string input;
    // std::cout << "Enter expression (e.g., x1 + x2 + 3*x3): ";
    std::getline(std::cin, input);

    auto terms = parse_input(input);

    Expression expr(terms);
    fhe_number result(expr);

    result.print_balanced_tree();

    return 0;
}

