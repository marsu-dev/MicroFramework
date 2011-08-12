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

#include <boost/signal.hpp>
#include <boost/bind.hpp>
#include <map>

namespace System
{
   namespace Events
   {
      namespace Detail
      {
         typedef boost::signals::connection Connection;

         template<class Observer, class Callback>
         class ObserverConnection : public Object
         {
         private:
            typedef typename std::map<const Observer*, Connection> ObserverConnectionMap;
            typedef typename std::pair<const Observer*, Connection> ObserverConnectionPair;
            typedef typename ObserverConnectionMap::iterator ObserverConnectionIterator;

            ObserverConnectionMap connections;
         public:

            bool HasConnection(const Observer& observer) const {
               return connections.find(&observer)!=connections.end();
            }

            Connection GetConnection(const Observer& observer) {
               if(!HasConnection(observer))
                  throw "Not registered connection";
               return connections.find(&observer)->second;
            }

            void Add(const Observer& observer, Connection connection) {
               if(HasConnection(observer))
                  return;
               connections.insert(ObserverConnectionPair(&observer, connection));
            }

            void Remove(const Observer& observer) {
               if(!HasConnection(observer))
                  return;
               connections.erase(connections.find(&observer));
            }
         };
      }

      template<typename Observer, typename Callback>
      class EventHandler : public boost::signal<Callback>, public Object
      {
      private:
         typedef Detail::ObserverConnection<Observer, Callback> ObserverConnection;
         ObserverConnection connections;

      public:
         bool Connect(Observer& observer) {
            if(connections.HasConnection(observer))
               return false;

            Detail::Connection connection = boost::signal<Callback>::connect(observer, observer);
            connections.Add(observer, connection);
            return true;
         }

         bool Disconnect(Observer& observer) {
            if(!connections.HasConnection(observer))
               return false;

            connections.GetConnection(observer).disconnect();
            connections.Remove(observer);
            return true;
         }

         EventHandler<Observer, Callback>& operator +=(Observer& observer) {
            Connect(observer);
            return *this;
         }

         EventHandler<Observer, Callback>& operator -=(Observer& observer) {
            Disconnect(observer);
            return *this;
         }
      };

      typedef Detail::Connection Connection;
      template<class H, class T, class F>
      static Connection Connect(H& h, T& t, F f) { return h.connect(boost::bind(f, &t, _1)); }

      template<typename Callback>
      class Event : public boost::signal<Callback>, public SimpleObject {};

      class Observer : public boost::signals::trackable, public SimpleObject {};
   }
}
