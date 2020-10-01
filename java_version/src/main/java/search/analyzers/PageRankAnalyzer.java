package search.analyzers;

import datastructures.concrete.ChainedHashSet;
import datastructures.concrete.dictionaries.ChainedHashDictionary;
import datastructures.concrete.dictionaries.KVPair;
import datastructures.interfaces.IDictionary;
import datastructures.interfaces.ISet;
import search.models.Webpage;

import java.net.URI;

/**
 * This class is responsible for computing the 'page rank' of all available webpages.
 * If a webpage has many different links to it, it should have a higher page rank.
 * See the spec for more details.
 */
public class PageRankAnalyzer {
    private IDictionary<URI, Double> pageRanks;

    /**
     * Computes a graph representing the internet and computes the page rank of all
     * available webpages.
     *
     * @param webpages  A set of all webpages we have parsed. Must be non-null and must not contain
     *                  nulls.
     * @param decay     Represents the "decay" factor when computing page rank (see spec). Must be a
     *                  number between 0 and 1, inclusive.
     * @param epsilon   When the difference in page ranks is less than or equal to this number,
     *                  stop iterating. Must be a non-negative number.
     * @param limit     The maximum number of iterations we spend computing page rank. This value
     *                  is meant as a safety valve to prevent us from infinite looping in case our
     *                  page rank never converges. Must be a non-negative number. (A limit of 0 should
     *                  simply return the initial page rank values from 'computePageRank'.)
     */
    public PageRankAnalyzer(ISet<Webpage> webpages, double decay, double epsilon, int limit) {
        // Implementation note: We have commented these method calls out so your
        // search engine doesn't immediately crash when you try running it for the
        // first time.

        // Step 1: Make a graph representing the 'internet'
        IDictionary<URI, ISet<URI>> graph = this.makeGraph(webpages);

        // Step 2: Use this graph to compute the page rank for each webpage
        this.pageRanks = this.makePageRanks(graph, decay, limit, epsilon);

        // Note: we don't store the graph as a field: once we've computed the
        // page ranks, we no longer need it!
    }

    /**
     * This method converts a set of webpages into an unweighted, directed graph,
     * in adjacency list form.
     *
     * You may assume that each webpage can be uniquely identified by its URI.
     *
     * Note that a webpage may contain links to other webpages that are *not*
     * included within set of webpages you were given. You should omit these
     * links from your graph: we want the final graph we build to be
     * entirely "self-contained".
     */
    private IDictionary<URI, ISet<URI>> makeGraph(ISet<Webpage> webpages) {
        IDictionary<URI, ISet<URI>> result = new ChainedHashDictionary<>();
        ISet<URI> uriSet = new ChainedHashSet<>();
        // First, makes a set of unique URIs
        for (Webpage webpage :webpages) {
            uriSet.add(webpage.getUri());
        }

        for (Webpage webpage :webpages) {
            // for every webpage in webpages
            ISet<URI> set = new ChainedHashSet<>();
            // make a new set
            for (URI link : webpage.getLinks()) {
                // go through its links, and if a link is in the uriSet and is not itself, add it
                if (uriSet.contains(link) && !set.contains(link) && link != webpage.getUri()) {
                    set.add(link);
                }
            }
            // After we construct a set of uris that our original uri is pointing to, we add them to result
            result.put(webpage.getUri(), set);
        }
        return result;
    }

    /**
     * Computes the page ranks for all webpages in the graph.
     *
     * Precondition: assumes 'this.graphs' has previously been initialized.
     *
     * @param decay     Represents the "decay" factor when computing page rank (see spec).
     * @param epsilon   When the difference in page ranks is less than or equal to this number,
     *                  stop iterating.
     * @param limit     The maximum number of iterations we spend computing page rank. This value
     *                  is meant as a safety valve to prevent us from infinite looping in case our
     *                  page rank never converges.
     */
    private IDictionary<URI, Double> makePageRanks(IDictionary<URI, ISet<URI>> graph,
                                                   double decay,
                                                   int limit,
                                                   double epsilon) {
        // Step 1: The initialize step should go here
        IDictionary<URI, Double> oldRanks = new ChainedHashDictionary<>();
        IDictionary<URI, Double> newRanks;
        int numPages = graph.size();
        // First, set all the pageRanks to 1 / n
        double initialRank = 1.0 / numPages;
        for (KVPair<URI, ISet<URI>> pair : graph) {
            oldRanks.put(pair.getKey(), initialRank);
        }

        for (int i = 0; i < limit; i++) {
            // Step 2: The update step should go here
            // set all the values in the result to 0
            newRanks = new ChainedHashDictionary<>();

            // loop through every page in pageRanks
            for (KVPair<URI, Double> pair : oldRanks) {
                URI page = pair.getKey();
                ISet<URI> links = graph.get(page);
                double distributedRank;
                if (links.size() == 0) {
                    // if the page doesn't have links in it, distribute its rank among all the other uris
                    distributedRank = oldRanks.get(page) / numPages * decay;
                    incrementAll(newRanks, distributedRank, graph);
                } else {
                    // if it has links in it, distribute its rank among all its children
                    distributedRank = oldRanks.get(page) / links.size() * decay;
                    for (URI link : links) {
                        newRanks.put(link, newRanks.getOrDefault(link, 0.0) + distributedRank);
                    }
                }
            }
            // After we distribute each uri page rank among its links, add the decayed users back to the result
            double distributedDecayedUsers = (1.0 - decay) / numPages;
            incrementAll(newRanks, distributedDecayedUsers, graph);
            // Step 3: the convergence step should go here.
            // Return early if we've converged.

            boolean converge = true;
            // for every page in result
            for (KVPair<URI, Double> pair : newRanks) {
                // if any one difference between the old PageRank and the new PageRank, make changed = true;
                if (Math.abs(pair.getValue() - oldRanks.get(pair.getKey())) > epsilon) {
                    converge = false;
                    break;
                }
            }
            if (converge) {
                // if all the pages have a difference of less than epsilon, return the old pageRank
                return oldRanks;
            } else {
                // otherwise, replace the old PageRank with the new PageRank
                oldRanks = newRanks;
            }
        }
        return oldRanks;
    }

    private void incrementAll(IDictionary<URI, Double> pageRank, double n, IDictionary<URI, ISet<URI>> graph) {
        for (KVPair<URI, ISet<URI>> pair : graph) {
            // for every page in pageRank, increment its rank by n
            URI page = pair.getKey();
            pageRank.put(page, pageRank.getOrDefault(page, 0.0) + n);
        }
    }

    /**
     * Returns the page rank of the given URI.
     *
     * Precondition: the given uri must have been one of the uris within the list of
     *               webpages given to the constructor.
     */
    public double computePageRank(URI pageUri) {
        return pageRanks.get(pageUri);
    }
}
