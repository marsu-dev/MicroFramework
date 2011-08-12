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

#include <System/Collections/Generic/List.h>

#include <System/Exception.h>
#include <exception>
#include <vector>
#include <algorithm>

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
               class List : public System::Pimpl
               {
               public:
                  List() 
                     : referenceCount(0) 
                  {}

                  size_t ReferenceCount() const;

                  size_t Empty() const
                  {
                     return objects.empty();
                  }

                  size_t Count() const
                  {
                     return objects.size();
                  }

                  void Add(const ObjectRef& object)
                  {
                     objects.push_back(object);
                  }

                  void AddRange(const System::Collections::Generic::Detail::List& newObjects)
                  {
                     if(this==newObjects.p)
                     {
                        System::Collections::Generic::Detail::List newList;
                        newList.AddRange(newObjects);
                        AddRange(newList);
                        return;
                     }

                     List* np = static_cast<List*>(newObjects.p);
                     const ObjectCollection& objs(np->objects);
                     ObjectCollection::const_iterator it(objs.begin());
                     while(it!=objs.end())
                     {
                        Add(*it++);
                     }
                  }

                  System::Object& At(size_t index)
                  {
                     if(index>=objects.size())
                        throw OutOfBoundException();
                     return objects[index];
                  }

                  void RemoveAt(size_t index)
                  {
                     if(index>=objects.size())
                        throw OutOfBoundException();

                     objects.erase(objects.begin()+index);
                  }

                  void Clear()
                  {
                     objects.clear();
                  }

                  void Reverse()
                  {
                     std::reverse(objects.begin(), objects.end());
                  }

                  void ForEach(ObjectDelegate& delegate)
                  {
                     ObjectCollection::iterator it(objects.begin());
                     while(it!=objects.end())
                     {
                        ObjectRef& object(*it++);
                        try {
                           delegate(object);
                        }
                        catch(System::Exception&){}
                        catch(std::exception&){}
                        catch(...){}
                     }
                  }

                  int referenceCount;
                  ObjectCollection objects;
               };
            }
         }
      }
   }
}

using namespace System::Collections::Generic::Detail;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::List* p(static_cast<Private::List*>(this->p));

size_t Private::List::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

List::List()
  : p(new Private::List)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

List::~List()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

List::List(const List& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

List& List::operator =(const List& src)
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

bool List::Empty() const
{
   PIMPL
   return p->Empty();
}

size_t List::Count() const
{
   PIMPL
   return p->Count();
}

void List::Add(const ObjectRef& object)
{
   PIMPL
   p->Add(object);
}

void List::AddRange(const List& objects)
{
   PIMPL
   p->AddRange(objects);
}

System::Object& List::At(size_t index)
{
   PIMPL
   return p->At(index);
}

void List::RemoveAt(size_t index)
{
   PIMPL
   p->RemoveAt(index);
}

void List::Clear()
{
   PIMPL
   p->Clear();
}

void List::Reverse()
{
   PIMPL
   p->Reverse();
}

size_t List::HashCode() const
{
   PIMPL
   return (size_t)p;
}

System::Collections::ObjectCollection List::ToArray() const
{
   PIMPL
   return p->objects;
}

void List::ForEach(ObjectDelegate& delegate)
{
   PIMPL
   p->ForEach(delegate);
}
