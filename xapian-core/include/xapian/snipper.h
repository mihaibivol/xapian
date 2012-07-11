#ifndef XAPIAN_INCLUDED_SNIPPER_H
#define XAPIAN_INCLUDED_SNIPPER_H

#include <string>
#include <xapian/intrusive_ptr.h>
#include <xapian/visibility.h>

namespace Xapian {

class MSet;
class Stem;

class XAPIAN_VISIBILITY_DEFAULT Snipper {
  public:

    /// Class representing snipper internals
    class Internal;

    Xapian::Internal::intrusive_ptr<Internal> internal;

    Snipper();

    Snipper(const Snipper & other);

    ~Snipper();

    void set_stemmer(const Xapian::Stem & stemmer);

    void set_mset(const MSet & mset);

    void set_window_size(unsigned int window_size);

    void set_smoothing_coef(double coef);

    void set_rm_docno(unsigned int docno);

    std::string generate_snippet(const std::string & text);
};


}

#endif /* XAPIAN_INCLUDED_SNIPPER_H */

