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
            class PriorityQueue : public Object
            {
            public:
               PriorityQueue();
               virtual ~PriorityQueue();
               PriorityQueue(const PriorityQueue& src);
               PriorityQueue& operator =(const PriorityQueue& src);

               size_t HashCode() const;

               bool Empty() const;
               size_t Count() const;

               void Enqueue(ObjectRef object, int priority);

               ObjectRef Dequeue();
               void Clear();

            private:
               Pimpl* p;
            };
         }

         template<class T>
         class PriorityQueue : public Object
         {
         public:
            size_t HashCode() const { return queue.HashCode(); }
            
            bool Empty() const { return queue.Empty(); }
            size_t Count() const { return queue.Count(); }

            void Enqueue(T t) { Enqueue(t, 1); }
            void Enqueue(T t, int priority) { queue.Enqueue(ObjectRef(new T(t)), priority); }

            T Dequeue()
            {
               ObjectRef obj(queue.Dequeue());
               return obj.Get<T>();
            }
            void Clear() { queue.Clear(); }

         private:
            Detail::PriorityQueue queue;
         };
      }
   }
}
