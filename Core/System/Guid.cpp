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

#include <System/Guid.h>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

#include <vector>
#include <sstream>


#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

namespace System
{
   namespace Private
   {
      class Guid : public System::Pimpl
      {
      public:
         Guid()
            : referenceCount(0)
            , guid(boost::uuids::nil_generator()())
         {}

         Guid(const std::string& string)
            : referenceCount(0)
            , guid(boost::uuids::string_generator()(string))
         {}

         size_t ReferenceCount() const;

         void Random()
         {
            guid = boost::uuids::random_generator()();
         }

         std::string ToString()
         {
            std::stringstream ss;
            ss << guid;
            return ss.str();
         }

         void GetBytes(std::vector<unsigned char>& bytes) const
         {
            bytes.clear();
            std::copy(guid.begin(), guid.end(), bytes.begin());
         }

         int referenceCount;
         boost::uuids::uuid guid;
      };
   }
}

using namespace System;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL_REF(r) static_cast<Private::Guid*>((r).p)
#define PIMPL Private::Guid* p(PIMPL_REF(*this));

size_t Private::Guid::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

Guid::Guid()
  : p(new Private::Guid)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Guid::Guid(String string)
  : p(new Private::Guid(string))
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Guid::~Guid()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

Guid::Guid(const Guid& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Guid& Guid::operator =(const Guid& src)
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

size_t Guid::HashCode() const
{
   PIMPL
   return (size_t)p;
}

std::string Guid::ToString() const
{
   PIMPL
   return p->ToString();
}

Guid Guid::Parse(String string)
{
   return Guid(string);
}

Guid Guid::New()
{
   Guid ret;
   Private::Guid* p(PIMPL_REF(ret));
   p->Random();
   return ret;
}

Guid Guid::Empty()
{
   return Guid();
}

bool Guid::operator ==(const Guid comp) const
{
   PIMPL
   Private::Guid* c(PIMPL_REF(comp));
   return p->guid==c->guid;
}

