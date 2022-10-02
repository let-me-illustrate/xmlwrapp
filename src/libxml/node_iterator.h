/*
 * Copyright (C) 2001-2003 Peter J Jones (pjones@pmade.org)
 *               2009      Vaclav Slavik <vslavik@gmail.com>
 * All Rights Reserved
 * 
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 * 3. Neither the name of the Author nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS''
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
 * TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
 * PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR
 * OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
 * AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

#ifndef _xmlwrapp_node_iterator_h_
#define _xmlwrapp_node_iterator_h_

// xmlwrapp includes
#include "xmlwrapp/node.h"

// libxml includes
#include <libxml/tree.h>

namespace xml
{

namespace impl
{

// helper to obtain the next node in "filtering" iterators (as used by
// nodes_view and const_nodes_view)
//
// Note: This class is reference-counted; don't delete instance of it, use
//       dec_ref() and inc_ref(). Newly created instance has reference count
//       of 1.
class iter_advance_functor
{
public:
    iter_advance_functor() = default;

    void inc_ref()
    {
        refcnt_++;
    }

    void dec_ref()
    {
        if ( --refcnt_ == 0 )
            delete this;
    }

    virtual xmlNodePtr operator()(xmlNodePtr node) const = 0;

protected:
    // use inc_ref(), dec_ref() instead of using the dtor explicitly
    virtual ~iter_advance_functor() = default;

private:
    // no copy ctor or assignment
    iter_advance_functor(const iter_advance_functor& other) = delete;
    iter_advance_functor& operator=(const iter_advance_functor& other) = delete;

private:
    int refcnt_{1};
};

// base iterator class
class node_iterator
{
public:
    node_iterator() : fake_node_(0), node_(nullptr) {}

    node_iterator(node &parent)
        : fake_node_(0),
          node_(reinterpret_cast<xmlNodePtr>(parent.get_node_data()))
    {
    }

    node_iterator(xmlNodePtr xmlnode) : fake_node_(0), node_(xmlnode) {}
    node_iterator(const node_iterator& other) : fake_node_(0), node_(other.node_) {}
    node_iterator& operator=(const node_iterator& other)
        { node_ = other.node_; return *this;}

    node *get() const;
    xmlNodePtr get_raw_node() { return node_; }

    void advance() { node_ = node_->next; }
    void advance(iter_advance_functor& next) { node_ = next(node_); }

private:
    mutable node fake_node_;
    xmlNodePtr node_;
};

} // namespace impl

} // namespace xml

#endif // _xmlwrapp_node_iterator_h_
