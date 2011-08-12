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

#include <System/Object.h>

namespace System
{
   class ObjectRef : public Object
   {
   public:
      template<class T>
      static ObjectRef Create() { return ObjectRef(new T); }
      template<class T>
      static ObjectRef Create(const T& t) { return ObjectRef(new T(t)); }

      ObjectRef(Object* object);
      virtual ~ObjectRef();
      ObjectRef(const ObjectRef& src);
      ObjectRef& operator =(const ObjectRef& src);

      void Reset();

      virtual size_t HashCode() const;

      operator bool() const;

      Object& Get(); 
      const Object& Get() const;

      operator Object& () { return Get(); } 
      operator const Object& () const { return Get(); }

      template<class T>
      T& Get() { return dynamic_cast<T&>(Get()); }

      template<class T>
      const T& Get() const { return dynamic_cast<T&>(Get()); }

      template<class T>
      bool IsTypeOf() const { return dynamic_cast<const T*>(&Get())!=NULL; }

   private:
      Pimpl* p;
   };
}
