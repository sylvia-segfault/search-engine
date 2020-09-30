"""
Sylvia Wang

This file represents a Document class that is for a single document
in the SearchEngine, and it has functionalities like getting a
list of words in this document, getting the full path of the document,
and computing the term frequency of a term in the document.
"""


import re


class Document:
    """
    Document class represents a single document in the SearchEngine,
    and it has functionalities like getting a list of words in this
    document, getting the full path of the document, and computing
    the term frequency of a term in the document.
    """

    def __init__(self, file_name):
        """
        An initializer for Document class which stores the given
        file name and builds up a dictionary that maps from each
        word appears in the document to its term frequency. Term
        frequency is defined as (# of occurences of term t
        in the document) / (# of words in the document)
        Arguments:
        - self: refer to this instance of Document class
        - file_name: the full path for this instance of the
                     Document class
        """
        self._path = file_name
        self._word_freq = {}
        count_word = 0
        with open(file_name) as f:
            for line in f.readlines():
                for word in line.split():
                    word = re.sub(r'\W+', '', word)
                    word = word.lower()
                    count_word += 1
                    if word in self._word_freq:
                        self._word_freq[word] += 1
                    else:
                        self._word_freq[word] = 1
        for k, v in self._word_freq.items():
            self._word_freq[k] = v / count_word

    def get_path(self):
        """
        Return the full path for this instance of Document
        Arguments:
        - self: refer to this instance of Document class
        """
        return self._path

    def term_frequency(self, term):
        """
        Return the term frequency for the given term, which is
        calculated through (# of occurences of the given term
        in the document) / (# of words in the document)
        Return 0 if the given term does not exist in this
        document.
        The given term is case-insensitive, and any punctation
        in the given term should be removed before computing
        its term frequency.
        Arguments:
        - self: refer to this instance of Document class
        - term: the given term whose term frequency is to be
                calculated
        """
        term = re.sub(r'\W+', '', term)
        term = term.lower()
        if term in self._word_freq:
            return self._word_freq[term]
        else:
            return 0

    def get_words(self):
        """
        Return a list of words in this document.
        Each word in the returned list is case-insensitive,
        and any punctation in each word should be removed
        from it.
        Arguments:
        - self: refer to this instance of Document class
        """
        return self._word_freq.keys()
