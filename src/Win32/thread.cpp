// Based on SLibrary.
// //////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// This class is produced by AST. Check homepage when you need any help.
// Mail Address.    ast@qt-space.com
// Official HP URL. http://ast.qt-space.com/

#include "../thread.h"

#define USESDLTHREAD				// SDL�̃X���b�h�g�p

#ifdef USESDLTHREAD
#include <SDL.h>
#else
#include <windows.h>
#include <process.h>
#endif



// Constructer
cThread::cThread( void )
{
	this->m_bCancel			= true;
	this->m_hThread			= NULL;
	this->m_BeginTheadParam	= NULL;
}


// Destructer
cThread::~cThread( void )
{
	bool bWaiting = this->Waiting();
	if( bWaiting == false ){
		#ifdef USESDLTHREAD
		#if SDL_VERSION_ATLEAST(2,0,0)
		// SDL2.0�ł�SDL_KillThread�͂Ȃ��Ȃ��Ă��܂���
		#else
		SDL_KillThread( (SDL_Thread *)this->m_hThread );
		#endif
		#else
		::TerminateThread( (HANDLE)this->m_hThread, 0 );
		#endif
	}
}


// Start Thread. 
bool cThread::BeginThread ( void *lpVoid )
{
	bool bSuccess = false;
	
	if( this->m_hThread == NULL ){
		this->m_BeginTheadParam = lpVoid;
		this->m_bCancel			= false;
		
		#ifdef USESDLTHREAD
		#if SDL_VERSION_ATLEAST(2,0,0)
		HTHREAD hThread = (HTHREAD)SDL_CreateThread( (int (*)(void *))ThreadProc, NULL, (void *)this );
		#else
		HTHREAD hThread = (HTHREAD)SDL_CreateThread( (int (*)(void *))ThreadProc, (void *)this );
		#endif
		if( hThread ){
			this->m_hThread = hThread;
			bSuccess = true;
		}
		#else
		HTHREAD hThread = (HTHREAD)::_beginthread( ThreadProc, 0, reinterpret_cast<void *>(this) );
		if( hThread != (HTHREAD)(unsigned int)-1 ){
			this->m_hThread = hThread;
			::SetThreadPriority( hThread, THREAD_PRIORITY_NORMAL );
			bSuccess = true;
		}
		#endif
	}
	
	return bSuccess;
}


// Wait for create thread end. ( forvOnThread end )
//inline bool cThread::Waiting (DWORD dwWaitTime)
bool cThread::Waiting( void )
{
	bool bSuccess = false;
	
	if( this->m_hThread != NULL ){
		#ifdef USESDLTHREAD
		int status = 0;
		SDL_WaitThread( (SDL_Thread *)this->m_hThread, &status );
		if( !status ){
		#else
		DWORD dwRet = ::WaitForSingleObject( (HANDLE)this->m_hThread, INFINITE );
		if( dwRet == WAIT_OBJECT_0 ){
		#endif
			this->m_hThread = NULL;
			bSuccess = true;
		}
	}else{
		bSuccess = true;
	}
	
	return bSuccess;
}


void cThread::Cancel()
{
	this->cCritical::Lock();
	this->m_bCancel = true;
	this->cCritical::UnLock();
}


bool cThread::IsCancel()
{
	bool bCancel = false;
	this->cCritical::Lock();
	bCancel = this->m_bCancel;
	this->cCritical::UnLock();
	return bCancel;
}


// Default thread procedure. Don't call this method in direct!
void cThread::ThreadProc(void *lpVoid)
{
	static __thread cThread *lpThis;
	
	if( !lpThis ) lpThis = STATIC_CAST( cThread *, lpVoid );	// �������g�̃I�u�W�F�N�g�|�C���^�擾
	lpThis->OnThread( lpThis->m_BeginTheadParam );	// virtual Procedure 
	lpThis->m_hThread = NULL;
	#ifndef USESDLTHREAD
	::_endthread();
	#endif
}
