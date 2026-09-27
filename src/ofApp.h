#pragma once

// ============================================================================
// ofApp.h
// Header file for the Weather App.
//
// This file declares the ofApp class, its functions and its variables.
// The actual implementation of these functions is written in ofApp.cpp.
// ============================================================================

// ofxGui is used to create the graphical user interface,
// including the city input field and search button.
#include "ofMain.h"
#include "ofxGui.h"

// ============================================================================
// MAIN APPLICATION CLASS
// ============================================================================

// ofApp inherits from ofBaseApp.
// ofBaseApp provides the standard openFrameworks application structure,
// including setup(), update() and draw().
class ofApp : public ofBaseApp {

public:
	// ------------------------------------------------------------------------
	// APPLICATION LIFECYCLE FUNCTIONS
	// ------------------------------------------------------------------------

	// Called once when the application starts.
	// It is used to initialise the GUI, fonts, images and API settings.
	void setup() override;

	// Called repeatedly while the application is running.
	// In this project, weather data is updated when the API response arrives.
	void update() override;

	// Called repeatedly to draw the application interface on the screen.
	void draw() override;

	// ------------------------------------------------------------------------
	// API HANDLING
	// ------------------------------------------------------------------------

	// Gets the city entered by the user and creates the WeatherAPI request.
	// This function is triggered when the user presses the search button.
	void fetchWeatherData();

	// Receives the response from the asynchronous API request.
	// It processes the HTTP response and extracts the weather information.
	void urlResponse(ofHttpResponse & response);

	// ------------------------------------------------------------------------
	// BACKGROUND HANDLING
	// ------------------------------------------------------------------------

	// Selects the correct background image based on the
	// weather condition returned by the API.
	void updateBackground();

	// Draws the selected weather image so that it covers
	// the entire application window.
	void drawBackgroundCover();

	// ------------------------------------------------------------------------
	// GUI ELEMENTS
	// ------------------------------------------------------------------------

	// Main ofxGui panel containing the search controls.
	ofxPanel gui;

	// Text input where the user enters a city name.
	ofxInputField<std::string> locationInput;

	// Button used to request the weather forecast.
	ofxButton searchBtn;

	// ------------------------------------------------------------------------
	// FONTS
	// ------------------------------------------------------------------------

	// Different font sizes are used for different parts
	// of the dashboard interface.
	ofTrueTypeFont titleFont;
	ofTrueTypeFont bodyFont;
	ofTrueTypeFont smallFont;

	// ------------------------------------------------------------------------
	// WEATHER BACKGROUND IMAGES
	// ------------------------------------------------------------------------

	// Background image used when no specific weather condition
	// matches the available categories.
	ofImage defaultBackground;

	// Background used for cloudy or poor-visibility conditions.
	ofImage cloudyBackground;

	// Background used for rain and drizzle conditions.
	ofImage rainyBackground;

	// Background used for snow and icy conditions.
	ofImage snowyBackground;

	// Background used for sunny and clear conditions.
	ofImage sunnyBackground;

	// Pointer to the background image that should currently
	// be displayed on the screen.
	//
	// It changes when new weather data is received.
	ofImage * currentBackground = nullptr;

	// ------------------------------------------------------------------------
	// API CONFIGURATION AND WEATHER DATA
	// ------------------------------------------------------------------------

	// WeatherAPI key used to authenticate API requests.
	// This key is used in ofApp.cpp when constructing the API URL.
	const std::string apiKey = "";

	// ------------------------------------------------------------------------
	// WEATHER INFORMATION
	// ------------------------------------------------------------------------

	// Name of the city returned by the API.
	std::string cityName = "N/A";

	// Country associated with the selected city.
	std::string country = "N/A";

	// Text description of the current weather condition.
	// Examples could include "Overcast", "Light rain shower"
	// or "Patchy light snow".
	std::string conditionText = "N/A";

	// Current temperature in Celsius.
	float tempC = 0.0f;

	// "Feels like" temperature in Celsius.
	float feelsLikeC = 0.0f;

	// Relative humidity as a percentage.
	int humidity = 0;

	// ------------------------------------------------------------------------
	// APPLICATION STATUS
	// ------------------------------------------------------------------------

	// Message displayed to the user to explain the current state
	// of the application.
	std::string statusMessage = "Enter a city to search.";

	// True while the application is waiting for an API response.
	bool isLoading = false;

	// True when an error has occurred.
	bool hasError = false;
};
