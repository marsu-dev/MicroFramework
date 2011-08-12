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

#pragma once

#include <System/Object.h>
#include <System/ObjectRef.h>
#include <System/Collections/Generic/List.h>

namespace System
{
   namespace Collections
   {
      namespace Generic
      {
         namespace Detail
         {
            class Set : public Object
            {
            public:
               Set();
               virtual ~Set();
               Set(const Set& src);
               Set& operator =(const Set& src);

               virtual size_t HashCode() const;

               bool Empty() const;
               size_t Count() const;
               bool Contains(const ObjectRef& object) const;
               void Add(const ObjectRef& object);
               void Remove(const ObjectRef& object);
               void Clear();

               List ToList() const;

            private:
               Pimpl* p;
            };
         }

         template<class T>
         class Set : public Object
         {
         public:
            size_t HashCode() const
            {
               return set.HashCode();
            }

            bool Empty() const
            {
               return set.Empty();
            }

            size_t Count() const
            {
               return set.Count();
            }

            bool Contains(const T t) const
            {
               return set.Contains(ObjectRef::Create(t));
            }

            void Add(const T t)
            {
               set.Add(ObjectRef::Create(t));
            }

            void Remove(const T t)
            {
               set.Remove(ObjectRef::Create(t));
            }

            void Clear()
            {
               set.Clear();
            }

            List<T> ToList() const
            {
               List<T> ret;

               ObjectCollection objects = set.ToList().ToArray();
               ObjectCollection::iterator it = objects.begin();
               while(it != objects.end())
               {
                  ObjectRef& obj(*it++);
                  if(obj.IsTypeOf<T>())
                     ret.Add(obj.Get<T>());
               }

               return ret;
            }

         private:
            Detail::Set set;
         };
      }
   }
}
