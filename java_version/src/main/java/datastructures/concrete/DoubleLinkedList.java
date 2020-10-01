package datastructures.concrete;

import datastructures.interfaces.IList;
import misc.exceptions.EmptyContainerException;

import java.util.Iterator;
import java.util.NoSuchElementException;
import java.util.Objects;

/**
 * Note: For more info on the expected behavior of your methods:
 * @see datastructures.interfaces.IList
 * (You should be able to control/command+click "IList" above to open the file from IntelliJ.)
 */
public class DoubleLinkedList<T> implements IList<T> {
    // You may not rename these fields or change their types.
    // We will be inspecting these in our private tests.
    // You also may not add any additional fields.
    private Node<T> front;
    private Node<T> back;
    private int size;

    public DoubleLinkedList() {
        this.front = null;
        this.back = null;
        this.size = 0;
    }

    // adds the given item to the end of the list
    @Override
    public void add(T item) {
        if (front == null) {
            // for the case when we want to add to the list and the list is empty
            front = new Node<>(item);
            back = front;
        } else {
            //  for the case when we want to add to the list and the list is not empty
            back.next = new Node<>(back, item, null);
            back = back.next;
        }
        size++;
    }

    // Pre: List has to not be empty, otherwise throws EmptyContainerException
    // Post: Removes a single item from the end of the list
    @Override
    public T remove() {
        if (front == null) {
            throw new EmptyContainerException();
        }
        Node<T> temp = back;
        T item = temp.data;
        if (this.size == 1) {
            // for the case when the list has only 1 element
            front = null;
            back = null;
        } else { //(this.size != 1) {
            // for the case when the list has more than 1 element
            temp.prev.next = null;
            back = back.prev;
        }
        size--;
        return item;
    }

    // Pre: 0 <= index < size, otherwise throws IndexOutOfBoundsException
    // Post: returns the item at the given index
    @Override
    public T get(int index) {
        if (index < 0 || index >= this.size()) {
            throw new IndexOutOfBoundsException();
        }
        // near the front
        if (index <= size / 2) {
            Node<T> curr = front;
            // the loop loops until it gets to the given index
            while (curr.next != null && index > 0) {
                index--;
                curr = curr.next;
            }
            return curr.data;
        } else {
            // near the end
            Node<T> curr = back;
            while (curr.prev != null && size - index - 1 > 0) {
                index++;
                curr = curr.prev;
            }
            return curr.data;
        }
    }

    // Pre: 0 <= index < size, otherwise throws IndexOutOfBoundsException
    // Post: changes the item at the given index to what the given item is
    @Override
    public void set(int index, T item) {
        if (index < 0 || index >= this.size()) {
            throw new IndexOutOfBoundsException();
        }
        if (index == 0) {
            // front case
            Node<T> temp = front;
            front = new Node<>(null, item, temp.next);
            if (front.next != null) {
                // if list.size > 1
                temp.next.prev = front;
            } else {
                // if list.size == 1
                back = front;
            }
        } else if (index == size - 1) {
            // end case
            Node<T> temp = back;
            back = new Node<>(back.prev, item, null);
            temp.prev.next = back;
        } else {
            // all the other cases
            // index is near the front
            if (index <= size / 2) {
                Node<T> curr = front;
                while (curr.next != null && index > 1) {
                    index--;
                    curr = curr.next;
                }
                curr.next = new Node<>(curr, item, curr.next.next);
                curr.next.next.prev = curr.next;
            } else {
                // index near the back
                Node<T> curr = back;
                while (curr.prev != null && size - index - 1 > 1) {
                    index++;
                    curr = curr.prev;
                }
                curr.prev = new Node<>(curr.prev.prev, item, curr);
                curr.prev.prev.next = curr.prev;
            }
        }
    }

    // Pre: 0 <= index < size, otherwise throws IndexOutOfBoundsException
    // Post: inserts the given item at the given index without altering the other items
    @Override
    public void insert(int index, T item) {
        if (index < 0 || index >= this.size() + 1) {
            throw new IndexOutOfBoundsException();
        }
        if (index == 0) {
            // front case
            front = new Node<>(null, item, front);
            if (front.next != null) {
                front.next.prev = front;
            } else {
                back = front;
            }
        } else if (index == this.size) {
            // end case
            back.next = new Node<>(back, item, null);
            back = back.next;
        } else {
            // middle cases
            // adding near the front
            if (index <= this.size / 2) {
                Node<T> curr = front;
                while (curr.next != null && index > 1) {
                    index--;
                    curr = curr.next;
                }
                curr.next = new Node<>(curr, item, curr.next);
                curr.next.next.prev = curr.next;
            } else {
                // adding near the back
                Node<T> curr = back;
                while (curr.prev != null && size - index > 1) {
                    index++;
                    curr = curr.prev;
                }
                curr.prev = new Node<>(curr.prev, item, curr);
                curr.prev.prev.next = curr.prev;
            }
        }
        size++;
    }

    // Pre: 0 <= index < size, otherwise throws IndexOutOfBoundsException
    // Post: deletes the item at the given index, and returns the deleted item
    @Override
    public T delete(int index) {
        if (index < 0 || index >= this.size()) {
            throw new IndexOutOfBoundsException();
        }
        T temp = null;
        if (index == 0) {
            // front case
            temp = front.data;
            front = front.next;
            if (front == null) {
                back = null;
            } else {
                front.prev = null;
            }
        } else if (index == size - 1) {
            // end case
            temp = back.data;
            back = back.prev;
            back.next = null;
        } else {
            // near the front
            if (index <= size / 2) {
                Node<T> curr = front;
                while (curr.next != null && index > 1) {
                    index--;
                    curr = curr.next;
                }
                temp = curr.next.data;
                curr.next = curr.next.next;
                curr.next.prev = curr;
            } else {
                // near the end
                Node<T> curr = back;
                while (curr.prev != null && size - index - 1 > 1) {
                    index++;
                    curr = curr.prev;
                }
                temp = curr.prev.data;
                curr.prev = curr.prev.prev;
                curr.prev.next = curr;
            }
        }
        size--;
        return temp;
    }

    // for this method we need to consider if there is an object that is null
    // which is being sandwiched by 2 non-null items in the list
    // in that case, we need to alter our while loop and check if the item is null
    // This method returns the index of an item, if it is present in the list, otherwise returns -1
    @Override
    public int indexOf(T item) {
        int index = 0;
        Node<T> curr = front;
        // first part of fence post
        while (!curr.equals(back)) {
            // we need to check if data is null in the first if and
            // make them nested if statements, to prevent comparing null to an object
            if (Objects.equals(curr.data, item)) {
                return index;
            }
            curr = curr.next;
            index++;
        }
        // this is the 2nd part of the fence post to make sure we compare the last item
        if (Objects.equals(curr.data, item)) {
            return index;
        }
        return -1;
    }

    // returns the size of the list
    @Override
    public int size() {
        return size;
    }

    // returns true if the list contains the given item, otherwise returns false
    @Override
    public boolean contains(T other) {
        return indexOf(other) != -1;
    }

    @Override
    public Iterator<T> iterator() {
        // Note: we have provided a part of the implementation of
        // an iterator for you. You should complete the methods stubs
        // in the DoubleLinkedListIterator inner class at the bottom
        // of this file. You do not need to change this method.
        return new DoubleLinkedListIterator<>(this.front);
    }

    private static class Node<E> {
        // You may not change the fields in this node or add any new fields.
        public final E data;
        public Node<E> prev;
        public Node<E> next;

        public Node(Node<E> prev, E data, Node<E> next) {
            this.data = data;
            this.prev = prev;
            this.next = next;
        }

        public Node(E data) {
            this(null, data, null);
        }

        // Feel free to add additional constructors or methods to this class.
    }

    private static class DoubleLinkedListIterator<T> implements Iterator<T> {
        // You should not need to change this field, or add any new fields.
        private Node<T> current;

        public DoubleLinkedListIterator(Node<T> current) {
            // You do not need to make any changes to this constructor.
            this.current = current;
        }

        /**
         * Returns 'true' if the iterator still has elements to look at;
         * returns 'false' otherwise.
         */
        public boolean hasNext() {
            return current != null;
        }

        /**
         * Returns the next item in the iteration and internally updates the
         * iterator to advance one element forward.
         *
         * @throws NoSuchElementException if we have reached the end of the iteration and
         *                                there are no more elements to look at.
         */
        public T next() {
            if (current == null) {
                throw new NoSuchElementException();
            }
            Node<T> temp = current;
            current = current.next;
            return temp.data;
        }
    }
}
