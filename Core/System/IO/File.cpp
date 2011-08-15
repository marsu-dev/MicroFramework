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

#include <System/IO/File.h>
#include <System/IO/Exception.h>
#include <boost/filesystem.hpp>

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

namespace System
{
   namespace IO
   {
      namespace Private
      {
         class File : public System::Pimpl
         {
         public:
            File()
               : referenceCount(0)
            {}

            size_t ReferenceCount() const;

            int referenceCount;
         };
      }
   }
}

using namespace System::IO;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::File* p(static_cast<Private::File*>(this->p));

size_t Private::File::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

File::File()
  : p(new Private::File)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

File::~File()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

File::File(const File& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

File& File::operator =(const File& src)
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

size_t File::HashCode() const
{
   PIMPL
   return (size_t)p;
}

bool File::Exists(String fileName)
{
   const std::string& filename(fileName);
   return boost::filesystem::exists(filename) && boost::filesystem::is_regular_file(filename);
}

int64_t File::Length(String fileName)
{
   if(!File::Exists(fileName))
      throw FileException();
   const std::string& filename(fileName);
   return (int64_t)boost::filesystem::file_size(filename);
}
