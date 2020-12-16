#include "archive.h"
#include <memory.h>
#include"../string/path.h"

namespace ugi
{
    class MemFile: public IFile
    {
		friend IFile* CreateMemoryBuffer( void *, size_t, MemoryFreeCB _cb );
		friend IFile* CreateMemoryBuffer(size_t _length, MemoryFreeCB _cb);
    private:
        void* _raw;
        long _size;
        long _position;
        MemoryFreeCB _destructor = nullptr;
    public:
        MemFile()
        {
            _raw = nullptr;
            _size = 0;
            _position = 0;
        }
        virtual bool readable()
        {
            return true;
        }

        virtual bool writable()
        {
            return true;
        }
        
        virtual bool seekable()
        {
            return true;
        }

        virtual size_t read( size_t _bytes, IFile* out_ )
        {
            char chunk[64];
            size_t bytesLeft = _bytes;
            size_t roundRead = 0;
            do
            {
                size_t memLeft = (_size - _position);
                roundRead = bytesLeft > sizeof(chunk) ? sizeof(chunk) : bytesLeft;
                size_t readReal = roundRead > memLeft ? memLeft : roundRead;
                memcpy( chunk, (char*)_raw + _position, readReal );
                _position += readReal;
                bytesLeft -= readReal;
                out_->write( readReal, chunk );
                if( readReal != roundRead )
                    break;
            }
            while( bytesLeft );
            return _bytes - bytesLeft;
        }

        virtual size_t read( size_t _bytes, void* out_ )
        {
            size_t memLeft = (_size - _position);
            size_t readReal = _bytes > memLeft ? memLeft : _bytes; 
            memcpy( out_, (char*)_raw + _position, readReal);
            return readReal;
        }

        virtual size_t write( size_t _bytes, IFile* _in )
        {
            size_t sizeWrite = (_size - _position);
            sizeWrite = sizeWrite > _bytes? _bytes:sizeWrite;
            auto nRead = _in->read( sizeWrite,  (char*)_raw + _position);
            _position += nRead;
            return nRead;
        }

        virtual size_t write( size_t _bytes, const void* _in )
        {
            size_t sizeWrite = (_size - _position);
            sizeWrite = sizeWrite > _bytes? _bytes:sizeWrite;
            memcpy( (char*)_raw + _position, _in, sizeWrite );
            _position+=sizeWrite;
            return sizeWrite;
        }

        virtual size_t tell()
        {
            return _position;
        }

        virtual bool seek( SeekFlag _flag, long _offset )
        {
            switch( _flag)
            {
                case SeekFlag::SeekCur:
                    _position += _offset;
                    break;
                case SeekFlag::SeekEnd:
                    _position = _size + _offset;
                    break;
                case SeekFlag::SeekSet:
                    _position = _offset;
            }

            if(_position < 0 )
            {
                _position = 0;
            }
            else if( _position > _size )
            {
                _position  = _size;
            }
            return true;
        }

		virtual size_t size()
		{
			return _size;
		}

		virtual const void* constData() const { 
			return this->_raw; 
		}
        //
        virtual void release()
        {
            if( _destructor )
                _destructor( _raw );
            delete this;
        }
    };

	IFile* CreateMemoryBuffer(void * _ptr, size_t _length, IFile::MemoryFreeCB _freeCB )
	{
		MemFile* buffer = new MemFile();
		buffer->_raw = _ptr;
		buffer->_destructor = _freeCB;
		buffer->_size = _length;
		buffer->_position = 0;
		return buffer;
	}

	IFile* CreateMemoryBuffer( size_t _length, IFile::MemoryFreeCB _freeCB)
	{
		MemFile* buffer = new MemFile();
		void * _ptr = malloc(_length);
		buffer->_raw = _ptr;
		buffer->_destructor = _freeCB;
		buffer->_size = _length;
		buffer->_position = 0;
		return buffer;
	}

}