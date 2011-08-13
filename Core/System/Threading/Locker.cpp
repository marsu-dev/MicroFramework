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

#include <System/Threading/Locker.h>

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

#include <boost/shared_ptr.hpp>
typedef boost::shared_ptr<lock_t> lock_ptr;

#include <cassert>

// avoid friend class use and private shared pimpl

namespace System
{
   namespace Threading
   {
      namespace Private
      {
         boost::mutex& MutexInternalField(const Threading::Mutex& mutex);

         class Locker : public System::Pimpl
         {
         public:
            Locker()
               : referenceCount(0)
            {}

            size_t ReferenceCount() const;

            void Lock(System::Threading::Mutex& mutex)
            {
               boost::mutex& mtx(MutexInternalField(mutex));
               locker.reset(new lock_t(mtx));
            }

            void Unlock()
            {
               locker.reset();
            }

            bool IsLocked() const { return locker.get()!=NULL; }

            int referenceCount;
            lock_ptr locker;
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
#define PIMPL Private::Locker* p(static_cast<Private::Locker*>(this->p));

size_t Private::Locker::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

Locker::Locker()
  : p(new Private::Locker)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Locker::~Locker()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

Locker::Locker(const Locker& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Locker& Locker::operator =(const Locker& src)
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

Locker::Locker(Mutex mutex)
  : p(new Private::Locker())
{
   {
      LOCK
      PIMPL
      p->referenceCount++;
   }
   PIMPL
   p->Lock(mutex);
}

size_t Locker::HashCode() const
{
   PIMPL
   return (size_t)p;
}

void Locker::Lock(Mutex mutex)
{
   PIMPL
   p->Lock(mutex);
}

void Locker::Unlock()
{
   PIMPL
   p->Unlock();
}

bool Locker::IsLocked() const
{
   PIMPL
   return p->IsLocked();
}
