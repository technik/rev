//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

namespace rev { namespace video {

	template<class Derived_>
	class GraphicsDriverBase {
	public:
		static Derived_& get() { return *sInstance; }

	protected:
		GraphicsDriverBase() {
			assert(!sInstance); // Do not create the driver more than once
			sInstance = static_cast<Derived_*>(this);
		}

		~GraphicsDriverBase() {
			sInstance = nullptr; // Clean instance, so we can safely re-create
		}

	private:
		static Derived_* sInstance;
	};

}}