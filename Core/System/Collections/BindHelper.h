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
#include <boost/bind.hpp>

namespace System
{
   namespace Collections
   {
      template<class C, class I, class F>
      static void ForEachWrap(C& c, I& i, F f) { c.ForEach(boost::bind(f, i, _1)); }

      template<class C>
      class ForeachIterator : public SimpleObject
      {
      public:
         ForeachIterator(C& c) : c(c) {}

         template<class I, class F>
         void ForEach(I& i, F f)
         {
            ForEachWrap(c, i, f);
         }

         template<class I, class F>
         static void ForEach(C& c, I& i, F f)
         {
            ForeachIterator it(c);
            it.ForEach(i, f);
         }

      private:
         C c;
      };
   }
}
