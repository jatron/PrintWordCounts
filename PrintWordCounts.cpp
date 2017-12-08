#include <iostream>
#include <cassert>
#include <climits>

using namespace std;

#define TEST

class HashMap {
private:
  uint32_t size;
  uint32_t tableSize;
  string* strings;
  uint32_t* words;

  void checkRep() {
    // TODO
  }

public:
  HashMap() {
    const uint32_t startingSize = 100;
    size = 0;
    tableSize = startingSize;
    strings = malloc(sizeof(string) * startingSize);
    words = malloc(sizeof(uint32_t) * startingSize);
  }

  void put(string key, uint32_t value) {
    uint32_t hash = hashFunction(key);

  }
  // makeFromFile()
  // getKeyArray()
  // size()
  // at()
  // constructor
  // put
  // equals
}

void merge(string* A, uint32_t p, uint32_t q, uint32_t r) {
  uint32_t n1 = q - p + 1;
  uint32_t n2 = r - q;
  string L[n1 + 1];
  string R[n2 + 1];
  for (uint32_t i = 0; i < n1; i++) {
    L[i] = A[p + i];
  }
  for (uint32_t j = 0; j < n2; j++) {
    R[j] = A[q + j + 1];
  }
  char minCharArray[2] = {'\0'};
  string minString(minCharArray);
  L[n1] = minString;
  R[n2] = minString;
  uint32_t i = 0;
  uint32_t j = 0;
  for (uint32_t k = p; k <= r; k++) {
    if (L[i].compare(R[j]) >= 0) {
      A[k] = L[i++];
    } else {
      A[k] = R[j++];
    }
  }
}

void mergeSort(string* A, uint32_t p, uint32_t r) {
  if (p < r) {
    uint32_t q = (p + r) / 2;
    mergeSort(A, p, q);
    mergeSort(A, q + 1, r);
    merge(A, p, q, r);
  }
}

/*
void printWordCounts(HashMap& wordCounts, string* words, uint32_t numberOfWords) {
  for (uint32_t i = 0; i < numberOfWords; i++) {
    cout << words[i] << " : " << wordCounts.at(word) << endl;
  }
}
*/

// ========== TESTS ==========

// mergeSort tests
void MergeSort_test0() {
  cout << "Running MergeSort_test0" << endl;
  const uint32_t size = 4;
  string input[size];
  input[0] = "Apple";
  input[1] = "apple";
  input[2] = "orange";
  input[3] = "bananas";

  string expectedOutput[size];
  expectedOutput[0] = "orange";
  expectedOutput[1] = "bananas";
  expectedOutput[2] = "apple";
  expectedOutput[3] = "Apple";

  mergeSort(input, 0, size - 1);

  for (uint32_t i = 0 ; i < size; i++) {
    assert(input[i].compare(expectedOutput[i]) == 0);
  }
  cout << "MergeSort_test0 passed!" << endl;
}

// HashMap::makeFromFile tests
void HashMap_makeFromFile_test0() {
  HashMap* wordCounts = HashMap::makeFromFile("words.txt");

  HashMap* expectedWordCount = new HashMap();
  expectedWordCount.put("orange", 1);
  expectedWordCount.put("bananas", 2);
  expectedWordCount.put("apple", 2);
  expectedWordCount.put("Apple", 1);

  assert(wordCounts.equals(expectedWordCount));
}

int main() {
#ifdef TEST
  // mergeSort tests
  MergeSort_test0();

  // HashMap::makeFromFile tests
  HashMap_makeFromFile_test0();
#endif // TEST

  // Build the HashMap
  // HashMap* wordCounts = HashMap::makeFromFile("words.txt");

  // Initialize array of sorted words
  // string sortedWords[wordCounts.size()];
  // wordCounts.getKeyArray(sortedWords);

  // Sort the array
  // mergeSort(sortedWords, 0, wordCounts.size() - 1);

  // Print word counts
  // printWordCounts(wordCounts, sortedWords, wordCounts.size());

  return 0;
}
