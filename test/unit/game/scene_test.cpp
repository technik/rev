//----------------------------------------------------------------------------------------------------------------------
// Math unit testing
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <game/scene/scene.h>
#include <game/scene/sceneNode.h>
#include <game/scene/ComponentLoader.h>

namespace rev { namespace game {
//----------------------------------------------------------------------------------------------------------------------
// Component mock
struct MockComponent : Component
{
	char a;
	void serialize(std::ostream& out) const override {}
};

// Loader Mock
ComponentLoader::ComponentLoader()
{}

std::unique_ptr<Component> ComponentLoader::loadComponent(std::istream& in) const
{
	auto c = std::make_unique<MockComponent>();
	in >> c->a;
	return std::move(c);
}

// Saver mock
void ComponentSerializer::save(const Component& c, std::ostream& out) const
{
	out << reinterpret_cast<const MockComponent*>(&c)->a;
}

}} // namespace rev::game

using namespace rev::game
;
//----------------------------------------------------------------------------------------------------------------------
int main() {
	/*ComponentLoader		loader;
	ComponentSerializer saver;
	{
		std::stringstream  stream;
		Scene s1;
		s1.save(stream, saver);
		Scene s2;
		s2.load(stream, loader);
		assert(s2.root()->children().empty());
		assert(s2.root()->components().empty());
	}
	{
		std::stringstream  stream;
		Scene s1;
		s1.root()->addChild(std::make_shared<SceneNode>());
		s1.save(stream, saver);
		s1.save(stream, saver);
		Scene s2;
		s2.load(stream, loader);
		assert(s2.root()->components().empty());
		assert(s2.root()->children().size() == 1);
		assert(s2.root()->children()[0]->components().empty());
	}
	{
		std::stringstream  stream;
		Scene s1;
		auto child1 = std::make_shared<SceneNode>();
		auto componentA = std::make_unique<MockComponent>();
		componentA->a = 'A';
		child1->addComponent(std::move(componentA));
		s1.root()->addChild(child1);
		s1.save(stream, saver);

		Scene s2;
		s2.load(stream, loader);
		assert(s2.root()->children().size() == 1);
		assert(s2.root()->children()[0]->components().size() == 1);
		assert(s2.root()->children()[0]->component<MockComponent>()->a == 'A');
	}*/
	return 0;
}