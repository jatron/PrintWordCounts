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
  // - the value associated with each non NULL key key in keys is
  //   values[indexof(key)]
  // - the number of keys in the HashTable is equivalent to size
  // - every non NULL key key in keys is stored in the first size indices of
  //   keyArray
  // - the array keys and the array values have size tableSize, and the array
  //   keyArray has size (tableSize / TABLE_RESIZE_FACTOR)
  // - size is always less than or equal to (tableSize / TABLE_RESIZE_FACTOR)

  // Implementation:
  // - This is an open addressing HashMap with linear probing

private:
  uint32_t size;
  uint32_t tableSize;
  string* keys;
  uint32_t* values;
  string* keyArray;

  bool inKeyArray(string* key) {
    for (uint32_t i = 0; i < size; i++) {
      if ((keyArray[i]).compare(*key) == 0) {
        return true;
      }
    }
    return false;
  }

  bool inKeys(string* key) {
    for (uint32_t i = 0; i < tableSize; i++) {
      if ((keys[i]).compare(*key) == 0) {
        return true;
      }
    }
    return false;
  }

  void doubleSize() {
    string* newKeys = new string[tableSize * 2];
    uint32_t* newValues = new uint32_t[tableSize * 2];
    string* newKeyArray = new string[tableSize / TABLE_RESIZE_FACTOR * 2];
    uint32_t newTableSize = tableSize * 2;
    uint32_t newSize = 0;
    // initialize newKeys, newValues, and newKeyArray
    for (uint32_t i = 0; i < newTableSize; i++) {
      newKeys[i] = "\0";
      newValues[i] = 0;
    }
    for (uint32_t i = 0; i < (newTableSize / TABLE_RESIZE_FACTOR); i++) {
      newKeyArray[i] = "\0";
    }
    // Transfer elements to newKeys, newValues, and newKeyArray
    for (uint32_t i = 0; i < size; i++) {
      string key = keyArray[i];
      uint32_t value = at(&key);
      insert(&key, value, newKeys, newValues, newKeyArray, newTableSize,
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
#ifndef NDEBUG
    checkRep();
#endif
  }

  void insert(string* key, uint32_t value, string* keys, uint32_t* values,
      string* keyArray, uint32_t tableSize, uint32_t* size) {
    uint32_t i = hashFunction(key, tableSize);
    do {
      if (keys[i].compare("\0") == 0) {
        keys[i] = *key;
        assert(values[i] == 0);
        values[i] = value;
        assert(keyArray[*size].compare("\0") == 0);
        keyArray[*size] = *key;
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
    for (uint32_t i = 0; i < tableSize; i++) {
      if (keys[i].compare("\0") != 0) {
        nonNullKeyCount++;
        // check that every non NULL key in keys has value greater or equal to 1
        assert(values[i] >= 1);
        // check that every non NULL key in keys is in keyArray
        assert(this->inKeyArray(&keys[i]));
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
        assert(keyArray[i].compare("\0") != 0);
        // check that every non NULL key in keyArray is in keys
        assert(this->inKeys(&keyArray[i]));
      } else {
        assert(keyArray[i].compare("\0") == 0);
      }
    }

    // check that size is less than or equal to (tableSize /TABLE_RESIZE_FACTOR)
    assert(size <= (tableSize / TABLE_RESIZE_FACTOR));

    assert(tableSize % TABLE_RESIZE_FACTOR == 0);

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
    for (uint32_t i = 0; i < startingSize; i++) {
      keys[i] = "\0";
      values[i] = 0;
    }
    for (uint32_t i = 0; i < (startingSize / TABLE_RESIZE_FACTOR); i++) {
      keyArray[i] = "\0";
    }
#ifndef NDEBUG
    checkRep();
#endif
  }

  ~HashMap() {
    delete [] keys;
    delete [] values;
    delete [] keyArray;
  }

  static HashMap* makeFromFile(string* fileName) {
    HashMap* newHashMap = new HashMap();

    ifstream file;
    file.open(*fileName);
    if (!file) {
      cout << "Unable to open file" << endl;
      exit(1); // terminate with error
    }
    string word;
    while (file >> word) {
      newHashMap->increment(&word);
    }
    file.close();

#ifndef NDEBUG
    newHashMap->checkRep();
#endif
    return newHashMap;
  }

  static uint32_t hashFunction(string* key, uint32_t tableSize) {
    // hash = (bits in the last characters of key)
    // Example: Suppose the tableSize is 2^9 (which is equal to 512) and the key is
    // "helao"
    // If the tableSize is to 2^9, then the hash of key needs to be a 9 bit
    // unsigned integer
    // To obtain this 9 bit integer representing the hash of key, we take the
    // last 2 characters of key, and AND them with the mask 0b1_1111_1111
    // This results in 'ao' & 0b1_1111_1111 = 0x16F

    uint32_t mask = tableSize - 1;
    // tableSize is always a power of 2, so the lower order bits of mask will
    // always be 1's and the higher order bits of mask will always be 0's. No
    // 0's and 1's will be interleaved
    uint32_t popcount = __builtin_popcount(mask);
    const uint32_t bitsInChar = 8;
    uint32_t charsNeeded = (popcount / bitsInChar) + 1;
    assert(charsNeeded <= 4);
    char lastChars[charsNeeded];
    // initialize lastChars array
    for (uint32_t i = 0; i < charsNeeded; i++) {
      lastChars[i] = (char) 0;
    }
    for (uint32_t i = 0; i < charsNeeded; i++) {
      int32_t charIndex = ((*key).length() - charsNeeded + i);
      if (charIndex >= 0) {
        // Fill up lastChars from the end of the array to the beginning
        lastChars[charsNeeded - i - 1] = (*key).at(charIndex);
      }
    }
    uint32_t* n = (uint32_t*) lastChars;
    return (*n) & mask;
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
      if (this->at(&key) != otherHashMap->at(&key)) {
        return false;
      }
    }
    // check that all key-value pairs in otherHashMap match a key-value pair in
    // this
    string otherHashMapKeyArray[otherHashMap->getSize()];
    otherHashMap->getKeyArray(otherHashMapKeyArray);
    for (uint32_t i = 0; i < otherHashMap->getSize(); i++) {
      string key = otherHashMapKeyArray[i];
      if (otherHashMap->at(&key) != this->at(&key)) {
        return false;
      }
    }
    return true;
  }

  uint32_t at(string* key) {
    uint32_t i = hashFunction(key, tableSize);
    do {
      if (keys[i].compare(*key) == 0) {
        return values[i];
      }
      // miss, check the next item
      i++;
      i %= tableSize;
    } while (1);
    assert(false);
    return 0;
  }

  void increment(string* key) {
    uint32_t i = hashFunction(key, tableSize);
    do {
      if (keys[i].compare("\0") == 0) {
        if ((size + 1) >= (tableSize / TABLE_RESIZE_FACTOR)) {
          // double the size of the HashMap
          doubleSize();
          // recompute the hash
          uint32_t i = hashFunction(key, tableSize);
          continue;
        }
        keys[i] = *key;
        assert(values[i] == 0);
        values[i] = 1;
        assert(keyArray[size].compare("\0") == 0);
        keyArray[size++] = *key;
        break;
      } else if (keys[i].compare(*key) == 0) {
        values[i]++;
        break;
      }
      // conflict, check the next item
      i++;
      i %= tableSize;
    } while (1);
#ifndef NDEBUG
    checkRep();
#endif
  }

  void print() {
    for (uint32_t i = 0; i < size; i++) {
      string key = keyArray[i];
      cout << key << " : " << at(&key) << endl;
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
  L[n1] = "\0";
  R[n2] = "\0";
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

void mergeSort(string* A, int32_t p, int32_t r) {
  if (p < r) {
    uint32_t q = (p + r) / 2;
    mergeSort(A, p, q);
    mergeSort(A, q + 1, r);
    merge(A, p, q, r);
  }
}

void printWordCounts(HashMap* wordCounts, string* words, uint32_t numberOfWords) {
  for (uint32_t i = 0; i < numberOfWords; i++) {
    cout << words[i] << " : " << wordCounts->at(&words[i]) << endl;
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
  string apple = "apple";
  newHashMap->increment(&apple);
  assert(newHashMap->at(&apple) == 1);
  newHashMap->increment(&apple);
  assert(newHashMap->at(&apple) == 2);

  cout << "HashMap_increment_test0 passed!" << endl;

  delete newHashMap;
}

// HashMap::makeFromFile tests
void HashMap_makeFromFile_test0() {
  cout << "Running HashMap_makeFromFile_test0" << endl;

  string fileName = "./test/words.txt";
  HashMap* wordCounts = HashMap::makeFromFile(&fileName);

  HashMap* expectedWordCounts = new HashMap();
  string Apple = "Apple";
  string apple = "apple";
  string orange = "orange";
  string bananas = "bananas";
  expectedWordCounts->increment(&Apple);
  expectedWordCounts->increment(&apple);
  expectedWordCounts->increment(&orange);
  expectedWordCounts->increment(&apple);
  expectedWordCounts->increment(&bananas);
  expectedWordCounts->increment(&bananas);

  assert(wordCounts->equals(expectedWordCounts));

  delete wordCounts;
  delete expectedWordCounts;

  cout << "HashMap_makeFromFile_test0 passed!" << endl;
}

// HashMap::hashFunction tests
void HashMap_hashFunction_test0() {
  cout << "Running HashMap_hashFunction_test0" << endl;

  string a = "a";
  string hello = "hello";
  string helao = "helao";

  uint32_t hash = HashMap::hashFunction(&a, 268435456); // 2^28
  assert(hash == 0x61);

  hash = HashMap::hashFunction(&hello, 256); // 2^8
  assert(hash == 0x6F);

  hash = HashMap::hashFunction(&hello, 65536); // 2^16
  assert(hash == 0x6C6F);

  hash = HashMap::hashFunction(&hello, 512); // 2^9
  assert(hash == 0x6F);

  hash = HashMap::hashFunction(&helao, 512); // 2^9
  assert(hash == 0x16F);

  cout << "HashMap_hashFunction_test0 passed!" << endl;
}

int main(int argc, char *argv[]) {
#ifdef TEST
  // mergeSort tests
  MergeSort_test0();

  // HashMap::increment tests
  HashMap_increment_test0();

  // HashMap::makeFromFile tests
  HashMap_makeFromFile_test0();

  // HashMap::hashFunction tests
  HashMap_hashFunction_test0();
#endif // TEST

  assert(argc == 2);
  string fileName(argv[1]);

  // Build the HashMap
  HashMap* wordCounts = HashMap::makeFromFile(&fileName);

  // Initialize array of sorted words
  string sortedWords[wordCounts->getSize()];
  wordCounts->getKeyArray(sortedWords);

  // Sort the array
  mergeSort(sortedWords, 0, wordCounts->getSize() - 1);

  // Print word counts
  printWordCounts(wordCounts, sortedWords, wordCounts->getSize());

  return 0;
}
