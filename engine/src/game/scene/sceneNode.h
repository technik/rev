//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <vector>

namespace rev { namespace game {

	class Component;

	/// Nodes are containers of logically related components
	/// You can attach components to a node
	/// You can attach a node to another node
	class SceneNode
	{
	public:
		// Handle components
		void				addComponent	(Component * _component);
		void				removeComponent	(Component * _component);
		size_t				nComponents		() const					{ return mComponents.size(); }
		Component&			component		(size_t _i) const			{ return *mComponents[_i]; }

		template<class T_>	
		T_*					component		() const {
			for (Component* c : mComponents) {
				if(typeid(*c) == typeid(T_))
					return static_cast<T_*>(c);
			}
			return nullptr;
		}

	private:
		std::vector<Component*>	mComponents;
	};
}}
