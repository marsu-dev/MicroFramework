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
            class Dictionary : public Object
            {
            public:
               Dictionary();
               virtual ~Dictionary();
               Dictionary(const Dictionary& src);
               Dictionary& operator =(const Dictionary& src);

               virtual size_t HashCode() const;

               bool Empty() const;
               size_t Count() const;
               bool Contains(const ObjectRef& key) const;
               ObjectRef operator[](const ObjectRef& key) const;

               void Add(const ObjectRef& key, const ObjectRef& value);
               void Remove(const ObjectRef& key);
               void Clear();

               ObjectRef& operator[](const ObjectRef& key);

               List AllKeys() const;

            private:
               Pimpl* p;
            };
         }

         template<class K, class V>
         class Dictionary : public Object
         {
         public:
            size_t HashCode() const { return dictionary.HashCode(); }

            bool Empty() const
            {
               return dictionary.Empty();
            }

            size_t Count() const
            {
               return dictionary.Count();
            }

            bool Contains(const K k) const
            {
               return dictionary.Contains(ObjectRef::Create(k));
            }

            V operator[](const K k) const
            {
               ObjectRef obj(dictionary[ObjectRef::Create(k)]);

               return obj.Get<V>();
            }

            void Add(const K k, const V v)
            {
               dictionary.Add(ObjectRef::Create(k), ObjectRef::Create(v));
            }

            void Remove(const K k)
            {
               dictionary.Remove(ObjectRef::Create(k));
            }

            void Clear()
            {
               dictionary.Clear();
            }

            List<K> AllKeys() const
            {
               List<K> ret;
               ObjectCollection objects = dictionary.AllKeys().ToArray();
               ObjectCollection::iterator it = objects.begin();
               while(it != objects.end())
               {
                  ObjectRef& obj(*it++);
                  if(obj.IsTypeOf<K>())
                     ret.Add(obj.Get<K>());
               }

               return ret;
            }

         private:
            Detail::Dictionary dictionary;
         };
      }
   }
}
