//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Engine's default player's entry point

int main(int _argc, const char** _argv) {
	
	rev::Player app(_argc,_argv);

	while(app.update());
	return 0;
}