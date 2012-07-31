#ifndef XAPIAN_INCLUDED_SNIPPER_H
#define XAPIAN_INCLUDED_SNIPPER_H

#include <string>
#include <xapian/intrusive_ptr.h>
#include <xapian/visibility.h>

namespace Xapian {

class MSet;
class Stem;

/** Class used to generate snippets from a given text.
 *
 *  For generating a snippet, a MSet is needed to calculate a relevance model.
 */
class XAPIAN_VISIBILITY_DEFAULT Snipper {
  public:

    /// Class representing snipper internals
    class Internal;

    Xapian::Internal::intrusive_ptr<Internal> internal;

    Snipper();

    Snipper(const Snipper & other);

    ~Snipper();

    /** Set the stemmer for the Snipper object. */
    void set_stemmer(const Xapian::Stem & stemmer);

    /** Set the MSet and calculate the relevance model according to it. */
    void set_mset(const MSet & mset);

    void set_window_size(unsigned int window_size);

    void set_smoothing_coef(double coef);

    void set_rm_docno(unsigned int docno);

    /** Generate snippet from given text.
     *
     * @param text  The text from which to generate the snippet
     *
     * @return	    Text of the snippet relevant to the model from input.
     */
    std::string generate_snippet(const std::string & text);
};


}

#endif /* XAPIAN_INCLUDED_SNIPPER_H */

