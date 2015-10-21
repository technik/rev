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
		class ContextObj {
		protected:
			ContextObj() = default;

			//--------------------------------------------------
			virtual ~ContextObj() {
				setContext(nullptr);
			}

			//--------------------------------------------------
			void setContext(Context_* _c) {
				Derived_* derivedThis = static_cast<Derived_*>(this);
				if(mContext)
					mContext->erase(derivedThis);
				mContext = _c;
				if(mContext)
					mContext->insert(derivedThis);
			}

			virtual Context_* defaultContext() { return nullptr; }
		private:
			Context_* mContext = nullptr;
		};

	}
}

#endif // _REV_CORE_CONTEXT_CONTEXTOBJ_H_