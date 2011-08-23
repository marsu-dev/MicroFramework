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

#include <System/IO/FileStream.h>
#include <System/IO/Exception.h>
#include <System/IO/File.h>
#include <System/Threading/Synchro.h>

#include <fstream>

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

namespace System
{
   namespace Private
   {
      byte_array& BufferInternalField(const System::Buffer& buffer);
      Threading::Synchro& BufferInternalLock(const System::Buffer& buffer);
   }

   namespace IO
   {
      namespace Private
      {
         class FileStream : public System::Pimpl
         {
         public:
            FileStream()
               : referenceCount(0)
               , length(0)
            {}

            size_t ReferenceCount() const;

            void Open(const std::string& fileName, const OpenMode& openMode)
            {
               if(IsOpen())
                  throw FileOpenException();

               if(!File::Exists(String(fileName)) && (openMode==OpenMode::Read))
                  throw FileOpenException();
               if(File::Exists(String(fileName)) && (openMode==OpenMode::Write))
                  throw FileOpenException();

               Threading::Locker lock(syncRoot);

               length = File::Length(String(fileName));

               this->openMode = openMode;
               int mode(std::ios::binary);
               if(openMode==OpenMode::Read) mode |= std::ios::in;
               if(openMode==OpenMode::Write) mode |= std::ios::out;

               fileStream.open(fileName.c_str(), mode);
            }

            void Close()
            {
               if(!IsOpen())
                  return;

               fileStream.close();
            }

            void Flush()
            {
               if(!IsOpen())
                  return;

               fileStream.flush();
            }

            bool IsOpen() const
            {
               return fileStream.is_open();
            }

            size_t Length() const
            {
               return (size_t)length;
            }

            bool CanRead() const
            {
               return IsOpen() && openMode==OpenMode::Read;
            }

            bool CanWrite() const
            {
               return IsOpen() && openMode==OpenMode::Write;
            }

            size_t Read(Buffer buffer, size_t offset, size_t count)
            {
               if(!CanRead())
                  throw FileReadException();

               byte_array& bytes(System::Private::BufferInternalField(buffer));
               if(offset+count>bytes.size())
                  throw FileReadException();

               fileStream.read((char*)&bytes[offset], count);
               return (size_t)fileStream.gcount();
            }

            size_t Write(Buffer buffer, size_t offset, size_t count)
            {
               if(!CanWrite())
                  throw FileWriteException();

               byte_array& bytes(System::Private::BufferInternalField(buffer));
               if(offset+count>bytes.size())
                  throw FileReadException();

               const std::ios::streampos start(fileStream.tellp());
               fileStream.write((char*)&bytes[offset], count);
               return (size_t)(fileStream.tellp()-start);
            }

            void SeekRead(int64_t position)
            {
               if(!CanRead())
                  throw FileReadException();

               fileStream.seekg(position);
               if(!fileStream.good())
                  throw FileReadException();
            }

            void SeekWrite(int64_t position)
            {
               if(!CanWrite())
                  throw FileWriteException();

               fileStream.seekp(position);
               if(!fileStream.good())
                  throw FileWriteException();
            }

            int referenceCount;
            Threading::Synchro syncRoot;
            OpenMode openMode;
            std::fstream fileStream;
            int64_t length;
         };
      }
   }
}

using namespace System;
using namespace System::IO;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::FileStream* p(static_cast<Private::FileStream*>(this->p));

size_t System::IO::Private::FileStream::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

FileStream::FileStream()
  : p(new Private::FileStream)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

FileStream::~FileStream()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

FileStream::FileStream(const FileStream& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

FileStream& FileStream::operator =(const FileStream& src)
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

size_t FileStream::HashCode() const
{
   PIMPL
   return (size_t)p;
}

void FileStream::Open(String fileName, OpenMode openMode)
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   p->Open(fileName, openMode);
}

void FileStream::Close()
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   p->Close();
}

void FileStream::Flush()
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   p->Flush();
}

bool FileStream::IsOpen() const
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   return p->IsOpen();
}

int64_t FileStream::Length() const
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   return p->Length();
}

bool FileStream::CanRead() const
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   return p->CanRead();
}

bool FileStream::CanWrite() const
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   return p->CanWrite();
}

size_t FileStream::Read(Buffer buffer, size_t offset, size_t count)
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   Threading::Locker lockBuffer(System::Private::BufferInternalLock(buffer));

   try{
      return p->Read(buffer, offset, count);
   }
   catch(std::exception&){
      throw IOException();
   }
}

size_t FileStream::Write(Buffer buffer, size_t offset, size_t count)
{
   PIMPL
   Threading::Locker lock(p->syncRoot);
   Threading::Locker lockBuffer(System::Private::BufferInternalLock(buffer));

   try{
      return p->Write(buffer, offset, count);
   }
   catch(std::exception&){
      throw IOException();
   }
}

void FileStream::SeekRead(int64_t position)
{
   PIMPL
   Threading::Locker lock(p->syncRoot);

   try{
      return p->SeekRead(position);
   }
   catch(std::exception&){
      throw IOException();
   }
}

void FileStream::SeekWrite(int64_t position)
{
   PIMPL
   Threading::Locker lock(p->syncRoot);

   try{
      return p->SeekWrite(position);
   }
   catch(std::exception&){
      throw IOException();
   }
}
