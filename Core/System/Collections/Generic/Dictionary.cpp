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

#include <System/Collections/Generic/Dictionary.h>
#include <System/SimpleObject.h>
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
               struct ObjectPair : public System::Pimpl
               {
               public:
                  ObjectPair(ObjectRef Key, ObjectRef Value)
                     : Key(Key)
                     , Value(Value)
                  {}

                  size_t ReferenceCount() const { return 0; }

                  ObjectRef Key;
                  ObjectRef Value;
               };

               typedef std::map<size_t, ObjectPair> ObjectMap;

               class Dictionary : public System::Pimpl
               {
               public:
                  Dictionary()
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

                  bool Contains(const ObjectRef& key) const
                  {
                     return objectMap.find(key.HashCode())!=objectMap.end();
                  }

                  ObjectRef operator[](const ObjectRef& key) const
                  {
                     if(!Contains(key))
                        throw ObjectNotFoundException();

                     return objectMap.find(key.HashCode())->second.Value;
                  }

                  void Add(const ObjectRef& key, const ObjectRef& value)
                  {
                     if(Contains(key))
                        throw ObjectPresentException();

                     objectMap.insert(std::make_pair(key.HashCode(), Private::ObjectPair(key, value)));
                  }

                  void Remove(const ObjectRef& key)
                  {
                     if(!Contains(key))
                        throw ObjectNotFoundException();

                     objectMap.erase(objectMap.find(key.HashCode()));
                  }

                  void Clear()
                  {
                     objectMap.clear();
                  }

                  List AllKeys() const
                  {
                     List ret;

                     Private::ObjectMap::const_iterator it(objectMap.begin());
                     while(it!=objectMap.end())
                     {
                        const ObjectRef& obj((*it++).second.Key);
                        ret.Add(obj);
                     }

                     return ret;
                  }

                  ObjectRef& operator[](const ObjectRef& key)
                  {
                     if(!Contains(key))
                        throw ObjectNotFoundException();
                     return (*objectMap.find(key.HashCode())).second.Value;
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
#define PIMPL Private::Dictionary* p(static_cast<Private::Dictionary*>(this->p));

size_t Private::Dictionary::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

Dictionary::Dictionary()
  : p(new Private::Dictionary)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Dictionary::~Dictionary()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

Dictionary::Dictionary(const Dictionary& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Dictionary& Dictionary::operator =(const Dictionary& src)
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

size_t Dictionary::HashCode() const
{
   PIMPL
   return (size_t)p;
}

bool Dictionary::Empty() const
{
   PIMPL
   return p->Empty();
}

size_t Dictionary::Count() const
{
   PIMPL
   return p->Count();
}

bool Dictionary::Contains(const ObjectRef& key) const
{
   PIMPL
   return p->Contains(key);
}

ObjectRef Dictionary::operator[](const ObjectRef& key) const
{
   PIMPL
   return (*p)[key];
}


void Dictionary::Add(const ObjectRef& key, const ObjectRef& value)
{
   PIMPL
   p->Add(key, value);
}

void Dictionary::Remove(const ObjectRef& key)
{
   PIMPL
   p->Remove(key);
}

void Dictionary::Clear()
{
   PIMPL
   p->Clear();
}

ObjectRef& Dictionary::operator[](const ObjectRef& key)
{
   PIMPL
   return (*p)[key];
}

List Dictionary::AllKeys() const
{
   PIMPL
   return p->AllKeys();
}
