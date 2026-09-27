// ============================================================================
// ofApp.cpp
// Main implementation file for the Weather App.
//
// This file contains the main application logic:
// - GUI setup
// - WeatherAPI requests
// - JSON parsing
// - Error handling
// - Dynamic weather backgrounds
// - Dashboard rendering
// ============================================================================

#include "ofApp.h"

// ============================================================================
// SETUP
// ============================================================================

void ofApp::setup() {

	// Register the application to receive asynchronous URL responses.
	// When the API request finishes, openFrameworks will call urlResponse().
	ofRegisterURLNotification(this);

	// Enable alpha blending.
	// This allows transparent colours to be used for the glass-style
	// panels and the dark overlay placed over background images.
	ofEnableAlphaBlending();

	// ------------------------------------------------------------------------
	// FONTS
	// ------------------------------------------------------------------------

	// Load different font sizes for different parts of the interface.
	// The title is larger, the body text is medium-sized and the status
	// information uses a smaller font.
	bodyFont.load(OF_TTF_SANS, 26);
	titleFont.load(OF_TTF_SANS, 44);
	smallFont.load(OF_TTF_SANS, 18);

	// ------------------------------------------------------------------------
	// GUI SETUP
	// ------------------------------------------------------------------------

	// Set the default size of the ofxGui controls.
	ofxGuiSetDefaultWidth(440);
	ofxGuiSetDefaultHeight(58);

	// Add some space between the GUI text and the control borders.
	ofxGuiSetTextPadding(14);

	// Create the main GUI panel.
	gui.setup("WEATHER DASHBOARD");

	// Add a text input field where the user can enter a city.
	// "London" is used as the initial example value.
	gui.add(locationInput.setup("City Search", "London"));

	// Add the button used to start the weather search.
	gui.add(searchBtn.setup("Get Forecast"));

	// ------------------------------------------------------------------------
	// GUI COLOURS
	// ------------------------------------------------------------------------

	// Make the built-in ofxGui panel transparent.

	// The application draws custom glass-style panels manually in draw(),
	// so the default dark GUI background should not cover them.
	gui.setBackgroundColor(ofColor(0, 0, 0, 0));

	// Set a slightly transparent white fill for the GUI controls.
	gui.setFillColor(ofColor(255, 255, 255, 18));

	// Set a subtle white border around the controls.
	gui.setBorderColor(ofColor(255, 255, 255, 65));

	// Make the GUI text white and mostly opaque.
	gui.setTextColor(ofColor(255, 255, 255, 245));

	// Make the GUI header transparent as well.
	gui.setHeaderBackgroundColor(ofColor(0, 0, 0, 0));

	// Connect the button's click event to fetchWeatherData().
	//
	// This means that when the user clicks "Get Forecast",
	// the fetchWeatherData() function will be executed.
	searchBtn.addListener(this, &ofApp::fetchWeatherData);

	// ------------------------------------------------------------------------
	// WEATHER BACKGROUNDS
	// ------------------------------------------------------------------------

	// Load all weather background images from the bin/data folder.
	defaultBackground.load("default.jpg");
	cloudyBackground.load("cloudy.jpeg");
	rainyBackground.load("rainy.jpg");
	snowyBackground.load("snowy.png");
	sunnyBackground.load("sunny.jpg");

	// Display the default background when the application first starts.
	currentBackground = &defaultBackground;
}

// ============================================================================
// FETCH WEATHER DATA
// ============================================================================

void ofApp::fetchWeatherData() {

	// Get the city name entered by the user.
	std::string city = locationInput;

	// ------------------------------------------------------------------------
	// INPUT VALIDATION
	// ------------------------------------------------------------------------

	// Check whether the user left the input field empty.
	//
	// If it is empty, there is no reason to send an API request.
	if (city.empty()) {

		// Tell the application that an error occurred.
		hasError = true;

		// Display a clear error message to the user.
		statusMessage = "Error: Input cannot be empty.";

		// Stop this function so no API request is made.
		return;
	}

	// ------------------------------------------------------------------------
	// LOADING STATE
	// ------------------------------------------------------------------------

	// The application is now waiting for the API response.
	isLoading = true;

	// Remove any previous error state.
	hasError = false;

	// Inform the user that the request is being processed.
	statusMessage = "Fetching weather data...";

	// ------------------------------------------------------------------------
	// WEATHERAPI URL
	// ------------------------------------------------------------------------

	// Start building the WeatherAPI current-weather endpoint.
	std::string url = "https://api.weatherapi.com/v1/current.json?key=";

	// Add the API key used to authenticate the request.
	url += apiKey;

	// Add the city entered by the user.
	url += "&q=" + city;

	// Disable air-quality information because this application
	// does not need that data.
	url += "&aqi=no";

	// ------------------------------------------------------------------------
	// ASYNCHRONOUS API REQUEST
	// ------------------------------------------------------------------------

	// Send the request asynchronously.
	//
	// The application does not freeze while waiting for the API.
	// Once the response arrives, urlResponse() will process it.
	ofLoadURLAsync(url, "weatherReq");
}

// ============================================================================
// API RESPONSE
// ============================================================================

void ofApp::urlResponse(ofHttpResponse & response) {

	// The API request has finished, so the loading state is removed.
	isLoading = false;

	// ------------------------------------------------------------------------
	// SUCCESSFUL RESPONSE
	// ------------------------------------------------------------------------

	// HTTP status 200 means that the request was successful.
	if (response.status == 200) {

		try {

			// Convert the API response text into a JSON object.
			//
			// WeatherAPI returns the weather information in JSON format.
			ofJson json = ofJson::parse(response.data.getText());

			// Check that the JSON contains the two main sections
			// required by the application.
			if (json.contains("location") && json.contains("current")) {

				// ------------------------------------------------------------
				// LOCATION DATA
				// ------------------------------------------------------------

				// Read the city name from the JSON response.
				cityName = json["location"]["name"].get<std::string>();

				// Read the country name.
				country = json["location"]["country"].get<std::string>();

				// ------------------------------------------------------------
				// WEATHER DATA
				// ------------------------------------------------------------

				// Read the current temperature in Celsius.
				tempC = json["current"]["temp_c"].get<float>();

				// Read the "feels like" temperature.
				feelsLikeC = json["current"]["feelslike_c"].get<float>();

				// Read the relative humidity percentage.
				humidity = json["current"]["humidity"].get<int>();

				// Read the text description of the current condition.
				//
				// Examples:
				// "Overcast"
				// "Light rain shower"
				// "Patchy light snow"
				conditionText = json["current"]["condition"]["text"]
									.get<std::string>();

				// ------------------------------------------------------------
				// BACKGROUND UPDATE
				// ------------------------------------------------------------

				// Choose the appropriate background image according
				// to the weather condition returned by the API.
				updateBackground();

				// Tell the user that the data was successfully processed.
				statusMessage = "Data successfully loaded!";
			}

			// If the JSON does not contain the expected structure,
			// display an error instead of trying to access missing data.
			else {

				hasError = true;

				statusMessage = "Error: Invalid JSON structure.";
			}
		}

		// Catch JSON parsing or data-conversion errors.
		//
		// This prevents invalid API data from crashing the application.
		catch (const std::exception &) {

			hasError = true;

			statusMessage = "Error: Could not read weather data.";
		}
	}

	// ------------------------------------------------------------------------
	// INVALID LOCATION
	// ------------------------------------------------------------------------

	// HTTP 400 or 404 can indicate that the requested location
	// could not be found or the request was invalid.
	else if (response.status == 400 || response.status == 404) {

		hasError = true;

		statusMessage = "Error: Location not found. Try another city.";
	}

	// ------------------------------------------------------------------------
	// OTHER API / NETWORK ERRORS
	// ------------------------------------------------------------------------

	else {

		// Set the application into an error state.
		hasError = true;

		// Display the HTTP response code.
		//
		// For example, HTTP code -1 can occur when the computer
		// cannot connect to the API, such as when it is offline.
		statusMessage = "API Error: HTTP Code " + ofToString(response.status);
	}
}

// ============================================================================
// UPDATE BACKGROUND
// ============================================================================

void ofApp::updateBackground() {

	// Convert the weather condition to lowercase.
	//
	// This makes the comparisons case-insensitive.
	// For example, "Rain" and "rain" will be treated the same way.
	std::string condition = ofToLower(conditionText);

	// ------------------------------------------------------------------------
	// RAIN
	// ------------------------------------------------------------------------

	// If the condition contains "rain" or "drizzle",
	// use the rainy background.
	if (condition.find("rain") != std::string::npos || condition.find("drizzle") != std::string::npos) {

		currentBackground = &rainyBackground;
	}

	// ------------------------------------------------------------------------
	// SNOW / ICE
	// ------------------------------------------------------------------------

	// Check for snow, sleet, blizzard or ice pellets.
	else if (condition.find("snow") != std::string::npos || condition.find("sleet") != std::string::npos || condition.find("blizzard") != std::string::npos || condition.find("ice pellets") != std::string::npos) {

		currentBackground = &snowyBackground;
	}

	// ------------------------------------------------------------------------
	// THUNDERSTORM
	// ------------------------------------------------------------------------

	// Thunder or lightning conditions use the default/storm-style image.
	else if (condition.find("thunder") != std::string::npos || condition.find("lightning") != std::string::npos) {

		currentBackground = &defaultBackground;
	}

	// ------------------------------------------------------------------------
	// SUNNY / CLEAR
	// ------------------------------------------------------------------------

	// Sunny and clear weather use the sunny background.
	else if (condition.find("sunny") != std::string::npos || condition.find("clear") != std::string::npos) {

		currentBackground = &sunnyBackground;
	}

	// ------------------------------------------------------------------------
	// CLOUD / VISIBILITY
	// ------------------------------------------------------------------------

	// Cloudy, overcast, mist, fog and haze conditions
	// use the cloudy background.
	else if (condition.find("cloud") != std::string::npos || condition.find("overcast") != std::string::npos || condition.find("mist") != std::string::npos || condition.find("fog") != std::string::npos || condition.find("haze") != std::string::npos) {

		currentBackground = &cloudyBackground;
	}

	// ------------------------------------------------------------------------
	// FALLBACK
	// ------------------------------------------------------------------------

	// If the API returns a condition that is not recognised,
	// use the default image instead.
	else {

		currentBackground = &defaultBackground;
	}
}

// ============================================================================
// DRAW BACKGROUND
// ============================================================================

void ofApp::drawBackgroundCover() {

	// Check whether the selected image exists and loaded correctly.
	//
	// If it did not load, draw a plain dark background instead.
	if (currentBackground == nullptr || !currentBackground->isAllocated()) {

		ofSetColor(25, 30, 40);

		ofDrawRectangle(
			0,
			0,
			ofGetWidth(),
			ofGetHeight());

		return;
	}

	// Get the dimensions of the selected image.
	float imageWidth = currentBackground->getWidth();

	float imageHeight = currentBackground->getHeight();

	// Get the current application window dimensions.
	float windowWidth = ofGetWidth();

	float windowHeight = ofGetHeight();

	// ------------------------------------------------------------------------
	// IMAGE SCALING
	// ------------------------------------------------------------------------

	// Calculate the scale required for the image to completely
	// cover the application window.
	//
	// std::max() chooses the larger scale so there are no empty
	// spaces around the image.
	float scale = std::max(
		windowWidth / imageWidth,
		windowHeight / imageHeight);

	// Calculate the final size of the image after scaling.
	float drawWidth = imageWidth * scale;

	float drawHeight = imageHeight * scale;

	// Centre the image inside the application window.
	//
	// Some parts of the image may be cropped, but the image
	// will not be stretched or distorted.
	float x = (windowWidth - drawWidth) / 2.0f;

	float y = (windowHeight - drawHeight) / 2.0f;

	// Draw the selected background image.
	ofSetColor(255);

	currentBackground->draw(
		x,
		y,
		drawWidth,
		drawHeight);

	// ------------------------------------------------------------------------
	// DARK OVERLAY
	// ------------------------------------------------------------------------

	// Add a transparent black overlay.
	//
	// This makes the background slightly darker and improves
	// the readability of the white 
	ofSetColor(0, 0, 0, 65);

	ofDrawRectangle(
		0,
		0,
		windowWidth,
		windowHeight);

	// Reset the drawing colour back to white.
	ofSetColor(255);
}

// ============================================================================
// UPDATE
// ============================================================================

void ofApp::update() {

	// The weather information is updated when the asynchronous
	// API response is received inside urlResponse().
	//
	// Therefore, no continuous calculations are required here.
}

// ============================================================================
// DRAW
// ============================================================================

void ofApp::draw() {

	// Draw the weather background first.
	// All other interface elements will then appear on top of it.
	drawBackgroundCover();

	// ------------------------------------------------------------------------
	// MAIN TITLE
	// ------------------------------------------------------------------------

	// Position of the main title.
	const float marginX = 42.0f;
	const float titleY = 72.0f;

	// Set the text colour to white.
	ofSetColor(255, 255, 255, 255);

	// Draw the main application title.
	titleFont.drawString(
		"WEATHER FORECAST",
		marginX,
		titleY);

	// Draw a smaller subtitle underneath the main title.
	smallFont.drawString(
		"LIVE WEATHER DASHBOARD",
		marginX,
		titleY + 30.0f);

	// ------------------------------------------------------------------------
	// DASHBOARD LAYOUT
	// ------------------------------------------------------------------------

	// Position and dimensions of the GUI and weather result panels.
	const float panelY = 128.0f;

	const float guiWidth = 440.0f;

	const float resultsX = 500.0f;
	const float resultsY = 128.0f;

	const float resultsW = 492.0f;
	const float resultsH = 438.0f;

	// ------------------------------------------------------------------------
	// LEFT GUI PANEL
	// ------------------------------------------------------------------------

	// Draw a semi-transparent rounded panel behind
	// the city search controls.
	ofSetColor(8, 14, 24, 105);

	ofDrawRectRounded(
		marginX - 12.0f,
		panelY - 12.0f,
		guiWidth + 24.0f,
		185.0f,
		18.0f);

	// Draw a thin white border around the panel.
	ofNoFill();

	ofSetColor(255, 255, 255, 75);

	ofDrawRectRounded(
		marginX - 12.0f,
		panelY - 12.0f,
		guiWidth + 24.0f,
		185.0f,
		18.0f);

	// Return to filled drawing mode.
	ofFill();

	// Position the ofxGui controls.
	gui.setPosition(
		marginX,
		panelY);

	// Draw the interactive GUI.
	gui.draw();

	// ------------------------------------------------------------------------
	// RIGHT WEATHER RESULTS CARD
	// ------------------------------------------------------------------------

	// Draw the main weather results card.
	ofSetColor(8, 14, 24, 145);

	ofDrawRectRounded(
		resultsX,
		resultsY,
		resultsW,
		resultsH,
		20.0f);

	// Draw a subtle white border around the results card.
	ofNoFill();

	ofSetColor(255, 255, 255, 90);

	ofDrawRectRounded(
		resultsX,
		resultsY,
		resultsW,
		resultsH,
		20.0f);

	ofFill();

	// ------------------------------------------------------------------------
	// WEATHER TEXT
	// ------------------------------------------------------------------------

	// Starting position for the weather information.
	const float textX = resultsX + 30.0f;

	float currentY = resultsY + 46.0f;

	const float lineSpacing = 60.0f;

	// Display the "LOCATION" heading.
	ofSetColor(255);

	smallFont.drawString(
		"LOCATION",
		textX,
		currentY);

	currentY += 31.0f;

	// Combine the city and country into one string.
	std::string locationText = cityName + ", " + country;

	// Use the larger body font by default.
	ofTrueTypeFont * locationFont = &bodyFont;

	// Check whether the location text is too wide
	// for the results card.
	//
	// If it is too long, use the smaller font to prevent
	// the text from going outside the panel.
	if (
		bodyFont.getStringBoundingBox(
					locationText,
					0,
					0)
			.width
		> resultsW - 60.0f) {

		locationFont = &smallFont;
	}

	// Draw the city and country.
	locationFont->drawString(
		locationText,
		textX,
		currentY);

	currentY += 54.0f;

	// Display the weather condition.
	bodyFont.drawString(
		"Condition: " + conditionText,
		textX,
		currentY);

	currentY += lineSpacing;

	// Display the current temperature.
	//
	// ofToString(tempC, 1) converts the float into text
	// with one decimal place.
	bodyFont.drawString(
		"Temperature: " + ofToString(tempC, 1) + " C",
		textX,
		currentY);

	currentY += lineSpacing;

	// Display the "feels like" temperature.
	bodyFont.drawString(
		"Feels Like: " + ofToString(feelsLikeC, 1) + " C",
		textX,
		currentY);

	currentY += lineSpacing;

	// Display the relative humidity percentage.
	bodyFont.drawString(
		"Humidity: " + ofToString(humidity) + "%",
		textX,
		currentY);

	// ========================================================================
	// STATUS MESSAGE
	// ========================================================================

	// Position and size of the status message at the bottom.
	const float statusX = resultsX;
	const float statusY = 586.0f;

	const float statusW = resultsW;
	const float statusH = 62.0f;

	// Start with green to represent the normal/success state.
	ofColor statusColor(
		25,
		115,
		70,
		205);

	// If an API request is currently running,
	// change the status colour to orange.
	if (isLoading) {

		statusColor = ofColor(
			205,
			125,
			35,
			215);
	}

	// If an error occurred, use red.
	if (hasError) {

		statusColor = ofColor(
			170,
			45,
			50,
			215);
	}

	// Draw the rounded status background.
	ofSetColor(statusColor);

	ofDrawRectRounded(
		statusX,
		statusY,
		statusW,
		statusH,
		16.0f);

	// Draw a subtle border around the status box.
	ofNoFill();

	ofSetColor(
		255,
		255,
		255,
		95);

	ofDrawRectRounded(
		statusX,
		statusY,
		statusW,
		statusH,
		16.0f);

	ofFill();

	// Combine the "Status:" label with the current message.
	const std::string fullStatus = "Status: " + statusMessage;

	// Display the status message to the user.
	ofSetColor(255);

	smallFont.drawString(
		fullStatus,
		statusX + 20.0f,
		statusY + 39.0f);
}
