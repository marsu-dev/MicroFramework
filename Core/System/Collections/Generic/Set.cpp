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

#include <System/Collections/Generic/Set.h>
#include <System/Exception.h>

#include <map>

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

namespace System
{
   namespace Collections
   {
      namespace Generic
      {
         namespace Detail
         {
            namespace Private
            {
               typedef std::map<size_t, ObjectRef> ObjectMap;

               class Set : public System::Pimpl
               {
               public:
                  Set()
                     : referenceCount(0) 
                  {}

                  size_t ReferenceCount() const;

                  bool Empty() const
                  {
                     return objectMap.empty();
                  }

                  size_t Count() const
                  {
                     return objectMap.size();
                  }

                  bool Contains(const ObjectRef& object) const
                  {
                     const Object& obj(object);
                     return objectMap.find(obj.HashCode())!=objectMap.end();
                  }

                  void Add(const ObjectRef& object)
                  {
                     if(Contains(object))
                        throw Exception();

                     const Object& obj(object);
                     objectMap.insert(std::make_pair(object.HashCode(), object));
                  }

                  void Remove(const ObjectRef& object)
                  {
                     if(!Contains(object))
                        throw Exception();

                     objectMap.erase(objectMap.find(object));
                  }

                  void Clear()
                  {
                     objectMap.clear();
                  }

                  List ToList() const
                  {
                     List ret;

                     Private::ObjectMap::const_iterator it(objectMap.begin());
                     while(it!=objectMap.end())
                     {
                        const ObjectRef& obj((*it++).second);
                        ret.Add(obj);
                     }

                     return ret;
                  }

                  int referenceCount;
                  Private::ObjectMap objectMap;
               };
            }
         }
      }
   }
}

using namespace System;
using namespace System::Collections::Generic::Detail;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::Set* p(static_cast<Private::Set*>(this->p));

size_t Private::Set::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

Set::Set()
  : p(new Private::Set)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Set::~Set()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

Set::Set(const Set& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Set& Set::operator =(const Set& src)
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

size_t Set::HashCode() const
{
   PIMPL
   return (size_t)p;
}

bool Set::Empty() const
{
   PIMPL
   return p->Empty();
}

size_t Set::Count() const
{
   PIMPL
   return p->Count();
}

bool Set::Contains(const ObjectRef& object) const
{
   PIMPL
   return p->Contains(object);
}

void Set::Add(const ObjectRef& object)
{
   PIMPL
   p->Add(object);
}

void Set::Remove(const ObjectRef& object)
{
   PIMPL
   p->Remove(object);
}

void Set::Clear()
{
   PIMPL
   p->Clear();
}

List Set::ToList() const
{
   PIMPL
   return p->ToList();
}
