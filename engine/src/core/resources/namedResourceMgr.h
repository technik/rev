//----------------------------------------------------------------------------------------------------------------------
// The MIT License (MIT)
// 
// Copyright (c) 2015 Carmelo J. Fernández-Agüera Tortosa
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Resource manager for string-indexed resources
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_CORE_RESOURCE_NAMEDRESOURCEMGR_H_
#define _REV_CORE_RESOURCE_NAMEDRESOURCEMGR_H_

#include <map>
#include <memory>
#include <string>

namespace rev {
	namespace core {
		//-----------------------------------------------------------------------------------------------------
		template<class Key_>
		class NamedResourceMgr {
		public:
			static void init() { sInstance = new NamedResourceMgr<Key_>; }
			static void end() { delete sInstance; sInstance = nullptr; }

			typedef std::shared_ptr<Val_>	Ptr;

			// Resource manager interface
			Ptr get(const std::string&);

		private:
			ResourceMgr() = default;
			~ResourceMgr() = default;

			static ResourceMgr*			sInstance;
			std::map<Key_, Val_*>	mResources;
		};

		//-----------------------------------------------------------------------------------------------------
		template<class Val_>
		class NamedResource {
		public:
			typedef NamedResourceMgr<Val_>	Mgr;
			typedef typename Mgr::Ptr	Ptr;

			inline static Mgr*	manager() {
				if(!sInstance)
					sInstance = new Mgr;
				return sInstance;
			}

		private:
			static Mgr* sInstance;
		};
	}
}

#endif // _REV_CORE_RESOURCE_NAMEDRESOURCEMGR_H_