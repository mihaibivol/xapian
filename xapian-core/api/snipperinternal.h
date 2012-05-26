#ifndef XAPIAN_INCLUDED_SNIPPERINTERNAL_H
#define XAPIAN_INCLUDED_SNIPPERINTERNAL_H

#include <xapian/stem.h>
#include <xapian/enquire.h>

namespace Xapian {

class Snipper::Internal : public Xapian::Internal::intrusive_base {
    public:
	Stem stemmer;

	std::string generate_snippet(const MSet & mset, const std::string & text);
};
}

#endif /* XAPIAN_INCLUDED_SNIPPERINTERNAL_H */
