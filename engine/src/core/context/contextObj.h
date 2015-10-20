//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Contexts include a list of objects of some type.
#ifndef _REV_CORE_CONTEXT_CONTEXTOBJ_H_
#define _REV_CORE_CONTEXT_CONTEXTOBJ_H_

namespace rev {
	namespace core {
		
		template<class Derived_, class Context_> // Context must fulfill the semantics of a set<Derived_>
		class ContextObj : {
		protected:
			//--------------------------------------------------
			ContextObj(Context_* _context = defaultContext())
				: mContext(_context)
			{
				setContext(_context);
			}

			//--------------------------------------------------
			virtual ~ContextObj() {
				setContext(nullptr);
			}

			//--------------------------------------------------
			void setContext(Context_* _c) {
				if(mContext)
					mContext->erase(this);
				mContext = this;
				if(mContext)
					mContext->insert(this);
			}

			virtual Context_* defaultContext() { return nullptr; }
		};

	}
}

#endif // _REV_CORE_CONTEXT_CONTEXTOBJ_H_