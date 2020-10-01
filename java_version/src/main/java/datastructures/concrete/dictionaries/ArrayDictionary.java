package datastructures.concrete.dictionaries;

import datastructures.interfaces.IDictionary;
import misc.exceptions.NoSuchKeyException;

import java.util.NoSuchElementException;

import java.util.Iterator;
import java.util.Objects;

public class ArrayDictionary<K, V> implements IDictionary<K, V> {
    // You may not change or rename this field.
    // We will be inspecting it in our private tests.
    private Pair<K, V>[] pairs;
    private int size;

    public static final int DEFAULT_CAPACITY = 2;

    // You may add extra fields or helper methods though!

    public ArrayDictionary() {
        this(DEFAULT_CAPACITY);
    }

    public ArrayDictionary(int capacity) {
        pairs = makeArrayOfPairs(capacity);
        this.size = 0;
    }

    /**
     * This method will return a new, empty array of the given size
     * that can contain Pair<K, V> objects.
     * <p>
     * Note that each element in the array will initially be null.
     */
    @SuppressWarnings("unchecked")
    private Pair<K, V>[] makeArrayOfPairs(int arraySize) {
        // It turns out that creating arrays of generic objects in Java
        // is complicated due to something known as 'type erasure'.
        //
        // We've given you this helper method to help simplify this part of
        // your assignment. Use this helper method as appropriate when
        // implementing the rest of this class.
        //
        // You are not required to understand how this method works, what
        // type erasure is, or how arrays and generics interact. Do not
        // modify this method in any way.
        return (Pair<K, V>[]) (new Pair[arraySize]);
    }

    // Pre: the arrayDictionary contains the given key, otherwise throws NoSuchKeyException
    // Post: returns the value associated with the given key
    @Override
    public V get(K key) {
        if (!this.containsKey(key)) {
            throw new NoSuchKeyException();
        }
        for (Pair<K, V> temp : pairs) {
            if (Objects.equals(temp.key, key)) {
                return temp.value;
            }
        }
        return null;
    }

    // inserts the given pair of key and value in the dictionary
    // if the key already exists it replaces the old value with the new value
    @Override
    public void put(K key, V value) {
        // if the array is full, a new one is made with double the size
        if (this.size == pairs.length) {
            Pair<K, V>[] temp = makeArrayOfPairs(size * 2);
            int i = 0;
            while (i < this.size) {
                temp[i] = pairs[i];
                i++;
            }
            pairs = temp;
        }
        // checks if the key already exists
        boolean sameKey = false;
        for (int i = 0; i < this.size; i++) {
            if (Objects.equals(pairs[i].key, key)) {
                pairs[i].value = value;
                sameKey = true;
            }
        }
        if (!sameKey) {
            pairs[this.size] = new Pair(key, value);
            this.size++;
        }
    }

    // Pre: the arrayDictionary contains the key, otherwise throws NoSuchKeyException
    // Post: removes the pair associated with the given key from the dictionary
    //       and returns the value of the key
    @Override
    public V remove(K key) {
        if (!this.containsKey(key)) {
            throw new NoSuchKeyException();
        }
        for (int i = 0; i < size; i++) {
            if (Objects.equals(pairs[i].key, key)) {
                Pair<K, V> temp = pairs[i];
                pairs[i] = pairs[size - 1];
                size--;
                return temp.value;
            }
        }
        return null;
    }

    // returns true if the dictionary contains the key, otherwise returns false
    @Override
    public boolean containsKey(K key) {
        for (int i = 0; i < size; i++) {
            if (Objects.equals(pairs[i].key, key)) {
                return true;
            }
        }
        return false;
    }

    // returns the number of elements in the dictionary
    @Override
    public int size() {
        return size;
    }

    // if the key is in the data structure, return its value
    // otherwise return the defaultValue
    public V getOrDefault(K key, V defaultValue) {
        return containsKey(key) ? get(key) : defaultValue;
    }

    private static class Pair<K, V> {
        public K key;
        public V value;

        // You may add constructors and methods to this class as necessary.
        public Pair(K key, V value) {
            this.key = key;
            this.value = value;
        }

        @Override
        public String toString() {
            return this.key + "=" + this.value;
        }
    }

    @Override
    public Iterator<KVPair<K, V>> iterator() {
        return new ArrayDictionaryIterator(pairs, this.size);
    }

    private static class ArrayDictionaryIterator<K, V> implements Iterator<KVPair<K, V>> {
        private Pair<K, V>[] pairs;
        private int size;
        private int index;

        public ArrayDictionaryIterator(Pair<K, V>[] pairs, int size) {
            this.pairs = pairs;
            this.size = size;
            this.index = 0;
        }

        public boolean hasNext() {
            return index < size;
        }

        public KVPair<K, V> next() {
            if (pairs[index] == null) {
                throw new NoSuchElementException();
            }
            KVPair<K, V> kvPair = new KVPair<>(pairs[index].key, pairs[index].value);
            index++;
            return kvPair;
        }
    }
}
