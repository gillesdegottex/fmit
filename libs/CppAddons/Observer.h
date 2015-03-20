// Copyright 2003 "Gilles Degottex"

// This file is part of "CppAddons"

// "CppAddons" is free software; you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation; either version 2.1 of the License, or
// (at your option) any later version.
// 
// "CppAddons" is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA


#ifndef _Observer_h_
#define _Observer_h_

#include <typeinfo>
#include <list>
#include <algorithm>	// for: find
using namespace std;

template<class Listener> class Talker;

template<class TListener>
class Listener
{
 	friend class Talker<TListener>;

	bool m_isListenning;

protected:
	void todo(const string& fn){cerr << typeid(this).name() << "::" << fn << " not yet implemented" << endl;}

public:
	Listener() : m_isListenning(false) {}

	bool isListenning()	{return m_isListenning;}
};

template<class TListener, class TypeEvent>
class FireFun
{
	void (TListener::*m_pfOccured)(TypeEvent*);

	TypeEvent* m_evt;

public:
	explicit FireFun(void (TListener::*pfOccured)(TypeEvent*), TypeEvent* evt)
		: m_pfOccured(pfOccured), m_evt(evt)
	{}

	void operator()(TListener* l) const {(l->*m_pfOccured)(m_evt);}
};

template<class SourceType>
class EventObject
{
	SourceType* m_obj;

public:
	EventObject(SourceType* source) : m_obj(source)	{}

	SourceType* getSource() const {return m_obj;}
};

template<class Listener>
class Talker
{
public:
	typedef Listener TypeListener;
	typedef list<TypeListener*> ListenersList;
	typedef typename ListenersList::iterator ListenersIterator;

private:
	ListenersList m_removed;

protected:

	ListenersList m_listeners;

	bool m_firing;
	void removeRemoved()
	{
		while(!m_removed.empty())
		{
			m_removed.front()->m_isListenning = false;
			m_listeners.remove(m_removed.front());
			m_removed.pop_front();
		}
	}

	#define MFireEventL(s, L) {Talker<L>::m_firing=true;for(Talker<L>::ListenersList::iterator _listenerIterator_=(Talker<L>::m_listeners).begin(); _listenerIterator_!=(Talker<L>::m_listeners).end(); ++_listenerIterator_) (*_listenerIterator_)->s;Talker<L>::m_firing=false;Talker<L>::removeRemoved();}
	#define MFireEvent(s) MFireEventL(s, TypeListener)

	template<typename TypeEvent>
	void fireEvent(void (Listener::*fOccured)(TypeEvent*), TypeEvent* evt)
	{
		m_firing=true;
		for_each(	(Talker<Listener>::m_listeners).begin(),
					(Talker<Listener>::m_listeners).end(),
					FireFun<Listener, TypeEvent>(fOccured, evt));
		m_firing=false;
		removeRemoved();
	}

public:
	Talker() : m_firing(false) {}

	bool hasListeners(){return !m_listeners.empty();}
	bool hasListener(Listener* l)
	{
		return find(m_listeners.begin(), m_listeners.end(), l)!=m_listeners.end();
	}
	void addListener(Listener* l)
	{
		m_listeners.push_back(l);	// discutable: back/front
		
		l->m_isListenning = true;
	}
	void removeListener(Listener* l)
	{
		if(m_firing)	m_removed.push_back(l);
		else
		{
			m_listeners.remove(l);
			l->m_isListenning = false;
		}
	}
	void toggleListener(Listener* l)
	{
		if(hasListener(l))	addListener(l);
		else			removeListener(l);
	}

	list<Listener*>& getListeners(){return m_listeners;}
};

#endif
