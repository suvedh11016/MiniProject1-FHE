// #include <iostream>
// #include <vector>
// #include <cmath>
// #include <string>
// #include <sstream>
// #include <map>

// const int LOG_THRESHOLD = 6; // log2(8)

// class Expression
// {
// public:
//     std::vector<int> terms_;  // Store integer coefficients directly.

//     Expression() {}

//     Expression(const std::vector<int>& terms)
//     {
//         for (int coeff : terms)
//         {
//             terms_.push_back(coeff);
//         }
//     }

//     Expression operator+(const Expression& other) const
//     {
//         std::vector<int> combined = terms_;
//         combined.insert(combined.end(), other.terms_.begin(), other.terms_.end());
//         return Expression(combined);
//     }

//     std::string balanced_tree() const
//     {
//         std::vector<std::string> terms;

//         for (int coeff : terms_)
//         {
//             if (coeff == 0) continue;

//             std::cout << "Expanding " << coeff << " → Binary: ";
//             bool started = false;
//             for (int i = 31; i >= 0; --i)
//             {
//                 if ((coeff >> i) & 1)
//                 {
//                     std::cout << "1";
//                     started = true;
//                 }
//                 else if (started)
//                 {
//                     std::cout << "0";
//                 }
//             }
//             std::cout << "\n";

//             int logval = std::log2(coeff);
//             if (logval > LOG_THRESHOLD)
//             {
//                 std::cout << "  → exceeds threshold, using direct multiplication: " << coeff << "\n";
//                 terms.push_back(std::to_string(coeff));
//             }
//             else
//             {
//                 for (int i = 0; (1 << i) <= coeff; ++i)
//                 {
//                     if ((coeff >> i) & 1)
//                     {
//                         int power = 1 << i;
//                         if (power == 1)
//                             terms.push_back("1");
//                         else
//                             terms.push_back(std::to_string(power));
//                     }
//                 }
//             }
//         }

//         // Build balanced binary tree
//         while (terms.size() > 1)
//         {
//             std::vector<std::string> next;
//             for (size_t i = 0; i + 1 < terms.size(); i += 2)
//             {
//                 next.push_back("(" + terms[i] + " + " + terms[i + 1] + ")");
//             }
//             if (terms.size() % 2 == 1)
//             {
//                 next.push_back(terms.back());
//             }
//             terms = std::move(next);
//         }

//         return terms.empty() ? "0" : terms[0];
//     }
// };

// class fhe_number
// {
// public:
//     Expression expr;
//     std::string name;

//     static int counter;

//     fhe_number(std::string var) : name(var)
//     {
//         expr = Expression({std::stoi(var)});
//     }

//     fhe_number(Expression e)
//     {
//         std::ostringstream oss;
//         oss << "tmp" << counter++;
//         name = oss.str();
//         expr = e;
//     }

//     friend fhe_number fheADD(const fhe_number& a, const fhe_number& b)
//     {
//         return fhe_number(a.expr + b.expr);
//     }

//     void print_balanced_tree() const
//     {
//         std::cout << "Expression for " << name << ": " << expr.balanced_tree() << "\n";
//     }
// };

// int fhe_number::counter = 0;

// std::vector<int> parse_input(const std::string& input)
// {
//     std::vector<int> terms;
//     std::istringstream ss(input);
//     std::string token;

//     while (ss >> token)
//     {
//         if (token == "+" || token == "*") continue;

//         if (token.find('*') != std::string::npos)
//         {
//             size_t pos = token.find('*');
//             int left = std::stoi(token.substr(0, pos));
//             int right = std::stoi(token.substr(pos + 1));
//             terms.push_back(left * right);
//         }
//         else
//         {
//             int coeff = std::stoi(token);
//             terms.push_back(coeff);
//         }
//     }

//     return terms;
// }

// int main()
// {
//     std::string input;
//     std::cout << "Enter expression (e.g., 7 + 5 + 3*4): ";
//     std::getline(std::cin, input);

//     // Parse input to get the integer coefficients
//     auto terms = parse_input(input);

//     Expression expr(terms);
//     fhe_number result(expr);

//     result.print_balanced_tree();

//     return 0;
// }

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <cmath>

const int LOG_THRESHOLD = 6;

class Expression
{
public:
    std::vector<std::pair<int, std::string>> terms_;

    Expression() {}

    Expression(const std::vector<std::pair<int, std::string>> &terms)
    {
        terms_ = terms; // keep as-is, don't simplify
    }

    Expression operator+(const Expression &other) const
    {
        std::vector<std::pair<int, std::string>> combined = terms_;
        combined.insert(combined.end(), other.terms_.begin(), other.terms_.end());
        return Expression(combined);
    }

    std::string balanced_tree() const
    {
        std::vector<std::string> expanded_terms;

        for (const auto &[coeff, var] : terms_)
        {
            if (coeff == 0)
                continue;
            if ((int)std::log2(coeff) <= LOG_THRESHOLD)
            {
                std::cout << "Expanding " << coeff << (var.empty() ? "" : "*" + var) << " → Binary: ";
            }
            else
            {
                std::cout << "Expanding " << coeff << (var.empty() ? "" : "*" + var) << " → exceeds threshold, using direct multiplication: ";
            }
            bool started = false;
            for (int i = 31; i >= 0; --i)
            {
                if ((coeff >> i) & 1)
                {
                    std::cout << "1";
                    started = true;
                }
                else if (started)
                {
                    std::cout << "0";
                }
            }
            std::cout << "\n";

            if ((int)std::log2(coeff) <= LOG_THRESHOLD)
            {
                for (int i = 0; (1 << i) <= coeff; ++i)
                {
                    if ((coeff >> i) & 1)
                    {
                        int power = 1 << i;
                        if (var.empty())
                            expanded_terms.push_back(std::to_string(power));
                        else if (power == 1)
                            expanded_terms.push_back(var);
                        else
                            expanded_terms.push_back(std::to_string(power) + "*" + var);
                    }
                }
            }
            else
            {
                if (var.empty())
                    expanded_terms.push_back(std::to_string(coeff));
                else
                    expanded_terms.push_back(std::to_string(coeff) + "*" + var);
            }
        }

        while (expanded_terms.size() > 1)
        {
            std::vector<std::string> next;
            for (size_t i = 0; i + 1 < expanded_terms.size(); i += 2)
            {
                next.push_back("(" + expanded_terms[i] + " + " + expanded_terms[i + 1] + ")");
            }
            if (expanded_terms.size() % 2 == 1)
            {
                next.push_back(expanded_terms.back());
            }
            expanded_terms = std::move(next);
        }

        return expanded_terms.empty() ? "0" : expanded_terms[0];
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

    friend fhe_number fheADD(const fhe_number &a, const fhe_number &b)
    {
        return fhe_number(a.expr + b.expr);
    }

    void print_balanced_tree() const
    {
        std::cout << expr.balanced_tree() << "\n";
    }
};

int fhe_number::counter = 0;

std::vector<std::pair<int, std::string>> parse_input(const std::string &input)
{
    std::vector<std::pair<int, std::string>> terms;
    std::istringstream ss(input);
    std::string token;

    while (ss >> token)
    {
        if (token == "+")
            continue;

        int coeff = 1;
        std::string var = token;

        if (token.find('*') != std::string::npos)
        {
            size_t pos = token.find('*');
            coeff = std::stoi(token.substr(0, pos));
            var = token.substr(pos + 1);
        }
        else if (std::isdigit(token[0]))
        {
            coeff = std::stoi(token);
            var = "";
        }

        terms.push_back({coeff, var});
    }

    return terms;
}

int main()
{
    std::string input;
    std::getline(std::cin, input);

    auto terms = parse_input(input);

    Expression expr(terms);
    fhe_number result(expr);

    result.print_balanced_tree();

    return 0;
}

/*hello ee message nuvvu kaaliga unnappudu chaduvu, its okay nuvvu 1 day tharvatha chusina paruvaledhu
disclaimer mundhe chepthunna ee msg chadivaka you may feel bad also i dont know
actually nuvvu roju lo entha maatladina i dont care, i want to talk so ill msg
but maatladakapoyina paruvaledhu
*/