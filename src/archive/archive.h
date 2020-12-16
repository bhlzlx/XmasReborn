#pragma once

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include <string>

namespace ugi
{
    enum SeekFlag
    {
        SeekCur = 0,
        SeekEnd,
        SeekSet
    };

    struct IFile
    {
		typedef void(*MemoryFreeCB)(void *);
		//
        virtual size_t read( size_t _bytes, IFile* out_ ) = 0;
        virtual size_t read( size_t _bytes, void* out_ ) = 0;
        virtual bool readable() = 0;
        
        virtual size_t write( size_t _bytes, IFile* out_ ) = 0;
        virtual size_t write( size_t _bytes, const void* out_ ) = 0;
        virtual bool writable() = 0;

        virtual size_t tell() = 0;
        virtual bool seek( SeekFlag _flag, long _offset ) = 0;
        virtual bool seekable() = 0;

		virtual const void* constData() const { return nullptr; }

		virtual size_t size() = 0;

        virtual void release() = 0;

        virtual ~IFile(){}
    };

	IFile* CreateMemoryBuffer(void * _ptr, size_t _length, IFile::MemoryFreeCB _freeCB = nullptr);
	IFile* CreateMemoryBuffer(size_t _length, IFile::MemoryFreeCB _freeCB = [](void* _ptr) {
		free(_ptr); 
	});
}