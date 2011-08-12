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
#include <System/Collections/Generic/ListDelegate.h>

#include <vector>

namespace System
{
   namespace Collections
   {
      typedef std::vector<ObjectRef> ObjectCollection;

      namespace Generic
      {
         namespace Detail
         {
            class List : public Object
            {
            public:
               List();
               virtual ~List();
               List(const List& src);
               List& operator =(const List& src);

               virtual bool Empty() const;
               virtual size_t Count() const;
               void Add(const ObjectRef& object);
               void AddRange(const List& objects);

               const System::Object& At(size_t index) const;

               void RemoveAt(size_t index);
               void Clear();
               void Reverse();

               virtual size_t HashCode() const;

               ObjectCollection ToArray() const;

               void ForEach(ObjectDelegate& delegate);

            public:
               Pimpl* p;
            };
         }

         template<class T>
         class List : public Object
         {
         public:
            size_t HashCode() const { return list.HashCode(); }

            bool Empty() const
            {
               return list.Empty();
            }

            size_t Count() const
            {
               return list.Count();
            }

            void Add(const T& t)
            {
               list.Add(ObjectRef::Create(t));
            }

            void AddRange(const List<T>& coll)
            {
               list.AddRange(coll.list);
            }

            T At(size_t index) const
            {
                const ObjectRef& ret(dynamic_cast<const ObjectRef&>(list.At(index)));
                return ret.Get<T>();
            }

            T operator[](size_t index) const
            {
                return At(index);
            }

            void RemoveAt(size_t index) { list.RemoveAt(index); }

            void Clear() { list.Clear(); }

            void Reverse() { list.Reverse(); }

            std::vector<T> ToArray() const
            {
               std::vector<T> ret;

               ObjectCollection objects = list.ToArray();
               ObjectCollection::iterator it = objects.begin();
               while(it != objects.end())
               {
                  ObjectRef& obj(*it++);
                  if(obj.IsTypeOf<T>())
                     ret.push_back(obj.Get<T>());
               }

               return ret;
            }

            template<class F>
            void ForEach(F f)
            {
               ListFunctor<F> func(f);
               list.ForEach(func);
            }

            void ForEach(ListDelegate<T> delegate)
            {
               list.ForEach(delegate);
            }

         private:
            Detail::List list;
            template<class F>
            class ListFunctor : public ListDelegate<T>
            {
               F f;
            public:
               ListFunctor(F f) : f(f) {}
               virtual void operator()(T& t) { f(t); }
            };
         };
      }
   }
}
