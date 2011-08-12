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

#include <System/Collections/Generic/PriorityQueue.h>

#include <ctime>
#include <queue>
#include <functional>

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
               struct PriorityItem : public System::Pimpl
               {
               public:
                  PriorityItem(int Priority, ObjectRef Object)
                     : Priority(Priority)
                     , Object(Object)
                     , itemId(PriorityItem::counter++)
                  {}

                  size_t ReferenceCount() const { return 0; }

                  int Priority;
                  ObjectRef Object;
                  size_t itemId;
                  static size_t counter;
               };
               size_t PriorityItem::counter = 0;

               struct PriorityItemComparer : public std::binary_function<PriorityItem&, PriorityItem&, bool>, public System::Pimpl
               {
                  size_t ReferenceCount() const { return 0; }
                  bool operator()(const PriorityItem& lhs, const PriorityItem& rhs) const
                  {
                     if(lhs.Priority == rhs.Priority)
                        return lhs.itemId > rhs.itemId; // make fifo for equal priority
                     return lhs.Priority < rhs.Priority;
                  }
               };

               typedef std::priority_queue<PriorityItem, std::vector<PriorityItem>, PriorityItemComparer> ObjectQueue;

               class PriorityQueue : public System::Pimpl
               {
               public:
                  PriorityQueue()
                     : referenceCount(0)
                  {}

                  size_t ReferenceCount() const;

                  bool Empty() const { return queue.empty(); }
                  size_t Count() const { return queue.size(); }

                  void Enqueue(ObjectRef& object, int priority)
                  {
                     queue.push(PriorityItem(priority, object));
                  }

                  ObjectRef Dequeue()
                  {
                     ObjectRef ret(queue.top().Object);
                     queue.pop();
                     return ret;
                  }

                  void Clear()
                  {
                     while(!queue.empty())
                        queue.pop();
                  }

                  int referenceCount;

                  ObjectQueue queue;
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
#define PIMPL Private::PriorityQueue* p(static_cast<Private::PriorityQueue*>(this->p));

size_t Private::PriorityQueue::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

PriorityQueue::PriorityQueue()
  : p(new Private::PriorityQueue)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

PriorityQueue::~PriorityQueue()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

PriorityQueue::PriorityQueue(const PriorityQueue& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

PriorityQueue& PriorityQueue::operator =(const PriorityQueue& src)
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

size_t PriorityQueue::HashCode() const
{
   PIMPL
   return (size_t)p;
}

bool PriorityQueue::Empty() const
{
   PIMPL
   return p->Empty();
}

size_t PriorityQueue::Count() const
{
   PIMPL
   return p->Count();
}

void PriorityQueue::Enqueue(ObjectRef object, int priority)
{
   PIMPL
   p->Enqueue(object, priority);
}

ObjectRef PriorityQueue::Dequeue()
{
   PIMPL
   return p->Dequeue();
}

void PriorityQueue::Clear()
{
   PIMPL
   p->Clear();
}
