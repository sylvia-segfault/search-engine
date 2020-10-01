package misc;

import datastructures.concrete.ArrayHeap;
import datastructures.concrete.DoubleLinkedList;
import datastructures.interfaces.IList;
import datastructures.interfaces.IPriorityQueue;

public class Sorter {
    /**
     * This method takes the input list and returns the top k elements
     * in sorted order.
     *
     * So, the first element in the output list should be the "smallest"
     * element; the last element should be the "largest".
     *
     * If the input list contains fewer than 'k' elements, return
     * a list containing all input.length elements in sorted order.
     *
     * This method must not modify the input list.
     *
     * @throws IllegalArgumentException  if k < 0
     * @throws IllegalArgumentException  if input is null
     */
    public static <T extends Comparable<T>> IList<T> topKSort(int k, IList<T> input) {
        // Implementation notes:
        //
        // - This static method is a _generic method_. A generic method is similar to
        //   the generic methods we covered in class, except that the generic parameter
        //   is used only within this method.
        //
        //   You can implement a generic method in basically the same way you implement
        //   generic classes: just use the 'T' generic type as if it were a regular type.
        //
        // - You should implement this method by using your ArrayHeap for the sake of
        //   efficiency.


        // Implementing it in O(N log(K)) means that we go through N elements in the list
        // and we go through K elements in the heap when we rotate
        //
        // When we do a normal heapSort algorithm, we first do Floyd heap method, which runs in O(N)
        // then when we remove each item from the heap, we do N removals, and each removal takes O(log(N))
        // so together it becomes O(N + N log(N)) = O(N log(N))
        //
        // As a result, if we want to make our sort run in O(N log(K)), it means that every time we percolate,
        // we have to percolate through K items, which means we have to have K or less items at all times
        //
        // Therefore, we construct a heap with K elements at first, then we compare subsequent items from the list
        // to the minimum item in the heap, if the subsequent item in the list > minimum item in the heap
        // then we replace the minimum item in the heap, otherwise we don't do anything
        //
        // This makes it so that we have at most K items at all times, and we will always have the largest K elements
        // at the end, because we are removing the smallest item and replacing it with a larger item
        if (k < 0 || input == null) {
            throw new IllegalArgumentException();
        }
        IPriorityQueue<T> heap = new ArrayHeap<>();
        IList<T> result = new DoubleLinkedList<>();
        // Iterator<T> iter = input.iterator();
        if (k == 0) {
            return result;
        } else if (input.size() < k) {
            // if input size is less than k
            for (T item : input) {
                heap.add(item);
            }
            for (int i = 0; i < input.size(); i++) {
                result.add(heap.removeMin());
            }
        } else {
            for (T item : input) {
                if (k > 0) {
                    // add to the heap until it has K item
                    heap.add(item);
                    k--;
                } else {
                    // after the heap has K items within, we compare subsequent list items to the minimum
                    if (item.compareTo(heap.peekMin()) > 0) {
                        // then we replace the minimum with the subsequent item, as we want the largest K items
                        heap.replace(heap.peekMin(), item);
                    }
                }
            }
        }
        // while (iter.hasNext()) {
        //     while (iter.hasNext() && heap.size() < k) {
        //         // add to the heap until it has K item
        //         heap.add(iter.next());
        //     }
        //     // after the heap has K items within, we compare subsequent list items to the minimum
        //     T item = iter.next();
        //     if (item.compareTo(heap.peekMin()) > 0) {
        //         // then we replace the minimum with the subsequent item, as we want the largest K items
        //         heap.replace(heap.peekMin(), item);
        //     }
        // }
        // At the end, we end up with a heap of K items, all of which are larger than any other item in the list
        // So when we removeMin from th heap, we get an ordered list of the largest K items
        while (heap.size() != 0) {
            result.add(heap.removeMin());
        }
        return result;
    }
}
