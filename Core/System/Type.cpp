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

#include <System/Type.h>
#include <System/Threading/Locker.h>
#include <System/Threading/Mutex.h>

#include <map>
#include <string>

using namespace System;

#if defined(_MSC_VER)
   #define RealName(name) name
#else
   // http://stackoverflow.com/questions/789402/typeid-returns-extra-characters-in-g
   #include <cxxabi.h>
   static std::string RealName(const std::string& name)
   {
      std::string ret;
      int status;
      char *realname = abi::__cxa_demangle(name.c_str(), 0, 0, &status);
      ret = realname;
      free(realname);

      return ret;
   }
#endif

Type& Type::FromObject(const Object& object)
{
   static Threading::Mutex mutex;
   Threading::Locker lock(mutex);

   typedef std::map<size_t, System::Type> TypeMap;
   static TypeMap typeMap;

   const String typeName(RealName(typeid(object).name()));

   const size_t code(typeName.HashCode());
   if(typeMap.find(code)==typeMap.end())
      typeMap.insert(std::make_pair(code, Type(typeName)));

   return typeMap[code];
}

Type& Object::Type() const
{
   return Type::FromObject(*this);
}

std::string Type::ToString() const
{
   return name;
}

std::string Object::ToString() const
{
   const System::Type type(Type::FromObject(*this));

   return type.ToString();
}

static bool TypeEquals(const Type& op1, const Type& op2)
{
   return op1.Name()==op2.Name();
}

bool Type::operator==(const Type& type) const
{
   return TypeEquals(*this, type);
}

bool Type::operator!=(const Type& type) const
{
   return !TypeEquals(*this, type);
}
