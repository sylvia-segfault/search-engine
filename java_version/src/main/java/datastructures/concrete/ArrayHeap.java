package datastructures.concrete;

import datastructures.concrete.dictionaries.ChainedHashDictionary;
import datastructures.interfaces.IDictionary;
import datastructures.interfaces.IPriorityQueue;
import misc.exceptions.EmptyContainerException;
import misc.exceptions.InvalidElementException;
/**
 * @see IPriorityQueue for details on what each method must do.
 */
public class ArrayHeap<T extends Comparable<T>> implements IPriorityQueue<T> {
    // See spec: you must implement a implement a 4-heap.
    private static final int NUM_CHILDREN = 4;

    // You MUST use this field to store the contents of your heap.
    // You may NOT rename this field: we will be inspecting it within
    // our private tests.
    private T[] heap;

    private int size;
    private IDictionary<T, Integer> elementDictionary;

    // Feel free to add more fields and constants.

    public ArrayHeap() {
        heap = makeArrayOfT(16);
        size = 0;
        elementDictionary = new ChainedHashDictionary<>();
    }

    /**
     * This method will return a new, empty array of the given size
     * that can contain elements of type T.
     *
     * Note that each element in the array will initially be null.
     */
    @SuppressWarnings("unchecked")
    private T[] makeArrayOfT(int arraySize) {
        // This helper method is basically the same one we gave you
        // in ArrayDictionary and ChainedHashDictionary.
        //
        // As before, you do not need to understand how this method
        // works, and should not modify it in any way.
        return (T[]) (new Comparable[arraySize]);
    }

    // Pre: index < size, otherwise throws IndexOutOfBoundsException
    // Post: if the item is less than its parent, then it swaps them and percolates again
    private void percolateUp(int index) {
        if (index >= size || index < 0) {
            throw new IndexOutOfBoundsException();
        }
        int parent = (index - 1) / NUM_CHILDREN;
        swap(index, parent);
        percolate(parent);
    }

    // Pre: 0 <= index < size, otherwise throws IndexOutOfBoundsException
    // Post: if the items is greater than its children, then it swaps them and percolates again
    private void percolateDown(int index) {
        /*
        if (index != 0 && (index >= size || index < 0)) { // why index != 0
            throw new IndexOutOfBoundsException();
        }
         */

        /*
            I first wrote the code for the case if we had only 5 elements or less
            i.e just 1 parent and 4 children
            then, I generalized the code to work for every single situation
         */
        // int child = 0;
        // // special case for when we removeMin and we have to percolate down
        // if (index == 0) {
        //     // compares all the children to each other, then chooses the index of the least one
        //     if (size >= 4) {
        //         for (int i = 1; i <= 4; i++) {
        //             if (heap[i].compareTo(heap[child]) < 0) {
        //                 child = i;
        //             }
        //         }
        //     } else {
        //         for (int j = 1; j < size; j++) {
        //             if (heap[j].compareTo(heap[child]) < 0) {
        //                 child = j;
        //             }
        //         }
        //     }
        // }
        // else {
        //     child = 4 * index + 4;
        //     if (child >= size) {
        //         child = index;
        //     }
        // }

        // First, we search for every child to see the smallest one
        int child = index;
        int firstChild = 4 * index + 1;
        int lastChild = 4 * index + 4;
        if (lastChild < size) {
            // if we have 4 children go through everyone one of them
            for (int i = firstChild; i <= lastChild; i++) {
                if (heap[i].compareTo(heap[child]) < 0) {
                    child = i;
                }
            }
        } else {
            // if we have less than 4 children, it means we are near the end of the heap
            // so we only search for however many children we have, i.e till the end of the heap
            for (int j = firstChild; j < size; j++) {
                if (heap[j].compareTo(heap[child]) < 0) {
                    child = j;
                }
            }
        }

        // if the parent at current index is bigger than the smallest child we found,
        // then swap them, and percolate.
        if (heap[index].compareTo(heap[child]) > 0) {
            swap(index, child);
            percolate(child);
            // problem is when we removeMin and we replace the first item with the last item
            // it always replaces the last item with its right most child, and not the min child
        }
    }

    // percolates the item at the given index
    private void percolate(int index) {
        if (index == 0) {
            // always percolate down when index is 0
            percolateDown(index);
        } else {
            int parent = (int) Math.floor((index - 1) / NUM_CHILDREN);
            if (heap[index].compareTo(heap[parent]) < 0) {
                // if the item is smaller than its parent, go up
                percolateUp(index);
            } else {
                // if the item is less than its parent, then percolate down
                percolateDown(index);
            }
        }
    }

    // Pre: 0 <= a < size, 0 <= b < size, otherwise throws IndexOutOfBoundsException
    // Post: swaps 2 elements of the given indices and updates dictionary accordingly
    private void swap(int a, int b) {
        if (a >= size || a < 0 || b >= size || b < 0) {
            throw new IndexOutOfBoundsException();
        }
        // swaps 2 elements within the heap, and then changes their indices in the dictionary
        T elementA = heap[a];
        T elementB = heap[b];
        heap[a] = heap[b];
        heap[b] = elementA;
        elementDictionary.put(elementA, b);
        elementDictionary.put(elementB, a);
    }

    // Pre: size > 0, otherwise throws EmptyContainerException
    // Post: removes the smallest item from the heap and returns it
    @Override
    public T removeMin() {
        if (size <= 0) {
            throw new EmptyContainerException();
        }
        // removes the first item in the heap, replaces it with the last item
        // then percolates down
        T temp = heap[0];
        heap[0] = heap[size - 1];
        size--;
        percolate(0);
        elementDictionary.remove(temp);
        return temp;
    }

    // Pre: size > 0, otherwise throws EmptyContainerException
    // Post: returns the value of the smallest item in the heap
    @Override
    public T peekMin() {
        if (size <= 0) {
            throw new EmptyContainerException();
        }
        return heap[0];
    }

    // Pre: item != null, otherwise throws IllegalArgumentException
    //      item is in the heap, otherwise throws InvalidElementException
    // Post: adds the item into the heap
    @Override
    public void add(T item) {
        if (item == null) {
            throw new IllegalArgumentException();
        } else if (contains(item)) {
            throw new InvalidElementException();
        }
        if (this.size == heap.length) {
            T[] temp = makeArrayOfT(size * 2);
            int i = 0;
            while (i < this.size) {
                temp[i] = heap[i];
                i++;
            }
            heap = temp;
        }
        heap[size] = item;
        size++;
        elementDictionary.put(item, size - 1);
        percolate(size - 1);
    }

    // Pre: item != null, otherwise throws IllegalArgumentException
    // Post: returns true if the item is in the heap, false otherwise
    @Override
    public boolean contains(T item) {
        if (item == null) {
            throw new IllegalArgumentException();
        }
        return elementDictionary.containsKey(item);
    }

    // Pre: the item already exists in the heap, otherwise throws InvalidElementException
    //      the item we are trying to remove is not null, otherwise throws IllegalArgumentException
    // Post: removes the item from the heap
    @Override
    public void remove(T item) {
        if (item == null) {
            throw new IllegalArgumentException();
        }
        if (!contains(item)) {
            throw new InvalidElementException();
        }
        int index = elementDictionary.remove(item); //remove 1--> 3
        heap[index] = heap[size - 1];
        size--;
        // Originally if we remove the last item in the array, and size is still greater than 0,
        // this would put that item back to the array. we don't want that,therefore add
        // another condition.
        if (size > 0 && size != index) {
            elementDictionary.put(heap[index], index);
            percolate(index);
        }
        //if (size != index) {
        //percolate(index);
        //}
    }

    // Pre: the heap contains the oldItem, and doesnt contain the newItem
    //      otherwise, throws InvalidArgumentException
    //      oldItem and newItem != null, otherwise throws IllegalArgumentException
    // Post: replaces oldItem with newItem
    @Override
    public void replace(T oldItem, T newItem) {
        if (oldItem == null || newItem == null) {
            throw new IllegalArgumentException();
        }
        if (contains(newItem) || !contains(oldItem)) {
            throw new InvalidElementException();
        }
        int index = elementDictionary.remove(oldItem);
        heap[index] = newItem;
        elementDictionary.put(newItem, index);
        percolate(index);
    }

    // returns number of elements in the heap
    @Override
    public int size() {
        return size;
    }
}
