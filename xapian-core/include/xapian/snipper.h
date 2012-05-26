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

    std::string generate_snippet(const MSet & mset, const std::string & text);
};


}

#endif /* XAPIAN_INCLUDED_SNIPPER_H */

