#include "sdes.h"
#include <iostream>
#include <vector>
#include <string>

using namespace std;

// tabelas s_boxes e permutação
// As tabelas S_BOX_0 e S_BOX_1 são usadas para substituição durante as
// operações do SDES
vector<vector<int>> S_BOX_0 = {{1, 0, 3, 2},
                               {3, 2, 1, 0},
                               {0, 2, 1, 3},
                               {3, 1, 3, 2}};
vector<vector<int>> S_BOX_1 = {{0, 1, 2, 3},
                               {2, 0, 1, 3},
                               {3, 0, 1, 0},
                               {2, 1, 0, 3}};
// Vetores de permutação são usados para reorganizar bits em várias etapas do algoritmo
vector<int> P10 = {3, 5, 2, 7, 4, 10, 1, 9, 8, 6};
vector<int> P8 = {6, 3, 7, 4, 8, 5, 10, 9};
vector<int> IP = {2, 6, 3, 1, 4, 8, 5, 7};
vector<int> EP = {4, 1, 2, 3, 2, 3, 4, 1};
vector<int> P4 = {2, 4, 3, 1};
vector<int> IP_INVERSE = {4, 1, 3, 5, 7, 2, 8, 6};

// variáveis guarda das chaves geradas de 8 bits
// As subchaves K1 e K2 serão geradas para uso durante as rodadas do SDES
vector<int> subkey_1(8), subkey_2(8);

// Converte um valor decimal (0-3) em uma string binária de 2 bits
string binary_from_decimal(int value) {
  static const string binary_map[] = {"00", "01", "10", "11"};
  return (value >= 0 && value < 4) ? binary_map[value] : "Invalid";
}

// Troca os nibbles (metades de 4 bits) de um vetor de bits
vector<int> swap_nibble(const vector<int>& bit_array, int nibble_size) {
  vector<int> swapped_bits(2 * nibble_size);
  for (int i = 0; i < nibble_size; i++) {
    swapped_bits[i] = bit_array[i + nibble_size];
    swapped_bits[i + nibble_size] = bit_array[i];
  }
  return swapped_bits;
}

// converte um vetor binário em seu valor decimal equivalente
int decimal_from_binary(const vector<int>& bit_vector) {
  int decimal_value = 0;
  for (int bit : bit_vector) {
    decimal_value = (decimal_value << 1) | bit;
  }
  return decimal_value;
}

// executa uma rodada do SDES com um conjunto de bits de entrada e uma chave de rodada
vector<int> sdes_round(const vector<int>& input_bits, const vector<int>& round_key) {
  // Divide o vetor de entrada em duas metades
  vector<int> left_half(input_bits.begin(), input_bits.begin() + 4);
  vector<int> right_half(input_bits.begin() + 4, input_bits.end());

  // Expande e permuta os bits da metade direita usando EP
  vector<int> expanded_right(8);
  for (int i = 0; i < 8; i++) {
    expanded_right[i] = right_half[EP[i] - 1];
  }

  // Realiza a operação XOR entre os bits expandidos e a chave de rodada
  for (int i = 0; i < 8; i++) {
    expanded_right[i] ^= round_key[i];
  }

  // Função lambda para processar os bits através de uma S-Box
  auto process_sbox = [](const vector<int>& bits, const vector<vector<int>>& sbox) {
    int row = decimal_from_binary({bits[0], bits[3]});
    int column = decimal_from_binary({bits[1], bits[2]});
    return binary_from_decimal(sbox[row][column]);
  };

  // Processa as duas metades da saída da expansão através das S-Boxes
  string sbox_left_output = process_sbox(vector<int>(expanded_right.begin(), expanded_right.begin() + 4), S_BOX_0);
  string sbox_right_output = process_sbox(vector<int>(expanded_right.begin() + 4, expanded_right.end()), S_BOX_1);

  // Converte os resultados das S-Boxes de volta para bits
  vector<int> sbox_output(4);
  for (int i = 0; i < 2; i++) {
    sbox_output[i] = sbox_left_output[i] - '0';
    sbox_output[i + 2] = sbox_right_output[i] - '0';
  }

  // Permuta a saída das S-Boxes usando P4
  vector<int> permuted_sbox_output(4);
  for (int i = 0; i < 4; i++) {
    permuted_sbox_output[i] = sbox_output[P4[i] - 1];
  }

  // Aplica XOR entre os bits permutados e a metade esquerda original
  for (int i = 0; i < 4; i++) {
    left_half[i] ^= permuted_sbox_output[i];
  }

  // Combina as metades para formar a saída da rodada
  vector<int> round_output(8);
  for (int i = 0; i < 4; i++) {
    round_output[i] = left_half[i];
    round_output[i + 4] = right_half[i];
  }

  return round_output;
}

vector<int> decrypt_sdes(const vector<int>& ciphertext) {
  // Realiza a permutação inicial nos bits do texto cifrado com base na tabela IP
  vector<int> permuted_bits(8);
  for (int i = 0; i < 8; i++) {
    permuted_bits[i] = ciphertext[IP[i] - 1];
  }

  vector<int> first_round_output = sdes_round(permuted_bits, subkey_2);
  vector<int> swapped_output = swap_nibble(first_round_output, 4);
  vector<int> second_round_output = sdes_round(swapped_output, subkey_1);

  // Realiza a permutação inversa nos bits após as duas rodadas
  vector<int> decrypted_bits(8);
  for (int i = 0; i < 8; i++) {
    decrypted_bits[i] = second_round_output[IP_INVERSE[i] - 1];
  }

  return decrypted_bits;
}

vector<int> encrypt_sdes(const vector<int>& plaintext) {
  // Realiza a permutação inicial nos bits do texto original com base na tabela IP
  vector<int> permuted_bits(8);
  for (int i = 0; i < 8; i++) {
    permuted_bits[i] = plaintext[IP[i] - 1];
  }

  vector<int> first_round_output = sdes_round(permuted_bits, subkey_1);
  vector<int> swapped_output = swap_nibble(first_round_output, 4);
  vector<int> second_round_output = sdes_round(swapped_output, subkey_2);

  // Realiza a permutação inversa nos bits após as duas rodadas
  vector<int> ciphertext(8);
  for (int i = 0; i < 8; i++) {
    ciphertext[i] = second_round_output[IP_INVERSE[i] - 1];
  }

  return ciphertext;
}

vector<int> circular_shift(const vector<int>& bits, int shift_amount) {
  // Ajusta a quantidade de deslocamento para garantir que seja um valor válido dentro do tamanho do vetor
  shift_amount %= bits.size();

  // Realiza o deslocamento circular dos bits
  vector<int> shifted_bits(bits.begin() + shift_amount, bits.end()); // Parte direita do vetor após o deslocamento
  shifted_bits.insert(shifted_bits.end(), bits.begin(), bits.begin() + shift_amount); // Parte esquerda do vetor após o deslocamento
  return shifted_bits;
}

void generate_subkeys(const vector<int>& key_10bit) {
  // Realiza a permutação da chave de 10 bits com base na tabela P10
  vector<int> permuted_key(10);
  for (int i = 0; i < 10; i++) {
    permuted_key[i] = key_10bit[P10[i] - 1];
  }

  // Divide a chave permutada em duas metades de 5 bits cada
  vector<int> left_half(permuted_key.begin(), permuted_key.begin() + 5);
  vector<int> right_half(permuted_key.begin() + 5, permuted_key.end());

  // Função auxiliar para criar subchaves a partir das metades da chave
  auto create_subkey = [](const vector<int>& left, const vector<int>& right, int shift_amount) {
    // Realiza o deslocamento circular nas duas metades da chave
    vector<int> shifted_left = circular_shift(left, shift_amount);
    vector<int> shifted_right = circular_shift(right, shift_amount);
    

    // Combina as metades deslocadas para formar a chave final
    vector<int> combined_key = shifted_left;
    combined_key.insert(combined_key.end(), shifted_right.begin(), shifted_right.end());
    return combined_key; // Retorna a chave combinada
  };

  // Gera a primeira subchave (K1) realizando um deslocamento de 1 bit nas metades
  vector<int> first_combined_key = create_subkey(left_half, right_half, 1);
  for (int i = 0; i < 8; i++) {
    subkey_1[i] = first_combined_key[P8[i] - 1]; // Aplica a permutação P8 para obter K1
  }

  // Gera a segunda subchave (K2) realizando um deslocamento de 3 bits nas metades
  vector<int> second_combined_key = create_subkey(left_half, right_half, 3);
  for (int i = 0; i < 8; i++) {
    subkey_2[i] = second_combined_key[P8[i] - 1]; // Aplica a permutação P8 para obter K2
  }

  // Exibe as subchaves geradas
  cout << "K1: ";
  for (auto bit : subkey_1) {
    cout << bit;
  }
  cout << endl;

  cout << "K2: ";
  for (auto bit : subkey_2) {
    cout << bit;
  }
  cout << endl;
}
