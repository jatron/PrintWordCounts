#include <iostream>
#include <cassert>
#include <climits>
#include <fstream>

using namespace std;

#define TABLE_RESIZE_FACTOR 8

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
  //   keyArray has size (tableSize / TABLE_RESIZE_FACTOR)
  // 5 size is always less than or equal to (tableSize / TABLE_RESIZE_FACTOR)

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

  uint32_t hashFunction(string key, uint32_t tableSize) {
    // use the last characters to compute the hash
    uint32_t popcount = __builtin_popcount(tableSize - 1);
    const uint32_t bitsInChar = 8;
    uint32_t charsNeeded = (popcount / bitsInChar) + 1;
    assert(charsNeeded <= 4);
    char lastChars[charsNeeded];
    for (uint32_t i = 0; i < charsNeeded; i++) {
      int32_t charIndex = (key.length() - charsNeeded + i);
      if (charIndex >= 0) {
        lastChars[i] = key.at(charIndex);
      } else {
        lastChars[i] = (char) 0;
      }
    }
    uint32_t* n = (uint32_t*) lastChars;
    uint32_t mask = tableSize - 1; // tableSize is always a power of 2
    return (*n) & mask;
  }

  void doubleSize() {
    string* newKeys = new string[tableSize * 2];
    uint32_t* newValues = new uint32_t[tableSize * 2];
    string* newKeyArray = new string[tableSize / TABLE_RESIZE_FACTOR * 2];
    uint32_t newTableSize = tableSize * 2;
    uint32_t newSize = 0;
    // initialize newKeys, newValues, and newKeyArray
    char nullChar[2] = {'\0'};
    string nullString(nullChar);
    for (uint32_t i = 0; i < newTableSize; i++) {
      newKeys[i] = nullString;
      newValues[i] = 0;
    }
    for (uint32_t i = 0; i < (newTableSize / TABLE_RESIZE_FACTOR); i++) {
      newKeyArray[i] = nullString;
    }
    // Transfer elements to newKeys, newValues, and newKeyArray
    for (uint32_t i = 0; i < size; i++) {
      string key = keyArray[i];
      uint32_t value = at(key);
      insert(key, value, newKeys, newValues, newKeyArray, newTableSize,
          &newSize);
    }
    assert(newSize == size);
    delete [] keys;
    delete [] values;
    delete [] keyArray;
    keys = newKeys;
    values = newValues;
    keyArray = newKeyArray;
    tableSize = newTableSize;
    size = newSize;
    checkRep();
  }

  void insert(string key, uint32_t value, string* keys, uint32_t* values,
      string* keyArray, uint32_t tableSize, uint32_t* size) {
    uint32_t i = hashFunction(key, tableSize);
    char nullChar[2] = {'\0'};
    string nullString(nullChar);
    do {
      if (keys[i].compare(nullString) == 0) {
        keys[i] = key;
        assert(values[i] == 0);
        values[i] = value;
        assert(keyArray[*size].compare(nullString) == 0);
        keyArray[*size] = key;
        (*size)++;
        break;
      }
      // conflict, check the next item
      i++;
      i %= tableSize;
    } while (1);
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

    for (uint32_t i = 0; i < (tableSize / TABLE_RESIZE_FACTOR); i++) {
      if (i < size) {
        // check that there are size non NULL strings in keyArray
        assert(keyArray[i].compare(nullString) != 0);
        // check that every non NULL key in keyArray is in keys
        assert(this->inKeys(keyArray[i]));
      } else {
        assert(keyArray[i].compare(nullString) == 0);
      }
    }

    // check that size is less than or equal to (tableSize /TABLE_RESIZE_FACTOR)
    assert(size <= (tableSize / TABLE_RESIZE_FACTOR));

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
    keyArray = new string[startingSize / TABLE_RESIZE_FACTOR];
    assert(startingSize % TABLE_RESIZE_FACTOR == 0);
    // initialize keys, values, and keyArray
    char nullChar[2] = {'\0'};
    string nullString(nullChar);
    for (uint32_t i = 0; i < startingSize; i++) {
      keys[i] = nullString;
      values[i] = 0;
    }
    for (uint32_t i = 0; i < (startingSize / TABLE_RESIZE_FACTOR); i++) {
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
    uint32_t i = hashFunction(key, tableSize);
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
    // open addressing with linear probing
    uint32_t i = hashFunction(key, tableSize);
    char nullChar[2] = {'\0'};
    string nullString(nullChar);
    do {
      if (keys[i].compare(nullString) == 0) {
        if ((size + 1) >= (tableSize / TABLE_RESIZE_FACTOR)) {
          // double the size of the HashMap
          doubleSize();
          // recompute the hash
          uint32_t i = hashFunction(key, tableSize);
          continue;
        }
        keys[i] = key;
        assert(values[i] == 0);
        values[i] = 1;
        assert(keyArray[size].compare(nullString) == 0);
        keyArray[size] = key;
        size++;
        break;
      } else if (keys[i].compare(key) == 0) {
        values[i]++;
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

// HashMap::increment tests
void HashMap_increment_test0() {
  cout << "Running HashMap_increment_test0" << endl;

  HashMap* newHashMap = new HashMap();
  newHashMap->increment("apple");
  assert(newHashMap->at("apple") == 1);
  newHashMap->increment("apple");
  assert(newHashMap->at("apple") == 2);

  cout << "HashMap_increment_test0 passed!" << endl;

  delete newHashMap;
}

// HashMap::makeFromFile tests
void HashMap_makeFromFile_test0() {
  cout << "Running HashMap_makeFromFile_test0" << endl;

  HashMap* wordCounts = HashMap::makeFromFile("./test/words.txt");

  HashMap* expectedWordCounts = new HashMap();
  expectedWordCounts->increment("Apple");
  expectedWordCounts->increment("apple");
  expectedWordCounts->increment("orange");
  expectedWordCounts->increment("apple");
  expectedWordCounts->increment("bananas");
  expectedWordCounts->increment("bananas");

  assert(wordCounts->equals(expectedWordCounts));

  delete wordCounts;
  delete expectedWordCounts;

  cout << "HashMap_makeFromFile_test0 passed!" << endl;
}

int main(int argc, char *argv[]) {
#ifdef TEST
  // mergeSort tests
  MergeSort_test0();

  // HashMap::increment tests
  HashMap_increment_test0();

  // HashMap::makeFromFile tests
  HashMap_makeFromFile_test0();
#endif // TEST

  assert(argc == 2);
  string fileName(argv[1]);

  // Build the HashMap
  HashMap* wordCounts = HashMap::makeFromFile(fileName);

  // Initialize array of sorted words
  string sortedWords[wordCounts->getSize()];
  wordCounts->getKeyArray(sortedWords);

  // Sort the array
  mergeSort(sortedWords, 0, wordCounts->getSize() - 1);

  // Print word counts
  printWordCounts(wordCounts, sortedWords, wordCounts->getSize());

  return 0;
}
