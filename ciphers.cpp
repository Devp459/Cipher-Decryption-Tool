#include <cctype>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "include/caesar_dec.h"
#include "include/caesar_enc.h"
#include "include/subst_dec.h"
#include "include/subst_enc.h"
#include "utils.h"

using namespace std;

void decryptSubstCipherFile(const QuadgramScorer& scorer);

// Initialize random number generator in .cpp file for ODR reasons
std::mt19937 Random::rng;

/**
 * Print instructions for using the program.
 */
void printMenu() {
  cout << "Ciphers Menu" << endl;
  cout << "------------" << endl;
  cout << "C - Encrypt with Caesar Cipher" << endl;
  cout << "D - Decrypt Caesar Cipher" << endl;
  cout << "E - Compute English-ness Score" << endl;
  cout << "A - Apply Random Substitution Cipher" << endl;
  cout << "S - Decrypt Substitution Cipher from Console" << endl;
  cout << "R - Set Random Seed for Testing" << endl;
  cout << "X - Exit Program" << endl;
}

int main() {
  Random::seed(time(NULL));
  string command;
  //dictionary open
  vector<string> dictionary;
  string word;
  ifstream file("dictionary.txt");
  if (!file.is_open()) {
    cout << "Error opening dictionary file" << endl;
  }

  while (file >> word) { // Read words one by one
    dictionary.push_back(word);
  }

  file.close();
  //dictionary close

  //Englishness file open
  ifstream quadgramFile("english_quadgrams.txt");
  vector<string> quadgrams;
  vector<int> counts;

  if (!quadgramFile.is_open()) {
      cout << "Error opening quadgram file." << endl;
      return 1;
  }
  string line;
  while (getline(quadgramFile, line)) {
    size_t commaPos = line.find(',');
    if (commaPos != string::npos) {
      string quadgram = line.substr(0, commaPos);
      int count = stoi(line.substr(commaPos + 1));
      quadgrams.push_back(quadgram);
      counts.push_back(count);
    }
  }
  quadgramFile.close();
  //Englishness file close

  cout << "Welcome to Ciphers!" << endl;
  cout << "-------------------" << endl;
  cout << endl;

  do {
    printMenu();
    cout << endl << "Enter a command (case does not matter): ";

    // Use getline for all user input to avoid needing to handle
    // input buffer issues relating to using both >> and getline
    getline(cin, command);
    cout << endl;
    QuadgramScorer scorer(quadgrams, counts);
    if (command == "R" || command == "r") {
      string seed_str;
      cout << "Enter a non-negative integer to seed the random number "
              "generator: ";
      getline(cin, seed_str);
      Random::seed(stoi(seed_str));
    }
    if (command == "C" || command == "c") { // run caesar encryption
      runCaesarEncrypt();
    } else if (command == "D" || command == "d") {// run caesar decrption
      runCaesarDecrypt(dictionary);
    } else if (command == "E" || command == "e") {// run englishness test
      computeEnglishnessCommand(scorer);
      
    } else if (command == "A" || command == "a") {// run random subsitution
      applyRandSubstCipherCommand();
    } else if (command == "S" || command == "s") {// run subsitution cipher decryption on Command
      decryptSubstCipherCommand(scorer);
    } 
      else if (command == "F" || command == "f") {// run subsitution cipher decryption from file
      decryptSubstCipherFile(scorer);
      
    } 

    cout << endl;

  } while (!(command == "x" || command == "X") && !cin.eof());

  return 0;
}

// "#pragma region" and "#pragma endregion" group related functions in this file
// to tell VSCode that these are "foldable". You might have noticed the little
// down arrow next to functions or loops, and that you can click it to collapse
// those bodies. This lets us do the same thing for arbitrary chunks!
#pragma region CaesarEnc//done

string rot(const string& line, int amount) {
  // TODO: student fill this in

  string final;
  for(char i : line){
    if(isalpha(i) || isspace(i)){
      final += rot(toupper(i), amount);
    }
  }
  return final;
}

char rot(char c, int amount) {
  // TODO: student fill this in
  if(isspace(c)){
    return c;
  }
  char temp = 'A' + ((c - 'A' + amount + 26) % 26);
  return temp;
}

void runCaesarEncrypt() {
  // TODO: student fill this in
  string text;
  int change;

  cout << "Enter the text to Caesar encrypt:" << endl;
  getline(cin, text);
  cout << "Enter the number of characters to rotate by:" << endl;
  cin >> change;
  cout << rot(text, change) << endl;
}

#pragma endregion CaesarEnc

#pragma region CaesarDec//done

void rot(vector<string>& strings, int amount) {
  // TODO: student fill this in

  for(string& str : strings){
    for(char& i : str){
      if(isalpha(i)){
        i = toupper(i);// Convert to uppercase
        i = 'A' + (i - 'A' + amount) % 26;
      }
    }
  }
}

string clean(const string& s) {
  // TODO: student fill this in

  string clean;
  for(char i : s){
    if(isalpha(i)){
      clean += toupper(i);
    }
  }
  return clean;
}

vector<string> splitBySpaces(const string& s) {
  // TODO: student fill this in
  vector<string> total_words;
  string current_word;
  bool in_word = false;

  for (char c : s) {
    if (!isspace(c)) {
      // Add each character to the current word
      current_word += c;
      in_word = true;
    } else if (in_word) {
      // If there is a space push_back that word to words vector
      total_words.push_back(current_word);  
      current_word.clear();// Clear the current word for next words
      in_word = false;
    }
  }

  // If the last word did not end with a space, add it to the vector
  if (in_word) {
    total_words.push_back(current_word);
  }

  return total_words;
}

string joinWithSpaces(const vector<string>& words) {
  // TODO: student fill this in
  string result;
  for(size_t i = 0; i < words.size(); i++){

    result += words[i];// Add each word to the result
    if(i < words.size() - 1){// Add a space after each word except the last
      result += " ";
    }
      
  }
  return result;
}

int numWordsIn(const vector<string>& words, const vector<string>& dict) {
  // TODO: student fill this in
  int count = 0; // To count how many words are present in the dictionary
  for (const string& word : words) {
    // Check if the word exists in the dictionary
    
    for (const string& dict_word : dict) {
      if (word == dict_word) {// Check if the word is in the dictionary
        count++;
        break; 
      }
    }
  }
  return count;
}

void runCaesarDecrypt(const vector<string>& dict) {
  // TODO: student fill this in

  string text;
  bool check = false;

  cout << "Enter the text to Caesar decrypt:" << endl;
  getline(cin, text);


  vector<string> words = splitBySpaces(text);
  
  for(int shift = 0; shift < 26; shift++){
    vector<string> decrypt_words = words; // Start with the original words
    rot(decrypt_words, shift);
    int word_count = numWordsIn(decrypt_words, dict);

    if(word_count > decrypt_words.size() / 2){// More than half the words are valid
      cout << joinWithSpaces(decrypt_words) << endl;
      check = true;
    }
  }
  if(!check){
    cout << "No good decryptions found" << endl;
    
  }
}

#pragma endregion CaesarDec

#pragma region SubstEnc//done

string applySubstCipher(const vector<char>& cipher, const string& s) {
  // TODO: student fill this in
  string result;
  for(char c : s){
    int index = ALPHABET.find(toupper(c));
    if(index != string::npos){
      result += cipher[index];
    }
    else{
      result += c;
    }
  }
  return result;
}


void applyRandSubstCipherCommand() {
  // TODO: student fill this in
  
  string input;
  cout << "Enter the text to substitution-cipher encrypt: ";
  getline(cin, input);
  cout << endl;
  string encr_text = applySubstCipher(genRandomSubstCipher(), input);
  cout << encr_text << endl;
}

#pragma endregion SubstEnc

#pragma region SubstDec//done

double scoreString(const QuadgramScorer& scorer, const string& s) {
  // TODO: student fill this in

  double totalScore = 0.0;
  string result;
  
  // Remove non-letter characters and convert to uppercase
  for (char c : s) {
    if (isalpha(c)) {
      result += toupper(c);
    }
  }

  // Calculate score by evaluating quadgrams
  for (size_t i = 0; i <= result.length() - 4; i++) {
    string quadgram = result.substr(i, 4);
    totalScore += scorer.getScore(quadgram);
  }

  return totalScore;
}

void computeEnglishnessCommand(const QuadgramScorer& scorer) {
  // TODO: student fill this in
  string input;
  cout << "Enter a string to score: ";
  getline(cin, input);

  double score = scoreString(scorer, input);
  cout << "Englishness: " << score << endl;
}

vector<char> decryptSubstCipher(const QuadgramScorer& scorer,
                                const string& ciphertext) {
  // TODO: student fill this in
  // return vector<char>{};

  //start the best key from A - Z from Alphabet
  int z = 0;
  vector<char> bestkey(26);
  for(char c : ALPHABET){
    bestkey[z] = c;
    z++;
  }
  //Swap the letters in bestkey Array 3 times to create a random key to start with
  for (int i = 0; i < 78; i++) {
    int index1 = Random::randInt(25);
    int index2;
    do {
      index2 = Random::randInt(25);
    } while (index1 == index2);

    swap(bestkey[index1], bestkey[index2]);
  }

  vector<char> currentKey = bestkey;
  string decryptedText = applySubstCipher(currentKey, ciphertext);
  double bestScore = scoreString(scorer, decryptedText);

  int trialsWithoutImprovement = 0;
  const int maxTrialsWithoutImprovement = 1500;

  while (trialsWithoutImprovement < maxTrialsWithoutImprovement) {
    // Generate two random indices and swap characters in currentKey
    int index1 = Random::randInt(25);
    int index2;
    do {
      index2 = Random::randInt(25);
    } while (index1 == index2);

    swap(currentKey[index1], currentKey[index2]);

    // Decrypt with the new key and score it
    string newDecryptedText = applySubstCipher(currentKey, ciphertext);
    double newScore = scoreString(scorer, newDecryptedText);

      
    // If the new key is better, accept it
    if (newScore > bestScore) {
      bestScore = newScore;
      bestkey = currentKey;
      trialsWithoutImprovement = 0; // Reset counter
    } else {
      // Revert the swap if not better
      swap(currentKey[index1], currentKey[index2]);
      trialsWithoutImprovement++;
    }
  }

  return bestkey;
}

void decryptSubstCipherCommand(const QuadgramScorer& scorer) {
  // TODO: student fill this in

  string ciphertext;
  cout << "Enter text to substitution-cipher decrypt: ";
  getline(cin, ciphertext);

  // Convert text to uppercase and preserve non-letter characters
  for (char& c : ciphertext) {
    if (islower(c)) {
      c = toupper(c);
    }
  }

  vector<char> bestOverallKey;
  double bestOverallScore = -1e9;

  // Run hill-climbing 20 times and take the best result
  for (int i = 0; i < 20; i++) {
    vector<char> key = decryptSubstCipher(scorer, ciphertext);
    string decryptedText = applySubstCipher(key, ciphertext);
    double score = scoreString(scorer, decryptedText);

    if (score > bestOverallScore) {
      bestOverallScore = score;
      bestOverallKey = key;
    }
  }

  string bestDecryptedText = applySubstCipher(bestOverallKey, ciphertext);
  cout << bestDecryptedText << endl;

}

void decryptSubstCipherFile(const QuadgramScorer& scorer){

  string inputFilename, outputFilename;
  cout << "Enter the input filename: ";
  getline(cin, inputFilename);
  cout << "Enter the output filename: ";
  getline(cin, outputFilename);

  ifstream inputFile(inputFilename);
  if (!inputFile.is_open()) {
    cerr << "Error opening input file." << endl;
    return;
  }

  string ciphertext((istreambuf_iterator<char>(inputFile)), istreambuf_iterator<char>());
  inputFile.close();

  // Convert text to uppercase
  for (char& c : ciphertext) {
    if (islower(c)) {
      c = toupper(c);
    }
  }

  vector<char> bestOverallKey;
  double bestOverallScore = -1e9;

  // Run hill-climbing 20 times and take the best result
  for (int i = 0; i < 20; i++) {
    vector<char> key = decryptSubstCipher(scorer, ciphertext);
    string decryptedText = applySubstCipher(key, ciphertext);
    double score = scoreString(scorer, decryptedText);

    if (score > bestOverallScore) {
      bestOverallScore = score;
      bestOverallKey = key;
    }
  }

  string bestDecryptedText = applySubstCipher(bestOverallKey, ciphertext);

  ofstream outputFile(outputFilename);
  if (!outputFile.is_open()) {
    cerr << "Error opening output file." << endl;
    return;
  }

  outputFile << bestDecryptedText;
  outputFile.close();

}

#pragma endregion SubstDec
