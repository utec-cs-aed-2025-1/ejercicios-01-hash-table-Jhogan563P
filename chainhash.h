#include <vector>

using namespace std;

const int maxColision = 5;
const float maxFillFactor = 0.8;

template<typename TK, typename TV>
struct ChainHashNode {
    TK key;
    TV value;
    ChainHashNode* next;
    size_t hashCode;

    ChainHashNode(TK _key, TV _value, size_t _hashCode)
        : key(_key), value(_value), hashCode(_hashCode), next(nullptr) {}
};

template<typename TK, typename TV>
class ChainHashListIterator {
private:
    typedef ChainHashNode<TK, TV>* Nodo;
    Nodo current;

public:
    ChainHashListIterator(Nodo head = nullptr) : current(head) {}

    ChainHashListIterator& operator++() {
        if(current) current = current->next;
        return *this;
    }

    bool operator!=(const ChainHashListIterator& other) const {
        return current != other.current;
    }

    ChainHashNode<TK, TV>& operator*() {
        return *current;
    }

    ChainHashNode<TK, TV>* operator->() {
        return current;
    }
};


template<typename TK, typename TV>
class ChainHash {
private:
    typedef ChainHashNode<TK, TV> Node;
    typedef ChainHashListIterator<TK, TV> Iterator;

    Node** array;
    int nsize;
    int capacity;
    int* bucket_sizes;
    int usedBuckets;

public:
    ChainHash(int initialCapacity = 10) {
        capacity = initialCapacity;
        array = new Node*[capacity]();
        bucket_sizes = new int[capacity]();
        nsize = 0;
        usedBuckets = 0;
    }

    ~ChainHash() {
        for(int i = 0; i < capacity; i++) {
            Node* current = array[i];
            while(current) {
                Node* temp = current->next;
                delete current;
                current = temp;
            }
        }
        delete[] array;
        delete[] bucket_sizes;
    }

    TV& get(TK key) {
        int index = getHashCode(key) % capacity;
        Node* current = array[index];
        while(current) {
            if(current->key == key)
                return current->value;
            current = current->next;
        }
        throw out_of_range("Key no encontrado");
    }

    bool contains(TK key) {
        int index = getHashCode(key) % capacity;
        Node* current = array[index];
        while(current) {
            if(current->key == key)
                return true;
            current = current->next;
        }
        return false;
    }

    void set(TK key, TV value) {
        if(fillFactor() > maxFillFactor) rehashing();

        int index = getHashCode(key) % capacity;
        Node* current = array[index];

        while(current) {
            if(current->key == key) {
                current->value = value;
                return;
            }
            current = current->next;
        }

        Node* newNode = new Node(key, value, getHashCode(key));
        newNode->next = array[index];
        array[index] = newNode;

        bucket_sizes[index]++;
        nsize++;
        if(bucket_sizes[index] == 1)
            usedBuckets++;

        if(bucket_sizes[index] > maxColision)
            rehashing();
    }

    bool remove(TK key) {
        int index = getHashCode(key) % capacity;
        Node* current = array[index];
        Node* prev = nullptr;

        while(current) {
            if(current->key == key) {
                if(prev)
                    prev->next = current->next;
                else
                    array[index] = current->next;

                delete current;
                bucket_sizes[index]--;
                if(bucket_sizes[index] == 0)
                    usedBuckets--;
                nsize--;
                return true;
            }
            prev = current;
            current = current->next;
        }
        return false;
    }

    Iterator begin(int index) {
        if(index < 0 || index >= capacity)
            throw out_of_range("Indice invalido");
        return Iterator(array[index]);
    }

    Iterator end(int) {
        return Iterator(nullptr);
    }

    int size() { return nsize; }
    int bucket_count() { return capacity; }
    int bucket_size(int index) {
        if(index < 0 || index >= capacity)
            throw out_of_range("Indice invalido");
        return bucket_sizes[index];
    }

private:
    double fillFactor() {
        return (double)usedBuckets / (double)capacity;
    }

    size_t getHashCode(TK key) {
        return hash<TK>{}(key);
    }

    void rehashing() {
        int oldCapacity = capacity;
        Node** oldArray = array;
        int* oldBucketSizes = bucket_sizes;

        capacity *= 2;
        array = new Node*[capacity]();
        bucket_sizes = new int[capacity]();
        nsize = 0;
        usedBuckets = 0;

        for(int i = 0; i < oldCapacity; i++) {
            Node* current = oldArray[i];
            while(current) {
                Node* next = current->next;

                int newIndex = current->hashCode % capacity;
                current->next = array[newIndex];
                array[newIndex] = current;

                bucket_sizes[newIndex]++;
                nsize++;
                if(bucket_sizes[newIndex] == 1)
                    usedBuckets++;

                current = next;
            }
        }

        delete[] oldArray;
        delete[] oldBucketSizes;
    }
};