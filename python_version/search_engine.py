"""
Sylvia Wang

This file represents a searchEngine class that is for managing
a collection of Documents and it will handle computing the
relevance of the Documents to a given queried term. The SearchEngine
will compute the TF-IDF of a given term for each Document which
contains that term. TF-IDF for a specific document is defined as
the term frequency for the given queried term that appears in this
document multiplied by the IDF score for this term.
"""


import math
import os
import re
from document import Document


class SearchEngine:
    """
    SearchEngine class represents a search engine that is for managing
    a collection of Documentsnd it will handle computing the
    relevance of the Documents to a given queried term. The SearchEngine
    will compute the TF-IDF of a given term for each Document which
    contains that term. TF-IDF for a specific document is defined as
    the term frequency for the given queried term that appears in this
    document multiplied by the IDF score for this term.
    """

    def __init__(self, directory_name):
        """
        An initializer for the SearchEngine class which keeps track of
        how many documents are there in the given directory and builds up
        an inverse index table that maps from each word from all those
        documents to its corresponding list of document objects in which
        that word appears.
        Arguments:
        - self: refer to this instance of SearchEngine class
        - directory_name: the given directory where the client queries
                          words
        """
        self._inverse_index = {}
        self._count_docs = 0
        for file_name in os.listdir(directory_name):
            self._count_docs += 1
            full_path = directory_name + '/' + file_name
            curr_doc = Document(full_path)
            words = curr_doc.get_words()
            for word in words:
                if word not in self._inverse_index:
                    self._inverse_index[word] = []
                self._inverse_index[word].append(curr_doc)

    def _calculate_idf(self, term):
        """
        A helper function for the search algorithm which calculates
        the IDF score for the given term. If the given term does not
        exist in the search engine, it returns 0. Otherwise, return
        the natural log of (# of documents in the given directory) /
        (# of documents in search engine containing the given term)
        Arguments:
        - self: refer to this instance of SearchEngine class
        - term: the given term whose IDF score is to be computed
        """
        term = self._format_term(term)
        if term in self._inverse_index:
            return math.log(self._count_docs / len(self._inverse_index[term]))
        else:
            return 0

    def _calculate_tf_idf(self, term, doc):
        """
        A helper function for the search algorithm which calculates
        the TF-IDF score for the given term in the specific document.
        TF-IDF is defined as the term frequency for the given word
        in this document multiplied by the given term's IDF score.
        Arguments:
        - self: refer to this instance of SearchEngine class
        - term: the given term whose TF-IDF score is to be computed
        - doc: the document in which the given term appears
        """
        idf = self._calculate_idf(term)
        return idf * doc.term_frequency(term)

    def _format_term(self, term):
        """
        A helper function that formats the given term by removing
        all the punctuation from it and converting it to lower
        case.
        Arguments:
        - self: refer to this instance of SearchEngine class
        - term: the given term that is to be formatted
        """
        term = re.sub(r'\W+', '', term)
        return term.lower()

    def search(self, term):
        """
        Return a list of document names that have the given
        queried term in them. The list of documents are
        sorted in descending order of their TF-IDF scores.
        If no document in the SearchEngine contains the given
        term, the function should return None. The search
        function should be case-insensitive, and ignore any
        punctuation in the given term.
        Arguments:
        - self: refer to this instance of SearchEngine class
        - term: the given multi-word query that is to be searched
        """
        # Split the multi-word query into a list of query words
        terms = term.split()
        doc_to_score = {}
        for one_term in terms:
            # format the current term
            one_term = self._format_term(one_term)
            # if the current term is in the search engine,
            # calculate its TF-IDF score for all the documents
            # in which that term appears.
            if one_term in self._inverse_index:
                for doc in self._inverse_index[one_term]:
                    path = doc.get_path()
                    if path not in doc_to_score:
                        doc_to_score[path] = 0
                    doc_to_score[path] += self._calculate_tf_idf(one_term, doc)
        if len(doc_to_score.keys()) == 0:
            return None
        return sorted(doc_to_score, key=doc_to_score.get, reverse=True)
