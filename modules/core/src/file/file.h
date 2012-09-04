//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Basic interaction with files

#ifndef _REV_CORE_FILE_FILE_H_
#define _REV_CORE_FILE_FILE_H_

namespace rev
{
	class File
	{
	public:
		static File * open(const char * fileName, bool writeable = true);

		~File();

		const void *	buffer			() const;
		const char *	bufferAsText	() const;
		int				sizeInBytes		() const;

	private:
		File();

		unsigned	mSize;
		void*		mBuffer;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	inline const void * File::buffer () const { return mBuffer; }
	inline const char * File::bufferAsText () const { return static_cast<const char*>(mBuffer); }
	inline int			 File::sizeInBytes () const { return mSize; }

}	// namespace rev

#endif // _REV_CORE_FILE_FILE_H_
