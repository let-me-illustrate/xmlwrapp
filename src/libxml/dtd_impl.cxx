/*
 * Copyright (C) 2001-2003 Peter J Jones (pjones@pmade.org)
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

// xmlwrapp includes
#include "dtd_impl.h"
#include "utility.h"

// standard includes
#include <string>
#include <cstring>

// libxml2 includes
#include <libxml/parser.h>
#include <libxml/valid.h>
#include <libxml/tree.h>

using namespace xml;
using namespace xml::impl;

namespace
{

extern "C" void dtd_error(void *ctxt, const char *message, ...)
{
    auto *dtd = static_cast<dtd_impl*>(ctxt);

    va_list ap;
    va_start(ap, message);
    printf2string(dtd->error_, message, ap);
    va_end(ap);
}

extern "C" void dtd_warning(void *ctxt, const char*, ...)
{
    auto *dtd = static_cast<dtd_impl*>(ctxt);
    ++dtd->warnings_;
}

} // anonymous namespace


dtd_impl::dtd_impl(const char *filename)
    : warnings_(0), dtd_(nullptr)
{
    if ( (dtd_ = xmlParseDTD(nullptr, reinterpret_cast<const xmlChar*>(filename))) == nullptr)
    {
        error_ = "unable to parse DTD ";
        error_ += filename;
    }
}


dtd_impl::dtd_impl() : warnings_(0), dtd_(nullptr)
{
}


dtd_impl::~dtd_impl()
{
    if (dtd_)
        xmlFreeDtd(dtd_);
}


void dtd_impl::init_ctxt()
{
    std::memset(&vctxt_, 0, sizeof(vctxt_));

    vctxt_.userData = this;
    vctxt_.error    = dtd_error;
    vctxt_.warning  = dtd_warning;
}


bool dtd_impl::validate(xmlDocPtr xmldoc)
{
    init_ctxt();

    if (dtd_)
        return xmlValidateDtd(&vctxt_, xmldoc, dtd_) != 0;
    else
        return xmlValidateDocument(&vctxt_, xmldoc) != 0;
}


xmlDtdPtr dtd_impl::release()
{
    xmlDtdPtr xmldtd = dtd_;
    dtd_ = nullptr;
    return xmldtd;
}
