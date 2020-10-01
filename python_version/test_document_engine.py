"""
Sylvia Wang

This program is to test the correctness of the functions for the
search engine implemented in Document and SearchEngine classes.
"""


from cse163_utils import assert_equals

from document import Document
from search_engine import SearchEngine
import math


def main():
    """
    A main function that calls the functions that test
    the correctness of the functions implemented in
    Document and SearchEngine classes.
    """
    test_document_class()
    test_search_engine_class()


def test_document_class():
    """
    This function tests the correctness of the functions
    implemented in Document class.
    """
    doc = Document('/home/dog.txt')
    assert_equals('/home/dog.txt', doc.get_path())
    assert_equals(['the', 'cutest', 'dog'], doc.get_words())
    assert_equals(0.25, doc.term_frequency('the'))
    assert_equals(0.25, doc.term_frequency('dog'))
    assert_equals(0.5, doc.term_frequency('cutest'))


def test_search_engine_class():
    """
    This function tests the correctness of the functions
    implemented in SearchEngine class.
    """
    search_engine = SearchEngine('test-files')
    assert_equals(math.log(3/2), search_engine._calculate_idf('dogs'))
    fir_search = search_engine.search('dogs')
    assert_equals(['test-files/doc3.txt', 'test-files/doc1.txt'], fir_search)
    sec_search = search_engine.search('Cats very cute')
    assert_equals(['test-files/doc2.txt'], sec_search)
    thir_search = search_engine.search('happy')
    assert_equals(None, thir_search)


if __name__ == '__main__':
    main()
