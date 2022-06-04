#ifndef SETTINGS_H
#define SETTINGS_H

#include <filesystem>

#include "graphics/imgui_tools.h"



/**
 * @brief File Exception used in the settings code.
 *
 * @todo Unify with storage::FileError
 * @deprecated Is in conflict with storage::FileError
 * @see storage::FileError
 */
class FileException : public std::runtime_error
{
public:
	template<typename... T>
	FileException(T... what) : runtime_error((std::string() += ... += what)) { }
};

/**
 * @brief The File Location Service is responsible for gathering the correct file paths.
 */
class FileLocationService
{
public:
	/**
	 * @brief Construct a new File Location Service object
	 */
	FileLocationService();

	/**
	 * @brief Get a config location path
	 *
	 * @param filename The name of the config file or folder
	 * @return std::filesystem::path to the correct config location
	 */
	std::filesystem::path getConfigLocation(std::string_view filename)
	{
		return configFolder / filename;
	}

	/**
	 * @brief Get a data location path
	 *
	 * @param filename The name of the data file or folder
	 * @return std::filesystem::path to the correct data location
	 */
	std::filesystem::path getDataLocation(std::string_view filename)
	{
		return dataFolder / filename;
	}

private:
	/// @brief The path to the config folder
	std::filesystem::path configFolder;
	/// @brief The path to the data folder
	std::filesystem::path dataFolder;
};

// Forward declaration
namespace graphics
{
	class StyleEditorWindow;
}

/**
 * @brief The style context handles all styling of the application
 */
class StyleContext
{
	friend class graphics::StyleEditorWindow;

public:
	/**
	 * @brief Construct a new style context
	 *
	 * @param path The location of the style configuration
	 */
	StyleContext(std::filesystem::path path);

	/**
	 * @brief Check if a specific style exists
	 *
	 * @param name The name of the style
	 * @return true If the style exists
	 * @return false If it does not
	 */
	bool isStyleFile(const std::string& name);
	/**
	 * @brief Load a specific style
	 *
	 * @param name The name of the style to load
	 */
	void loadStyleFile(const std::string& name);
	/**
	 * @brief Save a specific style
	 *
	 * @param name The name to save the style to
	 */
	void saveStyleFile(const std::string& name);

private:
	/// @brief The root folder where the styles are saved to
	std::filesystem::path styleFolder;

	/// @brief A pointer to the current styling of ImGui
	ImGuiStyle* current_style;
};

#endif // SETTINGS_H
