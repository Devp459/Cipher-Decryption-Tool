#pragma once

#include <cassert>
#include <cmath>
#include <functional>
#include <random>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

// Don't modify this! It's a hack to let us directly test the main function,
// while obeying the rule that a program can only have one main.
#ifdef COMPILED_FOR_GTEST
#define main ciphers_main
#endif

using namespace std;

// This file is OUT OF SCOPE for you! Feel free to read, but you only have to
// use these as directed in the project guide. You don't need to understand
// or reimplement this file.

class QuadgramScorer {
 private:
  // Map from quadgram to log likelihood
  unordered_map<string, double> likelihoods;
  // Extremely small log likelihood for quadgrams we don't know about
  double notFoundLikelihood;

  // Lightly enforce singleton pattern
  QuadgramScorer(const QuadgramScorer&);
  QuadgramScorer& operator=(const QuadgramScorer&);

 public:
  QuadgramScorer(const vector<string>& quadgrams, const vector<int>& counts)
      : likelihoods() {
    double total = 0;
    for (int c : counts) {
      total += c;
    }

    notFoundLikelihood = -log10(total);
    for (size_t i = 0; i < quadgrams.size(); i++) {
      likelihoods.emplace(quadgrams.at(i),
                          (double)log10(counts.at(i)) + notFoundLikelihood);
    }
  }

  /**
   * Return the log likelihood of the given quadgram.
   */
  double getScore(const string& quadgram) const {
    // Scoring is only valid for quadgrams! Make sure that we're
    // only scoring quadgrams, and bailing if we aren't.
    assert(quadgram.length() == 4);
    auto res = likelihoods.find(quadgram);
    if (res == likelihoods.end()) {
      return notFoundLikelihood;
    }
    return res->second;
  }
};

class Random {
 private:
  static mt19937 rng;

 public:
  static void seed(int s) { Random::rng.seed(s); }

  static int randInt(int max) {
    // Even though mt19937 is standardized, STL distributions aren't!
    // So, unfortunately, even though this is biased, we're forced to
    // do something like this. Technically uint32_fast_t isn't...
    // totally consistent across platforms? But within reason it works.
    return rng() % (max + 1);
  }
};

/**
 * Generate a random substitution cipher key
 */
inline vector<char> genRandomSubstCipher() {
  // Fisher-Yates (https://en.wikipedia.org/wiki/Fisher%E2%80%93Yates_shuffle)
  vector<char> cipher;
  for (char c = 'A'; c <= 'Z'; c++) {
    cipher.push_back(c);
  }

  for (int i = cipher.size() - 1; i >= 1; i--) {
    size_t j = Random::randInt(i - 1);
    swap(cipher.at(i), cipher.at(j));
  }

  return cipher;
}
