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

#include <System/SimpleObject.h>
#include <System/ObjectRef.h>

namespace System
{
   namespace Threading
   {
      class IRunnable : public SimpleObject
      {
      public:
         virtual void Run() = 0;
      };

      class RunnablePtr : public SimpleObject
      {
      public:
         template<class T>
         static RunnablePtr Create() { return RunnablePtr(new T); }

         operator bool () const { return runnable; }
         operator IRunnable&() { return Get<IRunnable>(); } 
         
         template<class T>
         T& Get() {return runnable.Get<T>(); }

      protected:
         RunnablePtr(IRunnable* runnable)
            : runnable(runnable)
         {}

      private:
         ObjectRef runnable;
      };
   }
}
