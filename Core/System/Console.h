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
#include <System/String.h>
#include <System/Collections/StringCollection.h>

namespace System
{
   class Console : public SimpleObject
   {
   public:
      static void Write(const std::string& string) { Console::Write(String(string)); }
      static void WriteLine(const std::string& string) { Console::WriteLine(String(string)); }

      static void Write(const String& string) { WriteLine(string, Collections::StringCollection()); }
      static void WriteLine(const String& string) { WriteLine(string, Collections::StringCollection()); }

      static void Write(const Object& object) { Console::Write(object.ToString()); }
      static void WriteLine(const Object& object) { Console::WriteLine(object.ToString()); }

      static void Write(const Object& format, const Object& arg1)
      {
         Collections::StringCollection args;
         args.Add(arg1.ToString());
         Console::Write(String(format.ToString()), args);
      }
      static void Write(const Object& format, const Object& arg1, const Object& arg2)
      {
         Collections::StringCollection args;
         args.Add(arg1.ToString());
         args.Add(arg2.ToString());
         Console::Write(String(format.ToString()), args);
      }
      static void Write(const Object& format, const Object& arg1, const Object& arg2, const Object& arg3)
      {
         Collections::StringCollection args;
         args.Add(arg1.ToString());
         args.Add(arg2.ToString());
         args.Add(arg3.ToString());
         Console::Write(String(format.ToString()), args);
      }

      static void WriteLine(const Object& format, const Object& arg1)
      {
         Collections::StringCollection args;
         args.Add(arg1.ToString());
         Console::WriteLine(String(format.ToString()), args);
      }
      static void WriteLine(const Object& format, const Object& arg1, const Object& arg2)
      {
         Collections::StringCollection args;
         args.Add(arg1.ToString());
         args.Add(arg2.ToString());
         Console::WriteLine(String(format.ToString()), args);
      }
      static void WriteLine(const Object& format, const Object& arg1, const Object& arg2, const Object& arg3)
      {
         Collections::StringCollection args;
         args.Add(arg1.ToString());
         args.Add(arg2.ToString());
         args.Add(arg3.ToString());
         Console::WriteLine(String(format.ToString()), args);
      }

      static void Write(const String& format, const Collections::StringCollection& args);
      static void WriteLine(const String& format, const Collections::StringCollection& args);
   };
}
