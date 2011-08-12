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

#include <System/ObjectRef.h>
#include <System/Exception.h>

#include <boost/thread/recursive_mutex.hpp>
typedef boost::lock_guard<boost::recursive_mutex> lock_t;

namespace System
{
   namespace Private
   {
      class ObjectRef : public System::Pimpl
      {
      public:
         ObjectRef()
            : referenceCount(0)
            , object(0)
         {}

         ~ObjectRef()
         {
            delete object;
         }

         size_t ReferenceCount() const;

         void Reset()
         {
            delete object;
            object = NULL;
         }

         int referenceCount;
         Object* object;
      };
   }
}

using namespace System;

static boost::recursive_mutex& m(){
   static boost::recursive_mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::ObjectRef* p(static_cast<Private::ObjectRef*>(this->p));

size_t Private::ObjectRef::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

ObjectRef::ObjectRef(Object* object)
  : p(new Private::ObjectRef)
{
   if(!object)
      throw NullPointerException();
   LOCK
   PIMPL
   p->referenceCount++;
   p->object = object;
}

ObjectRef::~ObjectRef()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

ObjectRef::ObjectRef(const ObjectRef& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

ObjectRef& ObjectRef::operator =(const ObjectRef& src)
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

size_t ObjectRef::HashCode() const
{
   PIMPL
   if(!p->object)
      return (size_t)p;

   return p->object->HashCode();
}

void ObjectRef::Reset()
{
   PIMPL
   return p->Reset();
}

ObjectRef::operator bool() const
{
   PIMPL
   return p->object!=NULL;
}

Object& ObjectRef::Get()
{
   PIMPL
   if(!p->object)
      throw NullPointerException();
   return *p->object;
}

const Object& ObjectRef::Get() const
{
   PIMPL
   if(!p->object)
      throw NullPointerException();
   return *p->object;
}
