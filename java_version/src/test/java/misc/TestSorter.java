package misc;

import datastructures.concrete.DoubleLinkedList;
import datastructures.interfaces.IList;
import org.junit.FixMethodOrder;
import org.junit.Test;
import org.junit.runners.MethodSorters;

import static org.junit.Assert.fail;


/**
 * See spec for details on what kinds of tests this class should include.
 */
@FixMethodOrder(MethodSorters.NAME_ASCENDING)
public class TestSorter extends BaseTest {
    @Test(timeout=SECOND)
    public void testSimpleUsage() {
        IList<Integer> list = new DoubleLinkedList<>();
        for (int i = 0; i < 20; i++) {
            list.add(i);
        }
        IList<Integer> top = Sorter.topKSort(5, list);
        assertEquals(5, top.size());
        for (int i = 0; i < top.size(); i++) {
            assertEquals(15 + i, top.get(i));
        }
    }

    @Test(timeout = SECOND)
    public void testListElementsLessThanK() {
        IList<Integer> list = new DoubleLinkedList<>();
        for (int i = 0; i < 20; i++) {
            list.add(i);
        }
        IList<Integer> top = Sorter.topKSort(30, list);
        assertEquals(20, top.size());
        for (int i = 0; i < top.size(); i++) {
            assertEquals(i, top.get(i));
        }
    }

    @Test(timeout = SECOND)
    public void testKLessThan0() {
        IList<Integer> list = new DoubleLinkedList<>();
        try {
            Sorter.topKSort(-1, list);
            fail("Expected IllegalArgumentException");
        } catch (IllegalArgumentException ex) {
            // Do nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    // This is not working because objects have references.
    /*@Test(timeout = SECOND)
    public void mutatesInput() {
        IList<Integer> list = new DoubleLinkedList<>();
        for (int i = 0; i < 10; i += 2) {
            list.add(i);
        }
        IList<Integer> comparingList = list;
        Sorter.topKSort(3, list);
        assertEquals(list, comparingList);
    }*/

    @Test(timeout = SECOND)
    public void testKis0() {
        IList<Integer> list = new DoubleLinkedList<>();
        for (int i = 0; i < 20; i++) {
            list.add(i);
        }
        IList<Integer> top = Sorter.topKSort(0, list);
        assertEquals(0, top.size());
    }

    @Test(timeout = SECOND)
    public void testInputIsNull() {
        try {
            Sorter.topKSort(2, null);
            fail("Expected IllegalArgumentException");
        } catch (IllegalArgumentException ex) {
            // Do nothing
        } catch (Exception e) {
            fail("Wrong Exception thrown");
        }
    }

    @Test(timeout = SECOND)
    public void testKis1() {
        IList<Integer> list = new DoubleLinkedList<>();
        for (int i = 0; i < 20; i++) {
            list.add(i);
        }
        IList<Integer> top = Sorter.topKSort(1, list);
        assertEquals(19, top.get(0));
    }

    @Test(timeout = SECOND)
    public void testInputSizeIs1() {
        IList<Integer> list = new DoubleLinkedList<>();
        list.add(1);
        IList<Integer> top = Sorter.topKSort(1, list);
        assertEquals(1, top.get(0));
    }

    @Test(timeout = SECOND)
    public void basicTestTopKSorted() {
        IList<Integer> list = new DoubleLinkedList<>();
        list.add(50);
        list.add(25);
        list.add(5);
        list.add(70);
        list.add(55);
        IList<Integer> top = Sorter.topKSort(5, list);
        assertEquals(5, top.get(0));
        assertEquals(25, top.get(1));
        assertEquals(50, top.get(2));
        assertEquals(55, top.get(3));
        assertEquals(70, top.get(4));
    }

    @Test(timeout = SECOND)
    public void testComboOfEdgeCases() {
        IList<Integer> list = new DoubleLinkedList<>();
        IList<Integer> top1 = Sorter.topKSort(0, list);
        assertEquals(0, top1.size());
        IList<Integer> top2 = Sorter.topKSort(5, list);
        assertEquals(0, top2.size());
    }

    @Test(timeout = SECOND)
    public void mutatesInput() {
        IList<Integer> list = new DoubleLinkedList<>();
        list.add(6);
        list.add(8);
        list.add(2);
        list.add(10);
        IList<Integer> top = Sorter.topKSort(2, list);
        assertEquals(6, list.get(0));
        assertEquals(8, list.get(1));
        assertEquals(2, list.get(2));
        assertEquals(10, list.get(3));
    }
}