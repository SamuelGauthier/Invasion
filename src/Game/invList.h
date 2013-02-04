#ifndef INVLIST_H
#define INVLIST_H

#include <stdlib.h>
#include <stdio.h>

template <class TYPE>
class invList
{
	struct invElement
	{
		TYPE* pElem;
		invElement* pNext;
	};

	invElement* pHead;
	invElement* pCurrent;
	invElement* pPrev;
	invElement* pTail;
public:
	inline invList();
	inline ~invList();

	inline void add(TYPE* t);
	inline void addBack(TYPE* t);
	inline void insert(TYPE* t);
	inline void next();
	inline TYPE* current() const;
	inline void remove();
	inline void erase();
	inline bool seek(TYPE* t);
	inline bool isEmpty() const;
	inline bool isEnd() const;
	inline void rewind();
	inline TYPE* getIndex(int index);
	inline TYPE* getHead();
	inline TYPE* getTail();
	void reverse();

	inline void clean();
	int count;
};

//------------------------------------------------
// invList
//------------------------------------------------
template <class TYPE>
inline invList<TYPE>::invList()
{
	pHead = NULL;
	pCurrent = NULL;
	pPrev = NULL;
	pTail = NULL;
	count = 0;
}

template <class TYPE>
inline invList<TYPE>::~invList()
{
	clean();
}

template <class TYPE>
inline bool invList<TYPE>::isEmpty() const
{
	return (pHead == NULL);
}

template <class TYPE>
inline bool invList<TYPE>::isEnd() const
{
	return (pCurrent == NULL);
}

template <class TYPE>
inline void invList<TYPE>::add(TYPE* t)
{
	invElement* e = new invElement;
	e->pElem = t;
	
	if(!pHead)
	{
		pHead = e;
		pTail = e;
		e->pNext = NULL;
	}

	else
	{
		if(pCurrent == pHead)
			pPrev = e;

		e->pNext = pHead;
		pHead = e;

	}
	count++;
}

template <class TYPE>
inline void invList<TYPE>::addBack(TYPE* t)
{
	invElement* e = new invElement;
	e->pElem = t;

	if(!pTail)
	{
		pTail = e;
		pHead = e;
	}

	else
	{
		pTail->pNext = e;
		pTail = e;
	}

	e->pNext = NULL;

	if(!pCurrent)
		pCurrent = e;
	count++;
}

template <class TYPE>
inline void invList<TYPE>::insert(TYPE* t)
{
	invElement* e = new invElement;
	e->pElem = t;
	e->pNext = NULL;

	if(pPrev)
		pPrev->pNext = e;
	else
		pHead = e;

	if(pCurrent)
		e->pNext = pCurrent;
	else
		pTail = e;

	pCurrent = e;
	count++;
}

template <class TYPE>
inline void invList<TYPE>::next()
{
	pPrev = pCurrent;
	pCurrent = pCurrent->pNext;
}

template <class TYPE>
inline TYPE* invList<TYPE>::current() const
{
	return pCurrent->pElem;
}

template <class TYPE>
inline void invList<TYPE>::remove()
{
	// for the Head
	if(pPrev)
		pPrev->pNext = pCurrent->pNext;
	else
		pHead = pCurrent->pNext;
	
	// for the Tail
	if(!pCurrent->pNext)
		pTail = pPrev;	

	invElement* tmp = pCurrent;
	pCurrent = pCurrent->pNext;
	
	delete tmp;
	count--;
}

template <class TYPE>
inline void invList<TYPE>::erase()
{
	if(pCurrent)
	{
		// for the Head
		if(pPrev)
			pPrev->pNext = pCurrent->pNext;
		else
			pHead = pCurrent->pNext;
		
		// for the Tail
		if(!pCurrent->pNext)
			pTail = pPrev;

		invElement* tmp = pCurrent;
		pCurrent = pCurrent->pNext;

		delete tmp->pElem;
		delete tmp;
		count--;
	}
}

template <class TYPE>
inline void invList<TYPE>::rewind()
{
	pCurrent = pHead;
	pPrev = NULL;
}

template <class TYPE>
inline bool invList<TYPE>::seek(TYPE* t)
{
	rewind();
	while(!isEnd()) {
		if(current() == t){ return true; }
		next();
	}
	return false;
}

template <class TYPE>
inline void invList<TYPE>::clean()
{
	rewind();
	while(!isEnd()) {
		erase();
	}
	
	pHead = NULL;
}

template <class TYPE>
inline TYPE* invList<TYPE>::getIndex(int index)
{
	rewind();
	while(index-- && !isEnd())
		next();
	return current();
}

template <class TYPE>
inline TYPE* invList<TYPE>::getHead()
{
	rewind();
	return current();
}

template <class TYPE>
inline TYPE* invList<TYPE>::getTail()
{
	return pTail;
}

template <class TYPE>
void invList<TYPE>::reverse()
{
	rewind();
	int original_count = count;
	for(int i=0;i<original_count;i++)
	{
		add(current());
		erase();
	}

	invElement* tmp = pTail;
	pTail = pHead;
	pHead = tmp;
}

#endif
