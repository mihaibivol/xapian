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
#include <fstream>
#include <queue>

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

void
Snipper::set_mset(const MSet & mset)
{
    internal->calculate_rm(mset);
}

string
Snipper::generate_snippet(const string & text)
{
    return internal->generate_snippet(text);
}

void
Snipper::set_stemmer(const Stem & stemmer)
{
    internal->stemmer = stemmer;
}

bool
Snipper::Internal::is_stemmed(const string & term)
{
    return (term.length() > 0 && term[0] == 'Z');
}

void
Snipper::Internal::calculate_rm(const MSet & mset)
{
    rm_docid current_id = 0;

    // Initialize global relevance model info.
    rm_total_weight = 0;
    rm_coll_size = 0;
    rm_documents.erase(rm_documents.begin(), rm_documents.end());
    rm_term_data.erase(rm_term_data.begin(), rm_term_data.end());

    // Index document and term data.
    for (MSetIterator ms_it = mset.begin(); ms_it != mset.end(); ms_it++) {
	rm_total_weight += ms_it.get_weight();
	const Document & doc = ms_it.get_document();
	int doc_size = 0;
	for (TermIterator term_it = doc.termlist_begin(); term_it != doc.termlist_end(); term_it++) {
	    if (is_stemmed(*term_it)) {
		// Increase current document size.
		doc_size += term_it.get_wdf();

		// Update term information.
		RMTermInfo & term_info = rm_term_data[*term_it];
		term_info.coll_occurence += term_it.get_wdf();

		// Add new document that indexes the term.
		pair<rm_docid, int> td_data = make_pair(current_id, term_it.get_wdf());
		term_info.indexed_docs_freq.push_back(td_data);
	    }
	}

	rm_coll_size += doc_size;

	rm_documents.push_back(RMDocumentInfo(current_id++, doc_size, ms_it.get_weight()));
    }
}

string
Snipper::Internal::generate_snippet(const string & text)
{
    string ret_value;

    Document text_doc;
    TermGenerator term_gen;

    term_gen.set_document(text_doc);
    term_gen.set_stemmer(stemmer);
    term_gen.index_text(text);

    const Document & snippet_doc = term_gen.get_document();

    // Dummy docterms array
    vector<pair<int, string> > docterms;
    for (TermIterator it = snippet_doc.termlist_begin(); it != snippet_doc.termlist_end(); it++) {
	if (is_stemmed(*it))
	    continue;

	for (PositionIterator pit = it.positionlist_begin(); pit != it.positionlist_end(); pit++) {
	    docterms.push_back(make_pair(*pit, *it));
	}
    }

    sort(docterms.begin(), docterms.end());

    // Relevance model for each term.
    map<string, double> term_score;


    // Smootihg coefficient for relevance probability.
    double alpha = .5;

    // Init docterms score.
    for (vector<pair<int, string> >::iterator it = docterms.begin(); it < docterms.end(); it++) {
	string term = "Z" + stemmer(it->second);
	term_score[term] = 0;
    }

    for (map<string, double>::iterator it = term_score.begin(); it != term_score.end(); it++) {
	const string & term = it->first;

	const RMTermInfo & term_info = rm_term_data[term];
	double irrelevant_prob = (double)term_info.coll_occurence / rm_coll_size;
	double relevant_prob = 0;
	for (unsigned int i = 0; i < term_info.indexed_docs_freq.size(); i++) {
	    rm_docid c_docid = term_info.indexed_docs_freq[i].first;
	    // Occurrence of term in document.
	    int doc_freq = term_info.indexed_docs_freq[i].second;

	    // Document info.
	    const RMDocumentInfo & rm_doc_info = rm_documents[c_docid];
	    // Probability for term to be relevant in the current document.
	    double term_doc_prob = alpha * ((double)doc_freq / rm_doc_info.document_size)
				   + (1 - alpha) * irrelevant_prob;

	    // Probability for the current document to be relevant to the query.
	    double doc_query_prob = rm_doc_info.weight / rm_total_weight;

	    relevant_prob += term_doc_prob * doc_query_prob;
	}

	// Add relevance to map.
	it->second = relevant_prob - irrelevant_prob;
    }

    // Calculate basic snippet.
    unsigned int window_size = 25;
    unsigned int snippet_begin = 0;
    unsigned int snippet_end = window_size < docterms.size() ? window_size : docterms.size();
    double sum = 0;
    double max_sum = 0;
    vector<double> docterms_relevance;

    ofstream out_stream(dumpfile.c_str());

    for (unsigned int i = 0; i < docterms.size(); i++) {
	string term = "Z" + stemmer(docterms[i].second);
	docterms_relevance.push_back(term_score[term]);
    }

    // Remove interrupts.
    for (unsigned int i = 0; i < docterms.size(); i++) {
	double prev_score = i > 0 ? docterms_relevance[i - 1] : 0;
	double next_score = i < (docterms.size() - 1) ? docterms_relevance[i + 1] : 0;
	if (docterms_relevance[i] < prev_score &&
	    docterms_relevance[i] < next_score) {
	    docterms_relevance[i] = (prev_score + next_score) / 2;
	}
	out_stream << i << " " << docterms_relevance[i] << endl;
    }

    for (unsigned int i = snippet_begin; i < snippet_end; i++) {
	double score = docterms_relevance[i];
	sum += score;
    }
    max_sum = sum;

    for (unsigned int i = snippet_end; i < docterms.size(); i++) {
	double score = docterms_relevance[i];
	sum += score;

	double head_score = docterms_relevance[i - window_size];
	sum -= head_score;

	if (sum > max_sum) {
	    max_sum = sum;
	    snippet_begin = i - window_size + 1;
	    snippet_end = i + 1;
	}
    }

    size_t last_pos = 0;

    // Retrieve actual snippet.
    string snippet;
    // Snippet size in bytes.
    unsigned int snippet_size = 200;

    unsigned int current_size = 0;
    do {
	size_t new_pos = text.find('.', last_pos);
	if (new_pos == string::npos) {
	    break;
	}

	string sentence = text.substr(last_pos, new_pos - last_pos);
	Document sent_doc;
	term_gen.set_document(sent_doc);
	term_gen.index_text(sentence);

	int sent_size = 0;
	for (TermIterator it = sent_doc.termlist_begin(); it != sent_doc.termlist_end(); it++) {
	    if (is_stemmed(*it)) {
		sent_size += it.get_wdf();
	    }
	}

	current_size += sent_size;
	if (current_size > snippet_begin)
	    snippet += sentence;

	if (current_size > snippet_end)
	    break;

	last_pos = new_pos + 1;
    } while (true);

    snippet = snippet.substr(0, snippet_size);

    ret_value = snippet;

    return snippet;
}

}
