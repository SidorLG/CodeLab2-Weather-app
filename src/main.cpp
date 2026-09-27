// ============================================================================
// main.cpp
// Entry point of the Weather App.
// This file is responsible for creating the application window and
// starting the ofApp class where the main application logic is located.
// ============================================================================

#include "ofApp.h"
#include "ofMain.h"

//========================================================================
int main() {

	// Create settings for the openFrameworks application window.
	// These settings allow us to control the size and display mode.
	ofGLWindowSettings settings;

	// Set the application window size to 1024 pixels wide
	// and 768 pixels high.
	settings.setSize(1024, 768);

	// Set the application to run in a normal window.
	// OF_FULLSCREEN could be used instead if fullscreen mode 
	settings.windowMode = OF_WINDOW;

	// Create the OpenGL window using the settings  
	auto window = ofCreateWindow(settings);

	// Create an instance of the ofApp class and attach
	// std::make_shared manages the application's object in memory
	ofRunApp(window, std::make_shared<ofApp>());

	// Start the openFrameworks main loop.
	// This keeps the application running and repeatedly calls
	ofRunMainLoop();
}
