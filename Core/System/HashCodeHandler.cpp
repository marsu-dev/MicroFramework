/**
 * Copyright (c) 2011 Michel Foucault
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#include <System/HashCodeHandler.h>

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

namespace System
{
   namespace Private
   {
      class HashCodeHandler : public System::Pimpl
      {
      public:
         HashCodeHandler()
            : referenceCount(0) 
         {}

         size_t ReferenceCount() const;

         int referenceCount;
      };
   }
}

using namespace System;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::HashCodeHandler* p(static_cast<Private::HashCodeHandler*>(this->p));

size_t Private::HashCodeHandler::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

HashCodeHandler::HashCodeHandler()
  : p(new Private::HashCodeHandler)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

HashCodeHandler::~HashCodeHandler()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

HashCodeHandler::HashCodeHandler(const HashCodeHandler& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

HashCodeHandler& HashCodeHandler::operator =(const HashCodeHandler& src)
{
   if(this==&src)
      return *this;

   LOCK
   {
      PIMPL
      p->referenceCount--;
      if(!p->referenceCount)
         delete p;
   }

   this->p = src.p;
   PIMPL
   p->referenceCount++;
   
   return *this;
}

size_t HashCodeHandler::HashCode() const
{
   PIMPL
   return (size_t)p;
}

HashCodeHandler::operator size_t() const
{
   return HashCode();
}
