//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2020
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <streambuf>
#include <iostream>
#include <istream>

namespace rev {
	namespace core {
		
		class File
		{
		public:
			File(const std::string& _path);
			~File();

			template<class T = void>
			const T *	buffer		() const;
			template<class T = void>
			T *			buffer		();
			size_t		size		() const;

			std::istream&	asStream() {				
				return mStream;
			}


#ifdef ANDROID
			static void setAssetMgr( AAssetManager* _mgr );
#endif // ANDROID

		private:
			size_t		mSize = 0;
			void*		mBuffer = nullptr;
			
			struct  filebuf : public std::streambuf
			{
				void set(char* begin, char* end) {
					this->setg(begin, begin, end);
				}
			}				mBufferAdapter;
			std::istream	mStream;
#if _DEBUG
			std::string	mPath;
#endif // _DEBUG
#ifdef ANDROID
			static AAssetManager* sAssetMgr;
#endif // ANDROID
		};

		//------------------------------------------------------------------------------------------------------------------
		// Inline implementation
		//------------------------------------------------------------------------------------------------------------------
		template<class T>
		inline T*		File::buffer () { return reinterpret_cast<T*>(mBuffer); }
		template<class T>
		inline const T*	File::buffer () const { return reinterpret_cast<const T*>(mBuffer); }
		inline size_t	File::size() const { return mSize; }
	}
}