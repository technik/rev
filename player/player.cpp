//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "player.h"
#include <math/algebra/vector.h>

namespace rev {

	const math::Vec2f vertices[] = {
		{1.f, 1.f},
		{-1.f, 1.f},
		{0.f,-1.f}
	};
	const uint16_t indices[] = { 0, 1, 2};
	GLuint vao;
	GLuint vbo[2];

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		assert(!mGfxDriver);

		mGfxDriver = graphics::GraphicsDriverGL::createDriver(_window);
		if(mGfxDriver) {
			glClearColor(0.f,0.5f,1.f,1.f);
			// Create shader
			mShader = graphics::Shader::createShader(
				R"(
#ifdef VTX_SHADER
			layout(location = 0) in vec2 vertex;
			
			void main ( void )
			{
				gl_Position = vec4(vertex, 0.0, 1.0);
			}
#endif
#ifdef PXL_SHADER
			out lowp vec3 color;
			void main (void) {
				
				color = vec3(0.0,1.0,0.0);
			}
#endif
				)"
			);
			if(mShader)
				mShader->bind();

			// Create geometry
			glGenVertexArrays(1,&vao);
			glGenBuffers(2,vbo);
			// VAO
			glBindVertexArray(vao);
			// VBO for vertex data
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(math::Vec2f)*3, vertices, GL_STATIC_DRAW);
			// VBO for index
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vbo[1]);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint16_t)*3, indices, GL_STATIC_DRAW);

			// Attributes
			glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
			glEnableVertexAttribArray(0); // Vertex pos
			glBindVertexArray(0);
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update() {
		if(!mGfxDriver)
			return true;

		glClear(GL_COLOR_BUFFER_BIT);

		glBindVertexArray(vao);
		glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, nullptr);

		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev