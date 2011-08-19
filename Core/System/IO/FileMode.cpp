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

#include <System/IO/FileMode.h>

using namespace System;
using namespace System::IO;

const Enum FileMode::CreateNew(Type::Get<FileMode>(), String("CreateNew"));
const Enum FileMode::Create(Type::Get<FileMode>(), String("Create"));
const Enum FileMode::Open(Type::Get<FileMode>(), String("Open"));
const Enum FileMode::OpenOrCreate(Type::Get<FileMode>(), String("OpenOrCreate"));
const Enum FileMode::Truncate(Type::Get<FileMode>(), String("Truncate"));
const Enum FileMode::Append(Type::Get<FileMode>(), String("Append"));

EnumCollection FileMode::All()
{
   static const Enum enums[] = {
      CreateNew,
      Create,
      Open,
      OpenOrCreate,
      Truncate,
      Append
   };

   EnumCollection ret;
   for(size_t i=0; i<sizeof(enums)/sizeof(enums[0]); i++)
      ret.Add(enums[i]);
   
   return ret;
}

const Enum FileAccess::Read(Type::Get<FileAccess>(), String("Read"));
const Enum FileAccess::Write(Type::Get<FileAccess>(), String("Write"));
const Enum FileAccess::ReadWrite(Type::Get<FileAccess>(), String("ReadWrite"));

EnumCollection FileAccess::All()
{
   static const Enum enums[] = {
      Read,
      Write,
      ReadWrite
   };

   EnumCollection ret;
   for(size_t i=0; i<sizeof(enums)/sizeof(enums[0]); i++)
      ret.Add(enums[i]);
   
   return ret;
}
