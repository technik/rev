//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

namespace rev {
	namespace core {

		class World;

		/// \Brief: World layers stand for logically connected systems of a world (e.g. a RenderLayer, a PhysicsWorld, IA Context)
		/// World Layers group together a set of components of similar types that will be updated in the same batch.
		/// For example, a render layer could include all its render objects, lights and cameras, or a physics world would include
		/// all rigid bodies, colliders and forces that belong to the same world.
		/// World layers register their factories to a world, so when the world loads a new scene, the factories will be invoked
		/// for the layer's components.
		class WorldLayer {
		public:
			virtual ~WorldLayer() = 0;

			virtual void init() {}
			/// \return true if the game should stop
			virtual bool update(float _dt) {};
		};
} }