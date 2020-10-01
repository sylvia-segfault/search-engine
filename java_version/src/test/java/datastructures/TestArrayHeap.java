package datastructures;

import datastructures.concrete.ArrayHeap;
import datastructures.concrete.DoubleLinkedList;
import datastructures.interfaces.IList;
import datastructures.interfaces.IPriorityQueue;
import misc.BaseTest;
import misc.exceptions.EmptyContainerException;
import misc.exceptions.InvalidElementException;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runners.MethodSorters;
// import sun.plugin2.os.windows.SECURITY_ATTRIBUTES;

// import java.util.ArrayList;
// import java.util.Random;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;


/**
 * See spec for details on what kinds of tests this class should include.
 */
@FixMethodOrder(MethodSorters.NAME_ASCENDING)
public class TestArrayHeap extends BaseTest {
    protected <T extends Comparable<T>> IPriorityQueue<T> makeInstance() {
        return new ArrayHeap<>();
    }

    @Test(timeout=SECOND)
    public void testBasicSize() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        heap.add(3);
        assertEquals(1, heap.size());
        assertFalse(heap.isEmpty());
    }

    @Test(timeout=SECOND)
    public void testBasicAddReflection() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        heap.add(3);
        Comparable<Integer>[] array = getArray(heap);
        assertEquals(3, array[0]);
    }

    @Test(timeout=SECOND)
    public void testAddDuplicates() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        heap.add(50);
        try {
            heap.add(50);
            fail("Expected InvalidElementException();");
        } catch (InvalidElementException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testAddNegativeAndSort() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        heap.add(-1);
        heap.add(-2);
        heap.add(-3);
        assertEquals(-3, heap.removeMin());
        assertEquals(-2, heap.removeMin());
        assertEquals(-1, heap.removeMin());
    }

    @Test(timeout = SECOND)
    public void testUpdateDecrease() {
        IntWrapper[] values = IntWrapper.createArray(new int[]{1, 2, 3, 4, 5});
        IPriorityQueue<IntWrapper> heap = this.makeInstance();

        for (IntWrapper value : values) {
            heap.add(value);
        }

        IntWrapper newValue = new IntWrapper(0);
        heap.replace(values[2], newValue);

        assertEquals(newValue, heap.removeMin());
        assertEquals(values[0], heap.removeMin());
        assertEquals(values[1], heap.removeMin());
        assertEquals(values[3], heap.removeMin());
        assertEquals(values[4], heap.removeMin());
    }

    @Test(timeout=SECOND)
    public void testUpdateIncrease() {
        IntWrapper[] values = IntWrapper.createArray(new int[]{0, 2, 4, 6, 8});
        IPriorityQueue<IntWrapper> heap = this.makeInstance();

        for (IntWrapper value : values) {
            heap.add(value);
        }

        IntWrapper newValue = new IntWrapper(5);
        heap.replace(values[0], newValue);

        assertEquals(values[1], heap.removeMin());
        assertEquals(values[2], heap.removeMin());
        assertEquals(newValue, heap.removeMin());
        assertEquals(values[3], heap.removeMin());
        assertEquals(values[4], heap.removeMin());
    }

    @Test(timeout = SECOND)
    public void basicTestConstructor() {
        IPriorityQueue<IntWrapper> heap = this.makeInstance();
        assertEquals(0, heap.size());
    }

    @Test(timeout = SECOND)
    public void testContainsItemBasic() {
        IPriorityQueue<IntWrapper> heap = this.makeInstance();
        IntWrapper[] values = IntWrapper.createArray(new int[]{1, 2, 3, 4, 5});
        for (IntWrapper value : values) {
            heap.add(value);
            assertTrue(heap.contains(value));
        }
    }

    @Test(timeout = SECOND)
    public void basicTestAddUpdatesSize() {
        IPriorityQueue<IntWrapper> heap = this.makeInstance();
        IntWrapper[] values = IntWrapper.createArray(new int[]{1, 2, 3, 4, 5});
        for (IntWrapper value : values) {
            heap.add(value);
        }
        assertEquals(5, heap.size());
    }

    @Test(timeout = SECOND)
    public void testRemoveMin() {
        IPriorityQueue<IntWrapper> heap = this.makeInstance();
        IntWrapper[] values = IntWrapper.createArray(new int[]{1, 2, 3, 4, 5});
        for (IntWrapper value : values) {
            heap.add(value);
            assertEquals(value, heap.removeMin());
        }
    }

    @Test(timeout = SECOND)
    public void testAddAndRemoveBasic() {
        IPriorityQueue<IntWrapper> heap = this.makeInstance();
        IntWrapper[] values = IntWrapper.createArray(new int[]{1, 2, 3, 4, 5});
        for (IntWrapper value : values) {
            heap.add(value);
            assertEquals(true, heap.contains(value));
            heap.remove(value);
            assertEquals(false, heap.contains(value));
        }
    }

    @Test(timeout = SECOND)
    public void testRemoveEmpty() {
        IPriorityQueue<IntWrapper> heap = this.makeInstance();
        IntWrapper[] values = IntWrapper.createArray(new int[]{0});
        try {
            for (IntWrapper item : values) {
                heap.remove(item);
            }
            fail("Expected InvalidElementException");
        } catch (InvalidElementException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testRemoveBasic() {
        IPriorityQueue<IntWrapper> heap = this.makeInstance();
        IntWrapper[] values = IntWrapper.createArray(new int[]{1, 2, 3, 4, 5});
        for (IntWrapper value : values) {
            heap.add(value);
        }
        for (IntWrapper value : values) {
            heap.remove(value);
        }
        assertEquals(0, heap.size());
    }

    // @SuppressWarnings("unchecked")
    // @Test(timeout = SECOND)
    // public void testRemoveEdgeCaseUpdateDict() {
    //     IPriorityQueue<Integer> heap = this.makeInstance();
    //     for (int i = 0; i < 100; i++) {
    //         heap.add(i);
    //     }
    //     for (int i = 0; i < 98; i++) {
    //         heap.remove(i);
    //     }
    //     assertEquals(2, heap.size());
    //     IDictionary<Integer, Integer> dictionary = (ChainedHashDictionary)
    //             getField(heap, "elementDictionary");
    //     assertEquals(0, dictionary.get(98));
    //     assertEquals(1, dictionary.get(99));
    // }

    @Test(timeout = SECOND)
    public void removeMinUpdatesSize() {
        IntWrapper[] values = IntWrapper.createArray(new int[]{0, 2, 4, 6, 8});
        IPriorityQueue<IntWrapper> heap = this.makeInstance();

        for (IntWrapper value : values) {
            heap.add(value);
        }

        int size = heap.size();
        int tempSize = heap.size();
        for (int i = 0; i < size; i++) {
            heap.removeMin();
            tempSize--;
            assertEquals(tempSize, heap.size());
        }
    }

    @Test(timeout = SECOND)
    public void peekMinReturnsCorrectElement() {
        IntWrapper[] values = IntWrapper.createArray(new int[]{0, 2, 4, 6, 8});
        IPriorityQueue<IntWrapper> heap = this.makeInstance();

        for (IntWrapper value : values) {
            heap.add(value);
        }

        for (int i = 0; i < heap.size(); i++) {
            assertEquals(heap.peekMin(), values[i]);
            heap.removeMin();
        }
    }

    @Test(timeout = SECOND)
    public void testAddResize() {
        IPriorityQueue<Integer> heap = this.makeInstance();

        for (int i = 0; i < 100; i++) {
            heap.add(i);
        }

        for (int i = 0; i < 100; i++) {
            assertEquals(heap.removeMin(), i);
        }
    }

    @Test(timeout = SECOND)
    public void testSizeOnNull() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        assertEquals(0, heap.size());
    }

    @Test(timeout = SECOND)
    public void testPercolatesProperly() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        IList<Integer> list = new DoubleLinkedList<>();
        boolean alternate = false;
        for (int i = 0; i < 100; i++) {
            if (alternate) {
                heap.add(i);
            } else {
                heap.add(100 - i);
            }
            alternate = !alternate;
        }
        for (int i = 0; i < 100; i++) {
            list.add(heap.removeMin());
        }
        for (int i = 0; i < 99; i++) {
            assertTrue(list.get(i) < list.get(i + 1));
        }
    }

    @Test(timeout = SECOND)
    public void testAddMinElementEveryTime() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        for (int i = 100; i >= 0; i--) {
            heap.add(i);
        }

        for (int i = 0; i <= 100; i++) {
            assertEquals(heap.removeMin(), i);
        }
    }

    @Test(timeout = SECOND)
    public void testReplaceEdgeCases() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        heap.add(5);
        heap.add(4);
        heap.add(8);
        heap.replace(4, 1);
        heap.replace(8, 9);
        assertEquals(1, heap.removeMin());
        heap.removeMin();
        assertEquals(9, heap.removeMin());
    }

    @Test(timeout = SECOND)
    public void testReplaceMany() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        for (int i = 0; i < 100; i++) {
            heap.add(i);
        }
        for (int i = 0; i < 100; i++) {
            heap.replace(i, i + 100000);
        }

        for (int i = 0; i < 100; i++) {
            assertEquals(heap.removeMin(), i + 100000);
        }
    }

    @Test(timeout = SECOND)
    public void testStress() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        IList<Integer> list = new DoubleLinkedList<>();
        boolean alternate = false;
        for (int i = 0; i < 1000; i++) {
            if (alternate) {
                heap.add(i);
            } else {
                heap.add(1000 - i);
            }
            alternate = !alternate;
        }
        for (int i = 0; i < 1000; i++) {
            list.add(heap.removeMin());
        }
        for (int i = 0; i < 999; i++) {
            assertTrue(list.get(i) < list.get(i + 1));
        }
    }


    @Test(timeout = SECOND)
    public void testRemoveMinThrowsException() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        try {
            heap.removeMin();
            fail("Expected EmptyContainerException");
        } catch (EmptyContainerException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testRemoveMinMany() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        for (int i = 0; i < 100; i++) {
            heap.add(i);
            heap.removeMin();
        }
    }

    @Test(timeout=SECOND)
    public void testPutAndGetSameItemRepeated() {
        IPriorityQueue<Integer> heap = this.makeInstance();

        // First insertion
        heap.add(3);
        assertEquals(3, heap.removeMin());

        // Second insertion
        heap.add(3);
        assertEquals(3, heap.removeMin());

        // Third insertion
        heap.add(3);
        assertEquals(3, heap.removeMin());
    }


    @Test(timeout = SECOND)
    public void testPeekMinThrowsException() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        try {
            heap.peekMin();
            fail("Expected EmptyContainerException");
        } catch (EmptyContainerException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testAddThrowsException() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        try {
            heap.add(null);
            fail("Expected IllegalArgumentException");
        } catch (IllegalArgumentException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }

        try {
            heap.add(1);
            heap.add(1);
            fail("Expected InvalidElementException");
        } catch (InvalidElementException ex) {
            // Do Nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testContainsThrowsException() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        try {
            heap.contains(null);
            fail("Expected IllegalArgumentException");
        } catch (IllegalArgumentException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testRemoveThrowsException() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        try {
            heap.add(1);
            heap.remove(null);
            fail("Expected IllegalArgumentException");
        } catch (IllegalArgumentException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }

        heap.remove(1);
        try {
            heap.remove(1);
            fail("Expected InvalidElementException");
        } catch (InvalidElementException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testReplaceThrowsException() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        try {
            heap.remove(null);
            fail("Expected IllegalArgumentException");
        } catch (IllegalArgumentException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }

        try {
            heap.remove(1);
            fail("Expected InvalidElementException");
        } catch (InvalidElementException ex) {
            // DO nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testContainsAfterRemove() {
        IPriorityQueue<Integer> heap = this.makeInstance();
        heap.add(50);
        heap.add(35);
        heap.add(15);
        heap.add(10);
        heap.add(60);
        heap.add(23);
        heap.remove(50);
        assertEquals(false, heap.contains(50));
    }
    /**
     * A comparable wrapper class for ints. Uses reference equality so that two different IntWrappers
     * with the same value are not necessarily equal--this means that you may have multiple different
     * IntWrappers with the same value in a heap.
     */
    public static class IntWrapper implements Comparable<IntWrapper> {
        private final int val;

        public IntWrapper(int value) {
            this.val = value;
        }

        public static IntWrapper[] createArray(int[] values) {
            IntWrapper[] output = new IntWrapper[values.length];
            for (int i = 0; i < values.length; i++) {
                output[i] = new IntWrapper(values[i]);
            }
            return output;
        }


        @Override
        public int compareTo(IntWrapper o) {
            return Integer.compare(val, o.val);
        }

        @Override
        public boolean equals(Object obj) {
            return this == obj;
        }

        @Override
        public int hashCode() {
            return this.val;
        }

        @Override
        public String toString() {
            return Integer.toString(this.val);
        }
    }

    /**
     * A helper method for accessing the private array inside a heap using reflection.
     */
    @SuppressWarnings("unchecked")
    private static <T extends Comparable<T>> Comparable<T>[] getArray(IPriorityQueue<T> heap) {
        return getField(heap, "heap", Comparable[].class);
    }

}
