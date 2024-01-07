#pragma once
#include "nlohmann/json.hpp"
#include <iostream>
#include <fstream>
#include <Windows.h>
class Settings
{
private:
	const char* filename = "QuarmTool.xml";
public:
	std::string game_path="";


	template<typename X>
	void update(const char* key, X& value)
	{
		nlohmann::json jsonData;
		std::ifstream fileIn(filename);
		if (fileIn.is_open()) {
			fileIn >> jsonData;
			fileIn.close();
		}

		// Update or add the key-value pair
		jsonData[key] = value;

		// Write the updated JSON back to the file
		std::ofstream fileOut(filename);
		if (fileOut.is_open()) {
			fileOut << jsonData.dump(4); // 4 is the indentation level for better readability
			fileOut.close();
		}

	}

	template<typename X>
	X read(const char* key)
	{
		try {
			std::ifstream fileIn(filename);

			if (!fileIn.is_open()) {
				return X(); // Return a default-constructed X or handle the error accordingly
			}

			nlohmann::json jsonData;
			fileIn >> jsonData;

			fileIn.close();

			if (jsonData.find(key) != jsonData.end()) {
				return jsonData[key].get<X>();
			}
			return X(); // Return a default-constructed X if the key is not found
		}
		catch (const std::exception& e) {
			// Handle exceptions (e.g., JSON parsing errors)
			std::cerr << "Exception: " << e.what() << std::endl;
			return X(); // Return a default-constructed X or handle the error accordingly
		}
	}
	Settings();
	~Settings() {};
};
