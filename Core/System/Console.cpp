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

#include <System/Console.h>
#include <System/Exception.h>
#include <System/Threading/Synchro.h>

#include <iostream>

namespace System
{
   namespace Private
   {
      Threading::Synchro SyncRoot()
      {
         static Threading::Synchro syncRoot;
         return syncRoot;
      }

      std::string& StringInternalField(const System::String& string);
   }
}

using namespace System;
using namespace System::Threading;

void Console::Write(const String& format, const Collections::StringCollection& args)
{
   Locker lock(Private::SyncRoot());
   std::cout << Private::StringInternalField(format);
   // TODO: manage format string and args
   for(size_t i=0; i<args.Count(); i++)
      std::cout << Private::StringInternalField(args[i]);
}

void Console::WriteLine(const String& format, const Collections::StringCollection& args)
{
   Locker lock(Private::SyncRoot());
   std::cout << Private::StringInternalField(format);
   // TODO: manage format string and args
   for(size_t i=0; i<args.Count(); i++)
      std::cout << Private::StringInternalField(args[i]);
   std::cout << std::endl;
}
