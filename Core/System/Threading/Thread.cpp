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

#include <System/Threading/Thread.h>
#include <System/Threading/Locker.h>

#include <boost/thread.hpp>
#include <boost/bind.hpp>

typedef boost::thread thread_t;
typedef boost::shared_ptr<thread_t> thread_ptr;

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

using namespace std;
namespace System
{
   namespace Threading
   {
      namespace Private
      {
         class Thread : public System::Pimpl
         {
         public:
            Thread()
               : referenceCount(0)
               , started(false)
               , autojoin(true)
            {}

            ~Thread()
            {
               if(started && autojoin)
                  Join();
            }

            size_t ReferenceCount() const;

            void Start(SyncRunner runner)
            {
               static Mutex mut;
               Locker lock(mut);

               // manage if thread is working (re-use Thread instance)
               if(thread && autojoin)
                  Join();
               else if(started)
                  thread.reset();

               thread.reset(new thread_t(boost::bind(&SyncRunner::Run, runner)));

               runner.Sync().WaitOne();
               started = true;
            }

            void Join()
            {
               assert(thread && "Invalid thread");
               if(!thread)
                  return;
               thread->join();
            }

            int referenceCount;
            bool started;
            bool autojoin;
            thread_ptr thread;
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
#define PIMPL Private::Thread* p(static_cast<Private::Thread*>(this->p));

size_t Private::Thread::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

Thread::Thread()
  : p(new Private::Thread)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Thread::~Thread()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

Thread::Thread(const Thread& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Thread& Thread::operator =(const Thread& src)
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

size_t Thread::HashCode() const
{
   PIMPL
   return (size_t)p;
}

void Thread::Sleep(size_t msecs)
{
   boost::this_thread::sleep(boost::posix_time::milliseconds(msecs));
}

void Thread::Start(SyncRunner runner)
{
   PIMPL
   p->Start(runner);
}

void Thread::Start(Runner runner)
{
   static ResetEvent sync;
   Start(SyncRunner(sync, runner));
}

void Thread::Join()
{
   PIMPL
   p->Join();
}

void Thread::Yield()
{
   boost::this_thread::yield();
}
