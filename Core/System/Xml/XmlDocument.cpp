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

#include <System/Xml/XmlDocument.h>
#include <System/Exception.h>
#include <istream>
#include <fstream>
#include <sstream>

#include <pugixml.hpp>

#include <boost/thread/mutex.hpp>
typedef boost::lock_guard<boost::mutex> lock_t;

namespace System
{
   namespace Xml
   {
      namespace Private
      {
         class XmlDocument : public System::Pimpl
         {
         public:
            XmlDocument()
               : referenceCount(0)
            {}

            size_t ReferenceCount() const;

            void LoadFile(const std::string& xmlFile)
            {
               std::ifstream stream;
               stream.open(xmlFile.c_str(), std::ios::in);
               if(!stream.is_open())
                  throw FileNotFoundException();
               Load(stream);
            }

            void LoadXml(const std::string& xml)
            {
               std::istringstream stream(xml);
               Load(stream);
            }

            void Load(std::istream& stream)
            {
               if(!doc.load(stream))
                  throw Exception();
            }

            std::string ToString() const
            {
               try
               {
                  std::ostringstream oss;
                  doc.save(oss);
                  oss << "\0";
                  return oss.str();
               }
               catch(std::exception&)
               {
                  throw Exception();
               }
            }

            int referenceCount;

            pugi::xml_document doc;
         };
      }
   }
}

using namespace System;
using namespace System::Xml;

static boost::mutex& m(){
   static boost::mutex mtx;
   return mtx;
}
#define LOCK lock_t l(m());
#define PIMPL Private::XmlDocument* p(static_cast<Private::XmlDocument*>(this->p));

size_t Private::XmlDocument::ReferenceCount() const
{
   LOCK
   return referenceCount;
}

XmlDocument::XmlDocument()
  : p(new Private::XmlDocument)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

XmlDocument::~XmlDocument()
{
   LOCK
   PIMPL
   p->referenceCount--;
   if(!p->referenceCount)
      delete p;
}

XmlDocument::XmlDocument(const XmlDocument& src)
  : p(src.p)
{
   LOCK
   PIMPL
   p->referenceCount++;
}

XmlDocument& XmlDocument::operator =(const XmlDocument& src)
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

size_t XmlDocument::HashCode() const
{
   PIMPL
   return (size_t)p;
}

void XmlDocument::LoadFile(String fileName)
{
   PIMPL
   p->LoadFile(fileName);
}

void XmlDocument::LoadXml(String xml)
{
   PIMPL
   p->LoadXml(xml);
}

std::string XmlDocument::ToString() const
{
   PIMPL
   return String(p->ToString());
}
