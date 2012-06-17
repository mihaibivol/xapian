#include "xapian/snipper.h"
#include "snipperinternal.h"

#include <xapian/enquire.h>
#include <xapian/stem.h>
#include <xapian/document.h>
#include <xapian/termgenerator.h>

#include <string>
#include <vector>
#include <algorithm>
#include <map>

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

    string ret_value("\n###############################\n");

    ret_value += text + "\n";

    ret_value += "\n###############################\n";
    const Document & snippet_doc = term_gen.get_document();

    // Dummy docterms array
    vector<pair<int, string> > docterms;
    for (TermIterator it = snippet_doc.termlist_begin(); it != snippet_doc.termlist_end(); it++) {
	if ((*it).length() > 0 && (*it)[0] == 'Z')
	    continue;

	for (PositionIterator pit = it.positionlist_begin(); pit != it.positionlist_end(); pit++) {
	    docterms.push_back(make_pair(*pit, *it));
	}
    }

    sort(docterms.begin(), docterms.end());

    // Collection size in stemmed terms.
    int coll_size = 0;
    // Document total weight.
    double total_weight = 0;

    // Precalculate total weight and collection size.
    for (MSetIterator ms_it = mset.begin(); ms_it != mset.end(); ms_it++) {
	total_weight += ms_it.get_weight();
        const Document & doc = ms_it.get_document();
	for (TermIterator term_it = doc.termlist_begin(); term_it != doc.termlist_end(); term_it++)
	    if ((*term_it).length() > 0 && (*term_it)[0] == 'Z')
		coll_size+=term_it.get_termfreq();
    }

    // Relevance model for each term.
    map<string, double> term_score;
    for (vector<pair<int, string> >::iterator it = docterms.begin(); it < docterms.end(); it++) {
	string term = "Z" + stemmer(it->second);
	term_score[term] = 0;
    }

    // For each term in snippeted text, calculate irrelevance prob.
    for (map<string, double>::iterator it = term_score.begin(); it != term_score.end(); it++) {
	const string & term = it->first;

	// Occurance of term in all documents.
	int occurance = 0;

	double relevant_prob = 0;

	for (MSetIterator ms_it = mset.begin(); ms_it != mset.end(); ms_it++) {
	    const Document & doc = ms_it.get_document();

	    // Snippeted text term frequency.
	    int tf = 0;
	    // Document size.
	    int document_size = 0;

	    for (TermIterator term_it = doc.termlist_begin(); term_it != doc.termlist_end(); term_it++) {
		if ((*term_it).length() > 0 && (*term_it)[0] == 'Z')
		    document_size += term_it.get_termfreq();

		if (term == *term_it) {
		    tf = term_it.get_termfreq();
		}
	    }

	    occurance += tf;
	    relevant_prob += ((double)tf / document_size) * (ms_it.get_weight() / total_weight);
	}

	double irrelevant_prob = (double)occurance / coll_size;

	it->second = relevant_prob - irrelevant_prob;
    }

    // Calculate basic snippet.

    unsigned int snippet_size = 15;
    unsigned int snippet_begin = 0;
    unsigned int snippet_end = snippet_size < docterms.size() ? snippet_size : docterms.size();
    double sum = 0;
    double max_sum = 0;

    for(unsigned int i = snippet_begin; i < snippet_end; i++) {
	string term = "Z" + stemmer(docterms[i].second);
	double score = term_score[term];
	sum += score;
    }
    max_sum = sum;

    for (unsigned int i = snippet_end; i < docterms.size(); i++) {
	string term = "Z" + stemmer(docterms[i].second);
	double score = term_score[term];
	sum += score;

	string head_term = "Z" + stemmer(docterms[i - snippet_size].second);
	double head_score = term_score[head_term];
	sum -= head_score;

	if (sum > max_sum) {
	    max_sum = sum;
	    snippet_begin = i - snippet_size + 1;
	    snippet_end = i + 1;
	}
    }

    for (unsigned int i = snippet_begin; i <= snippet_end; i++)
	ret_value += docterms[i].second + " ";


    return ret_value;
}

}
