#include "xapian/snipper.h"
#include "snipperinternal.h"

#include <xapian/enquire.h>
#include <xapian/stem.h>
#include <xapian/document.h>
#include <xapian/termgenerator.h>

#include <string>

using namespace std;

namespace Xapian {

Snipper::Snipper() : internal(new Snipper::Internal)
{
}

Snipper::Snipper(const Snipper & other) : internal(other.internal)
{
}

Snipper::~Snipper()
{
}

string
Snipper::generate_snippet(const MSet & mset, const string & text)
{
    return internal->generate_snippet(mset, text);
}

void
Snipper::set_stemmer(const Stem & stemmer)
{
    internal->stemmer = stemmer;
}

string
Snipper::Internal::generate_snippet(const MSet & mset, const string & text)
{
    Document text_doc;
    TermGenerator term_gen;

    term_gen.set_document(text_doc);
    term_gen.set_stemmer(stemmer);
    term_gen.index_text(text);

    // Ignore warning.
    return mset.get_description() + " " + text_doc.get_description();
}

}
