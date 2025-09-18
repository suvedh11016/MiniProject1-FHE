#include <iostream>
#include <vector>
#include <string>
#include <map>
#include <sstream>
#include <cmath>
#include <seal/seal.h>

using namespace std;
using namespace seal;

const int LOG_THRESHOLD = 6; // log2(8)

class Expression {
public:
    vector<pair<int, string>> terms_;
    Expression() {}
    Expression(const vector<pair<int, string>>& terms) {
        map<string, int> simplified;
        for (auto [coeff, var] : terms)
            simplified[var] += coeff;
        for (auto [var, coeff] : simplified)
            if (coeff != 0) terms_.emplace_back(coeff, var);
    }
    Expression operator+(const Expression& other) const {
        vector<pair<int, string>> combined = terms_;
        combined.insert(combined.end(), other.terms_.begin(), other.terms_.end());
        return Expression(combined);
    }
    string balanced_tree() const {
        vector<string> terms;
        for (const auto& [coeff, var] : terms_) {
            if (coeff == 0) continue;
            int logval = log2(coeff);
            if (logval > LOG_THRESHOLD)
                terms.push_back(to_string(coeff) + "*" + var);
            else {
                for (int i = 0; (1 << i) <= coeff; ++i)
                    if ((coeff >> i) & 1)
                        terms.push_back((1 << i == 1 ? "" : to_string(1 << i) + "*") + var);
            }
        }
        while (terms.size() > 1) {
            vector<string> next;
            for (size_t i = 0; i + 1 < terms.size(); i += 2)
                next.push_back("(" + terms[i] + " + " + terms[i + 1] + ")");
            if (terms.size() % 2 == 1)
                next.push_back(terms.back());
            terms = move(next);
        }
        return terms.empty() ? "0" : terms[0];
    }
};

class fhe_number {
public:
    Expression expr;
    string name;
    static int counter;
    fhe_number(string var) : name(var) { expr = Expression({{1, var}}); }
    fhe_number(Expression e) {
        ostringstream oss;
        oss << "tmp" << counter++;
        name = oss.str();
        expr = e;
    }
    friend fhe_number fheADD(const fhe_number& a, const fhe_number& b) {
        return fhe_number(a.expr + b.expr);
    }
    void print_balanced_tree() const {
        cout << "Expression for " << name << ": " << expr.balanced_tree() << "\n";
    }
};
int fhe_number::counter = 0;

vector<pair<int, string>> parse_input(const string& input) {
    vector<pair<int, string>> terms;
    istringstream ss(input);
    string token;
    while (ss >> token) {
        if (token == "+") continue;
        int coeff = 1;
        string var = token;
        if (token.find('*') != string::npos) {
            size_t pos = token.find('*');
            coeff = stoi(token.substr(0, pos));
            var = token.substr(pos + 1);
        }
        terms.push_back({coeff, var});
    }
    return terms;
}

int main() {
    // Input
    cout << "Enter expression (e.g., x1 + 2*x2 + 3*x3): ";
    string input;
    getline(cin, input);

    // Parse and build symbolic tree
    auto parsed = parse_input(input);
    fhe_number num(Expression(parsed));
    num.print_balanced_tree();

    // SEAL setup
    EncryptionParameters parms(scheme_type::bgv);
    size_t degree = 8192;
    parms.set_poly_modulus_degree(degree);
    parms.set_coeff_modulus(CoeffModulus::Create(degree, {40, 40, 40}));
    parms.set_plain_modulus(PlainModulus::Batching(degree, 20));

    SEALContext context(parms);
    KeyGenerator keygen(context);
    PublicKey pk = keygen.public_key();
    SecretKey sk = keygen.secret_key();
    Encryptor encryptor(context, pk);
    Evaluator evaluator(context);
    Decryptor decryptor(context, sk);
    BatchEncoder encoder(context);

    // Map variables to slots
    vector<uint64_t> slots(degree, 0);
    for (auto [coeff, var] : parsed) {
        if (var[0] == 'x') {
            int idx = stoi(var.substr(1));
            if (idx < degree)
                slots[idx] += coeff;
        }
    }

    // Encrypt and evaluate
    Plaintext plain;
    encoder.encode(slots, plain);
    Ciphertext ct;
    encryptor.encrypt(plain, ct);

    // Simple evaluation
    evaluator.square_inplace(ct);

    // Decrypt and print
    Plaintext dec;
    decryptor.decrypt(ct, dec);
    vector<uint64_t> out;
    encoder.decode(dec, out);

    cout << "Encrypted (squared) expression output:\n";
    for (int i = 0; i < 10; ++i)
        if (out[i] != 0)
            cout << "x" << i << ": " << out[i] << "\n";

    return 0;
}
