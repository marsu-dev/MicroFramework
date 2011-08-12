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

#include <System/Threading/Runner.h>
#include <System/Threading/Mutex.h>
#include <System/Threading/Locker.h>

#include <cassert>
#include <iostream>

using namespace System::Threading;

Runner::Runner(RunnablePtr runnable)
   : runnable(runnable)
{
   assert(runnable && "runnable is NULL");
   if(!runnable)
      throw "Null pointer exception";
}
static Mutex mutInterlock()
{
   static Mutex mtx;
   return mtx;
}

static int Increment(int& value)
{
   Locker lock(mutInterlock());
   value++;
   return value;
}

static int Decrement(int& value)
{
   Locker lock(mutInterlock());
   value--;
   return value;
}

static void Dots(int count, char c)
{
   if(count)
      return;
   static Mutex mutDisplay;
   Locker lock(mutDisplay);

   for( int i=0; i<count; i++)
      std::cerr << c;
   std::cerr << count << std::endl;
}

void Runner::Run()
{
   assert(runnable && "invalid runnable");
   if(!runnable)
      return;

   static int cumul = 0;
   int id = Increment(cumul);

   Dots(id, '.');
   {
      IRunnable& r(runnable);
      r.Run();
   }
   Dots(id, '*');
   Decrement(cumul);
}
