package datastructures.concrete.dictionaries;

import datastructures.interfaces.IDictionary;
import misc.exceptions.NoSuchKeyException;

import java.util.NoSuchElementException;
import java.util.Objects;

import java.util.Iterator;

/**
 * @see IDictionary and the assignment page for more details on what each method should do
 */
public class ChainedHashDictionary<K, V> implements IDictionary<K, V> {
    private final double lambda;
    private int capacity;
    private int size;
    // You MUST use this field to store the contents of your dictionary.
    // You may not change or rename this field: we will be inspecting
    // it using our private tests.
    private IDictionary<K, V>[] chains;

    // You're encouraged to add extra fields (and helper methods) though!!

    public ChainedHashDictionary() {
        this(0.75);
    }

    public ChainedHashDictionary(double lambda) {
        this.lambda = lambda;
        this.capacity = 16;
        chains = makeArrayOfChains(capacity);
        this.size = 0;
    }

    /**
     * This method will return a new, empty array of the given size
     * that can contain IDictionary<K, V> objects.
     * <p>
     * Note that each element in the array will initially be null.
     */
    @SuppressWarnings("unchecked")
    private IDictionary<K, V>[] makeArrayOfChains(int arraySize) {
        // Note: You do not need to modify this method.
        // See ArrayDictionary's makeArrayOfPairs(...) method for
        // more background on why we need this method.
        return (IDictionary<K, V>[]) new IDictionary[arraySize];
    }

    // returns an index to insert the key at
    private int keyHash(K key) {
        if (Objects.equals(key, null)) {
            return 0;
        } else {
            return Math.abs(key.hashCode() % capacity);
        }
    }

    // Pre: the key exists in the data structure, otherwise throws NoSuchKeyException
    // Post: returns the value associated with the key
    @Override
    public V get(K key) {
        if (!containsKey(key)) {
            throw new NoSuchKeyException();
        }
        // for (KVPair<K, V> pair : chains[keyHash(key)]) {
        //     if (Objects.equals(pair.getKey(), key)) {
        //         return pair.getValue();
        //     }
        // }
        // return null;
        return chains[keyHash(key)].get(key);
    }

    // inserts given pair of key and value into the data structure
    // if the key already exists, it overrides the value
    @Override
    public void put(K key, V value) {
        // if we reach thresh hold
        int keyHash = keyHash(key);
        if (chains[keyHash] == null) {
            chains[keyHash] = new ArrayDictionary<>();
        }
        if (chains[keyHash].containsKey(key)) {
            size--;
        }
        chains[keyHash].put(key, value);
        size++;
        if (1.0 * size >= capacity * lambda) {
            IDictionary<K, V>[] temp = makeArrayOfChains(capacity * 2);
            capacity = capacity * 2;
            for (KVPair<K, V> pair : this) {
                keyHash = keyHash(pair.getKey());
                if (temp[keyHash] == null) {
                    temp[keyHash] = new ArrayDictionary<>();
                }
                temp[keyHash].put(pair.getKey(), pair.getValue());
            }
            chains = temp;
        }
    }

    // Pre: the key already exists in the data structure
    // Post: removes the pair of key and value from the data structure
    @Override
    public V remove(K key) {
        if (!containsKey(key)) {
            throw new NoSuchKeyException();
        }
        size--;
        return chains[keyHash(key)].remove(key);
    }

    // returns true if the key is in the structure, false otherwise
    @Override
    public boolean containsKey(K key) {
        int keyHash = keyHash(key);
        // if (chains[keyHash] == null) {
        //     return false;
        // }
        return chains[keyHash] != null && chains[keyHash].containsKey(key);
    }

    // returns the number of elements in the data structure
    @Override
    public int size() {
        return this.size;
    }

    // if the key is in the data structure, return its value
    // otherwise return the defaultValue
    public V getOrDefault(K key, V defaultValue) {
        return containsKey(key) ? get(key) : defaultValue;
    }

    // returns an instance of an iterator
    public Iterator<KVPair<K, V>> iterator() {
        // Note: you do not need to change this method
        return new ChainedIterator<>(this.chains, this.size());
    }


    /**
     * Hints:
     * <p>
     * 1. You should add extra fields to keep track of your iteration
     * state. You can add as many fields as you want. If it helps,
     * our reference implementation uses three (including the one we
     * gave you).
     * <p>
     * 2. Before you try and write code, try designing an algorithm
     * using pencil and paper and run through a few examples by hand.
     * <p>
     * We STRONGLY recommend you spend some time doing this before
     * coding. Getting the invariants correct can be tricky, and
     * running through your proposed algorithm using pencil and
     * paper is a good way of helping you iron them out.
     * <p>
     * 3. Think about what exactly your *invariants* are. As a
     * reminder, an *invariant* is something that must *always* be
     * true once the constructor is done setting up the class AND
     * must *always* be true both before and after you call any
     * method in your class.
     * <p>
     * Once you've decided, write them down in a comment somewhere to
     * help you remember.
     * <p>
     * You may also find it useful to write a helper method that checks
     * your invariants and throws an exception if they're violated.
     * You can then call this helper method at the start and end of each
     * method if you're running into issues while debugging.
     * <p>
     * (Be sure to delete this method once your iterator is fully working.)
     * <p>
     * Implementation restrictions:
     * <p>
     * 1. You **MAY NOT** create any new data structures. Iterators
     * are meant to be lightweight and so should not be copying
     * the data contained in your dictionary to some other data
     * structure.
     * <p>
     * 2. You **MAY** call the `.iterator()` method on each IDictionary
     * instance inside your 'chains' array, however.
     */
    private static class ChainedIterator<K, V> implements Iterator<KVPair<K, V>> {
        private IDictionary<K, V>[] chains;
        private int size;
        private int index;
        private Iterator<KVPair<K, V>> iterator;

        // chains = data structure
        // size = number of elements
        // index = current chain we are on
        // count = number of elements we parsed through
        // iterator = individual chain iterator
        public ChainedIterator(IDictionary<K, V>[] chains, int size) {
            this.chains = chains;
            this.size = size;
            index = 0;
            if (size != 0) {
                while (index < chains.length && chains[index] == null) {
                    index++;
                }
                iterator = chains[index].iterator();
            }
        }

        // returns true if there are more elements to parse through
        @Override
        public boolean hasNext() {
            return size > 0;
        }

        // if there are no more elements to go through, throws NoSuchElementException
        // returns the next given element
        @Override
        public KVPair<K, V> next() {
            if (!hasNext()) {
                throw new NoSuchElementException();
            }
            if (iterator.hasNext()) {
                size--;
                return iterator.next();
            } else {
                index++;
                while (index < chains.length - 1 && chains[index] == null) {
                    index++;
                }
                iterator = chains[index].iterator();
                return next();
            }
        }
    }
}
