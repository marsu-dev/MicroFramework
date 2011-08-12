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

#include <iostream>
#include <sstream>
#include <list>

#include <System.h>
#include <System/Data.h>
#include <System/Threading.h>
#include <System/Collections.h>
#include <System/Xml/XmlDocument.h>

#include "MyWorker.h"

System::ObjectValue<int> intValue;

#include <System/Collections/BindHelper.h>

#if defined(_MSC_VER) && defined(_DEBUG) && 0
   #define VLD_MAX_DATA_DUMP 256
   #include <vld.h>
#endif

using namespace System;
using namespace System::Threading;

static Mutex mutex()
{
   static Mutex mtx;
   return mtx;
}

class WorkerObserver : public System::Events::Observer
{
public:
   void OnWorkFinished(MyWorker& myWorker)
   {
      std::cerr << "Worker Done:" << std::endl;
      std::cerr << (std::string)(myWorker.result) << std::endl;
   }

   void operator()(MyWorker& myWorker) { OnWorkFinished(myWorker); }

};

static int TypeTest();
static int CollectionTest();
static int SrtingTest();
static int XmlTest();
static int ThreadTest();
#include <set>
#include <list>
#include <map>
#include <vector>
#include <algorithm>

int main(int argc, char* argv[])
{
   try
   {
      TypeTest();
      CollectionTest();
      SrtingTest();
      XmlTest();
      ThreadTest();
   }
   catch(std::exception& e)
   {
      std::cout << e.what() << std::endl;
   }
   return 0;
}

typedef std::vector<String> StringCollection;
typedef std::vector<NameValue> NameValueCollection;

static int TypeTest()
{
   std::cout << "Type Test" << std::endl;
   Console::WriteLine("Hello");
   Console::WriteLine(String("Hello, "), String("World!"));
   Console::WriteLine(String().Type());
   Console::WriteLine(Type::Get<String>());
   return 0;
}

static int CollectionTest()
{
   std::cout << "Collection Test" << std::endl;
   System::Collections::StringCollection strings;
   strings.Add(String("Hello, World!"));

   StringCollection ss(strings.ToArray());
   System::Collections::NameValueCollection keyValue;
   keyValue.Add(NameValue(String("Hello, "), String("World!")));

   NameValueCollection kvs(keyValue.ToArray());
   std::cout << (std::string)kvs[0].Name << (std::string)kvs[0].Value << std::endl;

   System::Data::StructuredData structuredData;

   return 0;
}

static int SrtingTest()
{
   std::cout << "String Test" << std::endl;
   std::cout << String::Empty().HashCode() << std::endl;
   std::cout << String::Empty().HashCode() << std::endl;

   //std::list<std::string> strings;
   std::list<String> strings;
   for(int i=0; i<(4<<10); i++)
   {
      std::ostringstream oss;
      oss << i;
      //strings.push_back(oss.str());
      strings.push_back(String(oss.str()));

      if(!(i%(1<<16)))
         strings.clear();

      //std::cout << strings.back().HashCode() << std::endl;
   }
   return 0;
}

static int XmlTest()
{
   Xml::XmlDocument doc;

   doc.LoadFile(String("../pugixml/tests/data/small.xml"));
   Console::WriteLine(doc.ToString());

   return 0;
}

typedef System::Collections::Generic::ListDelegate<MyWorker> WorkerListDelegate;

class WorkerDelegate : public WorkerListDelegate
{
public:
   void operator()(MyWorker& worker)
   {
      Print(worker);
   }

   void OnWorker(MyWorker& worker)
   {
      Print(worker);
   }
   static void Print(MyWorker& worker)
   {
      std::cout << ".";
   }
};

typedef System::Collections::Generic::List<MyWorker> MyWorkerCollection;

class WorkerIterator
{
public:
   WorkerIterator(MyWorkerCollection& workers)
      : workers(workers)
   {}

   void ForEach()
   {
      workers.ForEach(*this);
   }

   void operator()(MyWorker& worker)
   {
      std::cout << "*";
   }

   static void ForEach(System::Collections::Generic::List<MyWorker>& workers)
   {
      WorkerIterator it(workers);
      it.ForEach();
   }

private:
   MyWorkerCollection workers;
};


typedef System::Collections::ForeachIterator<MyWorkerCollection> MyWorkerIterator;

static int ThreadTest()
{
   MyWorker w;
   std::cout << "Thread Test" << std::endl;
   MyWorkerCollection workerList;
   for(int i=0; i<(1<<3); i++)
      workerList.Add(MyWorker());
   w = workerList.At(0);
   workerList.AddRange(workerList);
   workerList.Reverse();

   WorkerDelegate workerDelegate;
   std::cerr << "Delegate instance: " << std::endl;
   workerList.ForEach(workerDelegate);
   std::cerr << std::endl;

   std::cerr << "Delegate static callback: " << std::endl;
   workerList.ForEach(WorkerDelegate::Print);
   std::cerr << std::endl;

   std::cerr << "Delegate functor (): " << std::endl;
   workerList.ForEach(workerDelegate);
   std::cerr << std::endl;

   std::cerr << "Delegate bind: " << std::endl;
   workerList.ForEach(boost::bind(&WorkerDelegate::OnWorker, workerDelegate, _1));
   std::cerr << std::endl;

   std::cerr << "Delegate bind wrapper: " << std::endl;
   System::Collections::ForEachWrap(workerList, workerDelegate, &WorkerDelegate::OnWorker);
   std::cerr << std::endl;

   std::cerr << "Foreach iterator: " << std::endl;
   MyWorkerIterator::ForEach(workerList, workerDelegate, &WorkerDelegate::OnWorker);
   MyWorkerIterator it(workerList);
   it.ForEach(workerDelegate, &WorkerDelegate::OnWorker);
   std::cerr << std::endl;

   std::cerr << "Worker iterator: " << std::endl;
   WorkerIterator::ForEach(workerList);
   std::cerr << std::endl;

   workerList.Clear();
   std::cerr << std::endl;

   System::Collections::Generic::Stack<MyWorker> workerStack;
   for(int i=0; i<(1<<16); i++)
   {
      workerStack.Push(MyWorker());
   }
   while(!workerStack.Empty())
      w = workerStack.Pop();

   std::cout << "Queue Test" << std::endl;
   System::Collections::Generic::Queue<MyWorker> workerQueue;
   for(int i=0; i<(1<<10); i++)
   {
      workerQueue.Enqueue(MyWorker());
   }
   while(!workerQueue.Empty())
      w = workerQueue.Dequeue();

   std::cout << "PriorityQueue Test" << std::endl;
   System::Collections::Generic::PriorityQueue<String> priorityQueue;
   for(int i=0; i<10; i++)
   {
      String str;
      {
         std::ostringstream oss;
         oss << i;
         str = String(oss.str());
      }
      priorityQueue.Enqueue(str, i%2);
   }
   while(!priorityQueue.Empty())
   {
      String s(priorityQueue.Dequeue());
      std::cout << (std::string)s << std::endl;
   }

   System::Collections::Generic::Set<MyWorker> workerSet;
   for(int i=0; i<(1<<16); i++)
   {
      workerSet.Add(MyWorker());
   }
   std::cout << "Set count: " << workerSet.ToList().Count() << std::endl;
   workerSet.Clear();

   System::Collections::Generic::Dictionary<String, MyWorker> workerDic;
   for(int i=0; i<(1<<16); i++)
   {
      String str;
      {
         std::ostringstream oss;
         oss << i;
         str = String(oss.str());
      }
      workerDic.Add(str, MyWorker());
      if(workerDic.Contains(str))
         str = workerDic[str].result;
   }
   std::cout << "Dictionary count: " << workerDic.AllKeys().Count() << std::endl;
   workerDic.Clear();

   int cpt = 10;
   while(cpt--)
   {
      WorkerObserver workObserver;
      Workers workers;
      for( int i=0;i<4; i++)
      {
         // Automatic thread start
         //workers.Add<MyWorker>(); continue;

         // Manually Create worker
         RunnablePtr runable(RunnablePtr::Create<MyWorker>());

         MyWorker& worker(runable.Get<MyWorker>());
         worker.ConnectWorkFinished(workObserver);
         workers.Add(runable);
      }
      workers.Clear();
      std::cerr << "#";
   }

   return 0;
}
