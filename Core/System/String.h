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

#include <string>
#include <System/Object.h>

namespace System
{
   class String : public Object
   {
   public:
      String();
      String(const std::string& string);
      virtual ~String();
      String(const String& src);
      String& operator =(const String& src);

      bool operator ==(const String comp) const;

      static const String Empty();

      operator std::string() const;
      size_t HashCode() const;

      String Contat(String right) const ;
      static String Contat(String left, String right);

      // TODO
      // String Trim(String chars) const;
      // String TrimLeft(String chars) const;
      // String TrimRight(String chars) const;
      // StringCollection Split(String chars) const;
      // String Replace(String oldStr, String newStr) const;
      // String Remove(String str) const { return Replace(str, Emtpy()); }

   private:
      Pimpl* p;
   };

   static String operator +(String left, String right);
}
