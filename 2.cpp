/****************************************************************/
/* SEAL BGV batched Integer Addition and Multiplication         */
/* Operation: Result = (Input1 + Input2) * Input3               */
/* Author: [Your Name]                                          */
/****************************************************************/
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <sys/resource.h>
#include <unistd.h>
#include "seal/seal.h"
#include "examples.h"

using namespace std;
using namespace seal;

int random_int(int min, int max){
    return min + rand() % (max + 1 - min);
}

long get_mem_usage(){
    struct rusage usage;
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}

int main() {
    srand(time(NULL));
    long baseline = get_mem_usage();

    /***** Parameter Generation *****/
    clock_t param_clock = clock();
    EncryptionParameters parms(scheme_type::bgv);
    size_t poly_modulus_degree = 32768;
    parms.set_poly_modulus_degree(poly_modulus_degree);
    parms.set_coeff_modulus(CoeffModulus::Create(poly_modulus_degree, { 60, 40, 40, 60 }));
    parms.set_plain_modulus(65537);
    SEALContext context(parms);
    print_parameters(context);
    param_clock = clock() - param_clock;

    /***** Key Generation *****/
    clock_t key_clock = clock();
    KeyGenerator keygen(context);
    SecretKey secret_key = keygen.secret_key();
    PublicKey public_key;
    keygen.create_public_key(public_key);
    RelinKeys relin_keys;
    keygen.create_relin_keys(relin_keys);
    Encryptor encryptor(context, public_key);
    Evaluator evaluator(context);
    Decryptor decryptor(context, secret_key);
    BatchEncoder batch_encoder(context);
    size_t slot_count = batch_encoder.slot_count();
    size_t row_size = slot_count / 2;
    key_clock = clock() - key_clock;

    /***** Encoding & Encryption *****/
    clock_t enc_clock = clock();
    int N = 8192;
    vector<uint64_t> input1(slot_count, 0ULL);
    vector<uint64_t> input2(slot_count, 0ULL);
    vector<uint64_t> input3(slot_count, 0ULL);

    for (int r = 0; r < 2; r++) {
        for (int c = 0; c < N/2; c++) {
            input1[r * row_size + c] = random_int(1, 100);
            input2[r * row_size + c] = random_int(1, 100);
            input3[r * row_size + c] = random_int(1, 10);
        }
    }

    Plaintext plain1, plain2, plain3;
    batch_encoder.encode(input1, plain1);
    batch_encoder.encode(input2, plain2);
    batch_encoder.encode(input3, plain3);

    Ciphertext enc1, enc2, enc3;
    encryptor.encrypt(plain1, enc1);
    encryptor.encrypt(plain2, enc2);
    encryptor.encrypt(plain3, enc3);
    enc_clock = clock() - enc_clock;

    /***** Evaluation *****/
    clock_t eval_clock = clock();
    Ciphertext enc_sum, enc_result;
    evaluator.add(enc1, enc2, enc_sum);
    evaluator.multiply(enc_sum, enc3, enc_result);
    evaluator.relinearize_inplace(enc_result, relin_keys);
    evaluator.mod_switch_to_next_inplace(enc_result);
    eval_clock = clock() - eval_clock;

    /***** Decryption & Decoding *****/
    clock_t dec_clock = clock();
    Plaintext plain_result;
    decryptor.decrypt(enc_result, plain_result);
    vector<uint64_t> result;
    batch_encoder.decode(plain_result, result);
    dec_clock = clock() - dec_clock;

    /***** Output *****/
    cout << "Input1: " << endl;
    print_matrix(input1, row_size);
    cout << "Input2: " << endl;
    print_matrix(input2, row_size);
    cout << "Input3: " << endl;
    print_matrix(input3, row_size);
    cout << "Result = (Input1 + Input2) * Input3: " << endl;
    print_matrix(result, row_size);
    cout << "Memory Usage: " << get_mem_usage() - baseline << " KB" << endl;
    cout << "Timings:" << endl;
    cout << "Param Gen  : " << ((float)param_clock)/CLOCKS_PER_SEC << " s" << endl;
    cout << "Key Gen    : " << ((float)key_clock)/CLOCKS_PER_SEC << " s" << endl;
    cout << "Encryption : " << ((float)enc_clock)/CLOCKS_PER_SEC << " s" << endl;
    cout << "Evaluation : " << ((float)eval_clock)/CLOCKS_PER_SEC << " s" << endl;
    cout << "Decryption : " << ((float)dec_clock)/CLOCKS_PER_SEC << " s" << endl;

    return 0;
}
