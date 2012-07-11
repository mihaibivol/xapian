#ifndef XAPIAN_INCLUDED_SNIPPERINTERNAL_H
#define XAPIAN_INCLUDED_SNIPPERINTERNAL_H

#include <xapian/stem.h>
#include <xapian/enquire.h>
#include <string>
#include <vector>
#include <map>

namespace Xapian {

class Snipper::Internal : public Xapian::Internal::intrusive_base {
    public:
	typedef int rm_docid;

	struct RMDocumentInfo {
	    /** ID in the relevance model */
	    rm_docid rm_id;
	    /** Document size in terms */
	    int document_size;
	    /** Weight of the document */
	    double weight;

	    RMDocumentInfo(rm_docid rm_id_, int document_size_, double weight_) :
		rm_id(rm_id_),
		document_size(document_size_),
		weight(weight_) { }
	};

	struct RMTermInfo {
	    /** Documents that index the term in relevance model */
	    std::vector<std::pair<rm_docid, int> > indexed_docs_freq;
	    /** Occurance in collection */
	    int coll_occurence;

	    RMTermInfo() : coll_occurence(0) { }
	};

	/** Stemmer used for generating text terms */
	Stem stemmer;
	// For debug purposes.
	std::string dumpfile;

	/** Relevance Model documents. */
	std::vector<RMDocumentInfo> rm_documents;

	/** Relevance model data for each term */
	std::map<std::string, RMTermInfo> rm_term_data;

	/** Relevance model collection size */
	int rm_coll_size;

	/** Relevance model total document weight */
	double rm_total_weight;

	/** Smoothing coeficient used for computing the relevance model */
	double smoothing_coef;

	/** Size of the window that returns the relevant snippet */
	unsigned int window_size;

	/** Relevance model document number */
	unsigned int rm_docno;

	Internal() : rm_coll_size(0), rm_total_weight(0) { }

	/** Return snippet generated from text using the precalculated relevance model */
	std::string generate_snippet(const std::string & text);

	/** Calculate relevance model based on a MSet */
	void calculate_rm(const MSet & mset);

    private:
	bool is_stemmed(const std::string & term);
};
}

#endif /* XAPIAN_INCLUDED_SNIPPERINTERNAL_H */
