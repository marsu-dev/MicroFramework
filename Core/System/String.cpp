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

#include <System/String.h>
#include <System/SimpleObject.h>

#include <map>
#include <vector>
#include <algorithm>
#include <boost/ptr_container/ptr_list.hpp>
#include <iostream>

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

typedef size_t hash_t;
static hash_t SdbmHash(const std::string& key);

namespace System
{
   namespace Private
   {
      class String : public System::Pimpl
      {
      public:
         String()
            : referenceCount(0)
            , string()
         {}

         String(const std::string& string)
            : referenceCount(0)
            , string(string)
         {}

         size_t ReferenceCount() const;

         int referenceCount;
         std::string string;

         typedef boost::ptr_list<String> Collection;
         typedef boost::shared_ptr<Collection> CollectionPtr;
         typedef std::map<hash_t, CollectionPtr> Map;
      };
   }

   class StringFactory : public SimpleObject
   {
   public:
      typedef Private::String::Map PimplMap;
      typedef Private::String::Collection PimplCollection;
      typedef Private::String::CollectionPtr PimplCollectionPtr;
      typedef std::vector<PimplMap> PimplMapCollection;

      static PimplCollection::iterator Find(PimplCollection& pimpls, const std::string& string)
      {
         PimplCollection::iterator it = pimpls.begin();
         while(it != pimpls.end())
         {
            PimplCollection::iterator f(it++);
            Private::String& pimpl(*f);
            if(pimpl.string == string)
               return f;
         }
         return pimpls.end();
      }

      Private::String& FromString(const std::string& string)
      {
         PimplCollection& pimpls(PimplCollectionFromHash(SdbmHash(string)));

         PimplCollection::iterator f = Find(pimpls, string);
         if(f != pimpls.end())
            return *f;

         pimpls.push_back(new Private::String(string));
         return pimpls.back();
      }

      void Remove(const std::string& string)
      {
         const size_t hash = SdbmHash(string);
         PimplCollection& pimpls(PimplCollectionFromHash(hash));

         pimpls.erase(Find(pimpls, string));
         if(pimpls.empty())
            RemovePimplCollection(hash);
      }

   private:
      PimplMapCollection pimplMaps;
      PimplMap& PimplMapFromHash(size_t hash)
      {
         hash ^= 0x55555555;
         if(pimplMaps.empty())
            pimplMaps.resize(2<<10);

         return pimplMaps[hash%pimplMaps.size()];
      }

      #define HASH_MASK 0x00FFFFFF;

      PimplCollection& PimplCollectionFromHash(size_t hash)
      {
         PimplMap& pimplMap(PimplMapFromHash(hash));

         hash &= HASH_MASK;

         PimplCollectionPtr& pPimpls = pimplMap[hash];
         if(!pPimpls)
            pPimpls.reset(new PimplCollection);

         return *pPimpls;
      }

      void RemovePimplCollection(size_t hash)
      {
         PimplMap& pimplMap(PimplMapFromHash(hash));

         hash &= HASH_MASK;

         pimplMap.erase(hash);
      }
   };
}

using namespace System;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL_REF(r) static_cast<Private::String*>((r).p)
#define PIMPL Private::String* p(PIMPL_REF(*this));

size_t Private::String::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

static StringFactory stringFactory;

static hash_t sdbm_hash(const unsigned char *key)
{
	hash_t h=0;
	while(*key) h=*key++ + (h<<6) + (h<<16) - h;
	return h;
}

static hash_t SdbmHash(const std::string& key)
{
   return sdbm_hash((const unsigned char *)key.c_str());
}

String::String()
   : p(NULL)
{
   LOCK
   this->p = &stringFactory.FromString(std::string());
   PIMPL
   p->referenceCount++;
}

String::String(const std::string& string)
   : p(NULL)
{
   LOCK
   this->p = &stringFactory.FromString(string);
   PIMPL
   p->referenceCount++;
}

String::~String()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      stringFactory.Remove(p->string);
}

String::String(const String& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

String& String::operator =(const String& src)
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

size_t String::HashCode() const
{
   PIMPL
   return (size_t)p;
}

bool String::operator ==(const String comp) const
{
   return HashCode()==comp.HashCode();  
}

const String String::Empty()
{
   static String empty(std::string(""));
   return empty;
}

String::operator std::string() const
{
   PIMPL
   return p->string;
}

String String::Contat(String right) const
{
   return Contat(*this, right);
}

String String::Contat(String left, String right)
{
   return String(PIMPL_REF(left)->string + PIMPL_REF(right)->string);
}

String operator +(String left, String right)
{
   return left.Contat(right);
}
