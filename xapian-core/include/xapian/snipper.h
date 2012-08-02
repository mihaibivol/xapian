/** @file snipper.h
 * @brief Generate snippets from text relevant to MSet.
 */
/* Copyright (C) 2012 Mihai Bivol
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301
 * USA
 */

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

    /// Class representing snipper internals.
    class Internal;

    Xapian::Internal::intrusive_ptr<Internal> internal;

    Snipper();

    Snipper(const Snipper & other);

    ~Snipper();

    /** Set the stemmer for the Snipper object. */
    void set_stemmer(const Xapian::Stem & stemmer);

    /**
     * Set the MSet and calculate the relevance model according to it.
     *
     * @param mset	MSet with the documents relevant to the query.
     * @param rm_docno	Maximum number of documents for the relevance
     *			model (default: 10).
     */
    void set_mset(const MSet & mset,
		  unsigned int rm_docno = 10);

    /**
     * Set the window size used by generate_snippet method.
     *
     * Before this method is called, the default 25 value is used.
     *
     * @param window_size   Size of the window
     */
    void set_window_size(unsigned int window_size);

    /**
     * Set the smootihg coefficient used by generate_snippet method.
     *
     * Before this method is called, the default .5 value is used.
     *
     * @param coef	Smoothing coefficient.
     */
    void set_smoothing_coef(double coef);

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

