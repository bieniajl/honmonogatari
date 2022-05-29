#ifndef SETTINGS_H
#define SETTINGS_H

#include <stdexcept>
#include <filesystem>

#include "graphics/imgui_tools.h"


class FileException : public std::runtime_error
{
public:
	template<typename... T>
	FileException(T... what) : runtime_error((std::string() += ... += what)) { }
};

std::filesystem::path getConfigPath();
std::filesystem::path getSharePath();

ImGuiStyle readStyleFile(const std::filesystem::path& path);
void writeStyleFile(const std::filesystem::path& path, const ImGuiStyle& style);

class StyleContext
{
public:
	StyleContext();

	void ShowStyleEditorWindow(bool* p_open = nullptr);

private:
	ImGuiTextFilter filter;
	ImGuiStyle loaded_style;
};

#endif // SETTINGS_H
