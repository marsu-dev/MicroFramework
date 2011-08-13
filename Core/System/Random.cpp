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

#include <System/Random.h>
#include <System/Threading/Synchro.h>

#include <boost/random.hpp>
#include <cmath>
#include <limits>

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

namespace System
{
   namespace Private
   {
      byte_array& BufferInternalField(const System::Buffer& buffer);

      class Random : public System::Pimpl
      {
      public:
         Random()
            : referenceCount(0)
            , syncRoot()
            , rng()
            , randInt(std::numeric_limits<int>::min(), std::numeric_limits<int>::max())
            , randDouble()
            , randBytes(0x00, 0xFF)
         {}

         Random(int min, int max)
            : referenceCount(0)
            , syncRoot()
            , rng()
            , randInt(std::min(min, max), std::max(min,max))
            , randDouble()
            , randBytes(0x00, 0xFF)
         {}

         size_t ReferenceCount() const;

         int Next()
         {
            Threading::Locker lock(syncRoot);
            return randInt(rng);
         }

         double NextDouble()
         {
            Threading::Locker lock(syncRoot);
            return randDouble(rng);
         }
         void NextBytes(Buffer& buffer)
         {
            Threading::Locker lock(syncRoot);
            byte_array& bytes(BufferInternalField(buffer));
            byte_array::iterator it(bytes.begin());
            while(it!=bytes.end())
               *it++ = (byte)randBytes(rng);
         }

         int referenceCount;
         Threading::Synchro syncRoot;

         boost::mt19937 rng;
         boost::uniform_int<> randInt;
         boost::uniform_01<> randDouble;
         boost::uniform_int<> randBytes;
      };
   }
}

using namespace System;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::Random* p(static_cast<Private::Random*>(this->p));

size_t Private::Random::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

Random::Random()
  : p(new Private::Random)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Random::~Random()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

Random::Random(int min, int max)
   : p(new Private::Random(min, max))
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Random::Random(const Random& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

Random& Random::operator =(const Random& src)
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

size_t Random::HashCode() const
{
   PIMPL
   return (size_t)p;
}

int Random::Next() const
{
   PIMPL
   return p->Next();
}

double Random::NextDouble() const
{
   PIMPL
   return p->NextDouble();
}

void Random::NextBytes(Buffer buffer) const
{
   PIMPL
   p->NextBytes(buffer);
}

System::Buffer Random::NextBytes(size_t size) const
{
   System::Buffer buffer;
   buffer.Resize(size);
   NextBytes(buffer);
   return buffer;
}
