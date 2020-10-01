package search.analyzers;

import datastructures.concrete.ChainedHashSet;
import datastructures.concrete.dictionaries.ChainedHashDictionary;
import datastructures.concrete.dictionaries.KVPair;
import datastructures.interfaces.IDictionary;
import datastructures.interfaces.IList;
import datastructures.interfaces.ISet;
import search.models.Webpage;

import java.net.URI;

/**
 * This class is responsible for computing how "relevant" any given document is
 * to a given search query.
 *
 * See the spec for more details.
 */
public class TfIdfAnalyzer {
    // This field must contain the IDF score for every single word in all
    // the documents.
    private IDictionary<String, Double> idfScores;

    // This field must contain the TF-IDF vector for each webpage you were given
    // in the constructor.
    //
    // We will use each webpage's page URI as a unique key.
    private IDictionary<URI, IDictionary<String, Double>> documentTfIdfVectors;

    // Feel free to add extra fields and helper methods.

    private IDictionary<URI, Double> normDictionary;
    /**
     * @param webpages  A set of all webpages we have parsed. Must be non-null and
     *                  must not contain nulls.
     */
    public TfIdfAnalyzer(ISet<Webpage> webpages) {
        // Implementation note: We have commented these method calls out so your
        // search engine doesn't immediately crash when you try running it for the
        // first time.
        //

        this.idfScores = this.computeIdfScores(webpages);
        this.documentTfIdfVectors = this.computeAllDocumentTfIdfVectors(webpages);

        normDictionary = new ChainedHashDictionary<>();
        for (KVPair<URI, IDictionary<String, Double>> pair : documentTfIdfVectors) {
            normDictionary.put(pair.getKey(), norm(pair.getValue()));
        }
    }

    // Note: this method, strictly speaking, doesn't need to exist. However,
    // we've included it so we can add some unit tests to help verify that your
    // constructor correctly initializes your fields.
    public IDictionary<URI, IDictionary<String, Double>> getDocumentTfIdfVectors() {
        return this.documentTfIdfVectors;
    }

    // Note: these private methods are suggestions or hints on how to structure your
    // code. However, since they're private, you're not obligated to implement exactly
    // these methods: feel free to change or modify these methods however you want. The
    // important thing is that your 'computeRelevance' method ultimately returns the
    // correct answer in an efficient manner.

    /**
     * Return a dictionary mapping every single unique word found
     * in every single document to their IDF score.
     */
    private IDictionary<String, Double> computeIdfScores(ISet<Webpage> pages) {
        IDictionary<String, Double> temp = new ChainedHashDictionary<>();
        IDictionary<String, Double> result = new ChainedHashDictionary<>();
        for (Webpage page : pages) {
            ISet<String> words = computeUniqueWords(page.getWords());
            for (String word : words) {
                temp.put(word, temp.getOrDefault(word, 0.0) + 1);
            }
        }

        int numPages = pages.size();
        for (KVPair<String, Double> pair : temp) {
            double value = Math.log(numPages / pair.getValue());
            result.put(pair.getKey(), value);
        }
        return result;
    }

    /**
     * Returns a dictionary mapping every unique word found in the given list
     * to their term frequency (TF) score.
     *
     * The input list represents the words contained within a single document.
     */
    private IDictionary<String, Double> computeTfScores(IList<String> words) {
        // IDictionary<String, Double> temp = new ChainedHashDictionary<>();
        IDictionary<String, Double> result = new ChainedHashDictionary<>();
        int size = words.size();
        for (String word : words) {
            double value = result.getOrDefault(word, 0.0);
            if (value != 0.0) {
                value *= size;
            }
            value++;
            result.put(word, value / size);
        }

        return result;
    }

    /**
     * See spec for more details on what this method should do.
     */
    private IDictionary<URI, IDictionary<String, Double>> computeAllDocumentTfIdfVectors(ISet<Webpage> pages) {
        // Hint: this method should use the idfScores field and
        // call the computeTfScores(...) method.

        IDictionary<URI, IDictionary<String, Double>> result = new ChainedHashDictionary<>();

        for (Webpage page : pages) {
            IDictionary<String, Double> tfIdfScores = new ChainedHashDictionary<>();
            IDictionary<String, Double> tfScores = computeTfScores(page.getWords());

            for (KVPair<String, Double> pair : tfScores) {
                double value = pair.getValue() * idfScores.get(pair.getKey());
                tfIdfScores.put(pair.getKey(), value);
            }
            result.put(page.getUri(), tfIdfScores);
        }
        return result;
    }

    /**
     * Returns the cosine similarity between the TF-IDF vector for the given query and the
     * URI's document.
     *
     * Precondition: the given uri must have been one of the uris within the list of
     *               webpages given to the constructor.
     */
    public Double computeRelevance(IList<String> query, URI pageUri) {
        // Note: The pseudocode we gave you is not very efficient. When implementing,
        // this method, you should:
        //
        // 1. Figure out what information can be precomputed in your constructor.
        //    Add a third field containing that information.
        //
        // 2. See if you can combine or merge one or more loops.

        IDictionary<String, Double> documentVector = documentTfIdfVectors.get(pageUri);
        IDictionary<String, Double> queryVector = computeQueryVector(query);

        double numerator = 0.0;
        for (String word : query) {
            double docWordScore = documentVector.containsKey(word) ? documentVector.get(word) : 0.0;
            double queryWordScore = queryVector.get(word);
            numerator += docWordScore * queryWordScore;
        }

        double denominator = normDictionary.get(pageUri) * norm(queryVector);

        return denominator != 0 ? numerator / denominator : 0.0;
    }

    private double norm(IDictionary<String, Double> vector) {
        double output = 0.0;
        for (KVPair<String, Double> pair : vector) {
            double score = pair.getValue();
            output += score * score;
        }
        return Math.sqrt(output);
    }

    private ISet<String> computeUniqueWords(IList<String> words) {
        ISet<String> set = new ChainedHashSet<>();
        for (String word : words) {
            set.add(word);
        }
        return set;
    }

    private IDictionary<String, Double> computeQueryVector(IList<String> query) {
        IDictionary<String, Double> result = new ChainedHashDictionary<>();
        ISet<String> wordSet = computeUniqueWords(query);
        IDictionary<String, Double> queryTfScores = computeTfScores(query);

        for (String word : wordSet) {
            if (idfScores.containsKey(word)) {
                double value = queryTfScores.get(word) * idfScores.get(word);
                result.put(word, value);
            }
        }
        return result;
    }
}
