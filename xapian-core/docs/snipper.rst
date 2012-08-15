Xapian::Snipper Snippet Generation
==================================

This document describes the way the Xapian::Snipper class generates
dynamic query relevant snippets using a relevance model.

Generating the Relevance Model
------------------------------

For generating a relevance model, a ``Xapian::MSet`` yielded by the
query is needed.

Using the ``set_mset(const MSet& mset, int rm_docno)`` method, data
about the terms in the most relevant documents for the query is stored.

* For each document we need to know its weight.

* For each stemmed term we need to know the documents in which it
  appears and its wdf(within document fequency).

For a term to be stemmed, it has to be marked with the `'Z'` prefix.

Snippet Extraction
------------------

For extracting a snippet the ``generate_snippet(std::string text)``
method is used. Before this method is called, a stemmer needs to be
assigned.

Every stemmed term from the text is given a score.

.. math::

    S(t) = P(t|RM) - P(t|\overline{RM})

    \text{where}

    P(t|RM) \text{ is the probability of the term to be relevant to the
     model.}

    P(t|\overline{RM}) \text{ is the probability of the term to be
     irrelevant to the model.}

    P(t|\overline{RM}) =
    \frac{\sum\limits_{doc\in MSet}wdf(t,doc)}
         {\sum\limits_{doc\in MSet,\ t'\in doc}wdf(t',doc)}

    P(t|RM) = \sum_{doc\in MSet} P(t|doc)P(doc|query)

    P(doc|query) = \frac{weight(doc)}{\sum\limits_{d\in MSet}weight(d)}

    P(t|doc) = \alpha \frac{wdf(t,doc)}
                           {\sum\limits_{t'\in doc} wdf(t', doc)}
               + (1 - \alpha) P(t|\overline{RM})


Viewing the text as a list of words `[W1, W2, ... Wn]`, for each Wi
S(stemmed(Wi)) is calculated, resulting in `[S1, S2, ... Sn]`.

Due to imperfections in the model interrupts in the score array may
appear, altering the result. To remove the interrupts, any local minimum
`Si` is smoothed to the average of `Si-1` and `Si+1`.

The snippet is located by finding the best sum of a fixed size window
in the score array. The first sentence that overlaps the window
represents the start of the snippet. The default window size is set to
25, but it can be changed using the ``set_window_size(int window_size)``
method.

The alpha coefficient can be set using the
``set_smoothing_coef(double coef)`` method. The default value is .5.
