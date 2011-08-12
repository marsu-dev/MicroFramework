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

#include <System/Threading/ResetEvent.h>

#include <boost/thread/mutex.hpp>
#include <boost/thread/condition_variable.hpp>

typedef boost::mutex::scoped_lock lock_t;

namespace System
{
   namespace Threading
   {
      namespace Private
      {
         class ResetEvent : public System::Pimpl
         {
         public:
            ResetEvent()
               : referenceCount(0)
            {}

            size_t ReferenceCount() const;

            void Reset()
            {
               boost::lock_guard<boost::mutex> lock(mut);
               data_ready=true;
            }

            void WaitOne()
            {
               boost::unique_lock<boost::mutex> lock(mut);
               while(!data_ready)
               {
                  cond.wait(lock);
               }
            }

            void NotifyOne()
            {
               {
                  boost::lock_guard<boost::mutex> lock(mut);
                  data_ready=true;
               }
               cond.notify_one();
            }

            int referenceCount;
            boost::condition_variable cond;
            boost::mutex mut;
            bool data_ready;
         };
      }
   }
}

using namespace System::Threading;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::ResetEvent* p(static_cast<Private::ResetEvent*>(this->p));

size_t Private::ResetEvent::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

ResetEvent::ResetEvent()
  : p(new Private::ResetEvent)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

ResetEvent::~ResetEvent()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

ResetEvent::ResetEvent(const ResetEvent& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

ResetEvent& ResetEvent::operator =(const ResetEvent& src)
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

void ResetEvent::Reset()
{
   PIMPL
   p->Reset();
}

void ResetEvent::WaitOne()
{
   PIMPL
   p->WaitOne();
}

void ResetEvent::NotifyOne()
{
   PIMPL
   p->NotifyOne();
}

size_t ResetEvent::HashCode() const
{
   PIMPL
   return (size_t)p;
}
