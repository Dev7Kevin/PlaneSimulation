//
// ArrayList.h : DXUtil 의 ArrayList 클래스 헤더 파일.
//
//
//////////////////////////////////////////////////////////////////////////////

#pragma once

#include "windows.h"

//-----------------------------------------------------------------------------
// Name: ArrayListType
// Desc: Indicates how data should be stored in a CArrayList
//-----------------------------------------------------------------------------
enum ArrayListType
{
    AL_VALUE,       // entry data is copied into the list
    AL_REFERENCE,   // entry pointers are copied into the list
};


 
//-----------------------------------------------------------------------------
// Name: CArrayList
// Desc: A growable array
//-----------------------------------------------------------------------------
class CArrayList
{
protected:
    ArrayListType m_ArrayListType;
    void* m_pData;
    UINT m_BytesPerEntry;
    UINT m_NumEntries;
    UINT m_NumEntriesAllocated;

public:
    CArrayList( ArrayListType Type, UINT BytesPerEntry = 0 );
    ~CArrayList( void );
    HRESULT Add( void* pEntry );
    void Remove( UINT Entry );
    void* GetPtr( UINT Entry );
    UINT Count( void ) { return m_NumEntries; }
    bool Contains( void* pEntryData );
    void Clear( void ) { m_NumEntries = 0; }
};

/**************** end of  "Util.h" ********************************************/