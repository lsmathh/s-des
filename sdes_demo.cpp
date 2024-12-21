#include "sdes.h"
#include <iostream>
#include <vector>

using namespace std;

void execute_sdes_process() {
  // Chave de 10 bits fornecida pelo usuário
  vector<int> user_key_10bit = {1, 0, 1, 0, 0, 0, 0, 0, 1, 0};

  cout << "Chave fornecida: ";
  size_t index = 0;
  while (index < user_key_10bit.size()) {
    cout << user_key_10bit[index];
    index++;
  }
  cout << endl;

  // Geração das subchaves de 8 bits
  generate_subkeys(user_key_10bit);

  // Texto claro fornecido pelo usuário
  vector<int> user_plaintext = {1, 1, 0, 1, 0, 1, 1, 1};

  cout << "Texto em claro: ";
  index = 0;
  while (index < user_plaintext.size()) {
    cout << user_plaintext[index];
    index++;
  }
  cout << endl;

  // Processo de encriptação
  vector<int> encrypted_text = encrypt_sdes(user_plaintext);
  cout << "Texto cifrado: ";
  index = 0;
  while (index < encrypted_text.size()) {
    cout << encrypted_text[index];
    index++;
  }
  cout << endl;

  // Processo de decriptação
  vector<int> decrypted_text = decrypt_sdes(encrypted_text);
  cout << "Texto decifrado: ";
  index = 0;
  while (index < decrypted_text.size()) {
    cout << decrypted_text[index];
    index++;
  }
  cout << endl;
}

