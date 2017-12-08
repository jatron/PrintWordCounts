#include <iostream>
#include <cassert>
#include <climits>
#include <fstream>

using namespace std;

#define TEST

class HashMap {
  // Abstraction Function:
  // - represents a HashMap where the keys are strings and the values are
  //   uint32_ts

  // Representation Invariant:
  // 1 the value associated with each non NULL key key in keys is
  //   values[indexof(key)]
  // 2 the number of keys in the HashTable is equivalent to size
  // 3 every non NULL key key in keys is stored in the first size indices of
  //   keyArray
  // 4 the array keys and the array values have size tableSize, and the array
  //   keyArray has size (tableSize / 8)
  // 5 size is always less than or equal to (tableSize / 8)

private:
  uint32_t size;
  uint32_t tableSize;
  string* keys;
  uint32_t* values;
  string* keyArray;

  bool inKeyArray(string key) {
    for (uint32_t i = 0; i < size; i++) {
      if ((keyArray[i]).compare(key) == 0) {
        return true;
      }
    }
    return false;
  }

  bool inKeys(string key) {
    for (uint32_t i = 0; i < tableSize; i++) {
      if ((keys[i]).compare(key) == 0) {
        return true;
      }
    }
    return false;
  }

  uint32_t hashFunction(string key) {
    // TODO: handle table resizing
    // TODO: use the last characters to compute the hash
    return 0;
  }

  void checkRep() {

    uint32_t nonNullKeyCount = 0;
    uint32_t nonZeroValueCount = 0;
    char nullChar[2] = {'\0'};
    string nullString(nullChar);
    for (uint32_t i = 0; i < tableSize; i++) {
      if (keys[i].compare(nullString) != 0) {
        nonNullKeyCount++;
        // check that every non NULL key in keys has value greater or equal to 1
        assert(values[i] >= 1);
        // check that every non NULL key in keys is in keyArray
        assert(this->inKeyArray(keys[i]));
      }
      if (values[i] != 0) {
        nonZeroValueCount++;
      }
    }

    // check that there are size non NULL strings in keys an size non zero
    // uint32_ts in values
    assert(nonNullKeyCount == size);
    assert(nonZeroValueCount == size);

    for (uint32_t i = 0; i < (tableSize / 8); i++) {
      if (i < size) {
        // check that there are size non NULL strings in keyArray
        assert(keyArray[i].compare(nullString) != 0);
        // check that every non NULL key in keyArray is in keys
        assert(this->inKeys(keyArray[i]));
      } else {
        assert(keyArray[i].compare(nullString) == 0);
      }
    }

    // check that size is less than or equal to (tableSize / 8)
    assert(size <= (tableSize / 8));

    // check that every string appears only once in keys and keyArray
    // TODO
  }

public:
  HashMap() {
    const uint32_t startingSize = 256;
    size = 0;
    tableSize = startingSize;
    keys = new string[startingSize];
    values = new uint32_t[startingSize];
    keyArray = new string[startingSize / 8];
    assert(startingSize % 8 == 0);
    // initialize keys, values, and keyArray
    char nullChar[2] = {'\0'};
    string nullString(nullChar);
    for (uint32_t i = 0; i < startingSize; i++) {
      keys[i] = nullString;
      values[i] = 0;
    }
    for (uint32_t i = 0; i < (startingSize / 8); i++) {
      keyArray[i] = nullString;
    }
    checkRep();
  }

  ~HashMap() {
    delete [] keys;
    delete [] values;
    delete [] keyArray;
  }

  static HashMap* makeFromFile(string fileName) {
    HashMap* newHashMap = new HashMap();

    ifstream file;
    file.open(fileName);
    if (!file) {
      cout << "Unable to open file" << endl;
      exit(1); // terminate with error
    }
    string word;
    while (file >> word) {
      newHashMap->increment(word);
    }
    file.close();

    newHashMap->checkRep();
    return newHashMap;
  }

  uint32_t getSize() {
    return size;
  }

  void getKeyArray(string* keyArray) {
    for (uint32_t i = 0; i < size; i++) {
      keyArray[i] = this->keyArray[i];
    }
  }

  bool equals(HashMap* otherHashMap) {
    // check that all key-value pairs in this match a key-value pair in
    // otherHashMap
    for (uint32_t i = 0; i < size; i++) {
      string key = this->keyArray[i];
      if (this->at(key) != otherHashMap->at(key)) {
        return false;
      }
    }
    // check that all key-value pairs in otherHashMap match a key-value pair in
    // this
    string otherHashMapKeyArray[otherHashMap->getSize()];
    otherHashMap->getKeyArray(otherHashMapKeyArray);
    for (uint32_t i = 0; i < otherHashMap->getSize(); i++) {
      string key = otherHashMapKeyArray[i];
      if (otherHashMap->at(key) != this->at(key)) {
        return false;
      }
    }
    return true;
  }

  uint32_t at(string key) {
    uint32_t i = hashFunction(key);
    do {
      if (keys[i].compare(key) == 0) {
        return values[i];
      }
      // miss, check the next item
      i++;
      i %= tableSize;
    } while (1);
    assert(false);
    return 0;
  }

  void increment(string key) {
    // TODO: handle table resizing
    // open addressing with linear probing
    uint32_t i = hashFunction(key);
    char nullChar[2] = {'\0'};
    string nullString(nullChar);
    do {
      if (keys[i].compare(nullString) == 0) {
        keys[i] = key;
        assert(values[i] == 0);
        values[i] = 1;
        assert(keyArray[size].compare(nullString) == 0);
        keyArray[size] = key;
        size++;
        break;
      }
      // conflict, check the next item
      i++;
      i %= tableSize;
    } while (1);
    checkRep();
  }

  void print() {
    for (uint32_t i = 0; i < size; i++) {
      string key = keyArray[i];
      cout << key << " : " << at(key) << endl;
    }
  }
};

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

void printWordCounts(HashMap* wordCounts, string* words, uint32_t numberOfWords) {
  for (uint32_t i = 0; i < numberOfWords; i++) {
    cout << words[i] << " : " << wordCounts->at(words[i]) << endl;
  }
}

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
  cout << "Running HashMap_makeFromFile_test0" << endl;

  HashMap* wordCounts = HashMap::makeFromFile("words.txt");

  HashMap* expectedWordCounts = new HashMap();
  expectedWordCounts->increment("Apple");
  expectedWordCounts->increment("apple");
  expectedWordCounts->increment("orange");
  expectedWordCounts->increment("apple");
  expectedWordCounts->increment("bananas");
  expectedWordCounts->increment("bananas");

  // TODO: delete these prints
  wordCounts->print();
  expectedWordCounts->print();

  assert(wordCounts->equals(expectedWordCounts));

  delete wordCounts;
  delete expectedWordCounts;

  cout << "HashMap_makeFromFile_test0 passed!" << endl;
}

int main() {
#ifdef TEST
  // mergeSort tests
  MergeSort_test0();

  // HashMap::makeFromFile tests
  HashMap_makeFromFile_test0();
#endif // TEST

  // Build the HashMap
  HashMap* wordCounts = HashMap::makeFromFile("words.txt");

  // Initialize array of sorted words
  string sortedWords[wordCounts->getSize()];
  wordCounts->getKeyArray(sortedWords);

  // Sort the array
  mergeSort(sortedWords, 0, wordCounts->getSize() - 1);

  // Print word counts
  printWordCounts(wordCounts, sortedWords, wordCounts->getSize());

  return 0;
}
