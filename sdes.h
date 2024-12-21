#ifndef SDES_H
#define SDES_H

#include <vector>
#include <string>

using namespace std;

// Declarações de variáveis globais
extern vector<vector<int>> S_BOX_0, S_BOX_1;
extern vector<int> P10, P8, IP, EP, P4, IP_INVERSE;
extern vector<int> subkey_1, subkey_2;

// Declarações de funções
string binary_from_decimal(int value);
vector<int> swap_nibble(const vector<int>& bit_array, int nibble_size);
int decimal_from_binary(const vector<int>& bit_vector);
vector<int> sdes_round(const vector<int>& input_bits, const vector<int>& round_key);
vector<int> decrypt_sdes(const vector<int>& ciphertext);
vector<int> encrypt_sdes(const vector<int>& plaintext);
vector<int> circular_shift(const vector<int>& bits, int shift_amount);
void generate_subkeys(const vector<int>& key_10bit);

#endif // SDES_H
