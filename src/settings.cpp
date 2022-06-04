#include <fstream>
#include <iostream>

#include "settings.h"

FileLocationService::FileLocationService()
{
	if (const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME"))
		configFolder = std::filesystem::path(xdg_config_home) / "honmonogatari";
	else if (const char* home = std::getenv("HOME"))
		configFolder = std::filesystem::path(home) / ".config" / "honmonogatari";
	else
		configFolder = "./config";

	if (const char* xdg_data_home = std::getenv("XDG_DATA_HOME"))
		dataFolder = std::filesystem::path(xdg_data_home) / "honmonogatari";
	else if (const char* home = std::getenv("HOME"))
		dataFolder = std::filesystem::path(home) / ".local" / "share" / "honmonogatari";
	else
		dataFolder = "./honmonogatari";
}

StyleContext::StyleContext(std::filesystem::path path) : styleFolder(path)
{
	current_style = &ImGui::GetStyle();

	std::filesystem::create_directories(path);
	if (std::filesystem::is_regular_file(path / "default.style"))
		loadStyleFile("default");
	else
	{
		current_style->WindowRounding = 0.0f;
		current_style->Colors[ImGuiCol_WindowBg].w = 1.0f;
		current_style->Colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.188f, 0.25f, 0.25f, 1.0f);
	}
}

bool StyleContext::isStyleFile(const std::string& name)
{
	return std::filesystem::is_regular_file(styleFolder / (name + ".style"));
}

void StyleContext::loadStyleFile(const std::string& name)
{
	std::filesystem::path path = styleFolder / (name + ".style");

	if (!std::filesystem::is_regular_file(path))
		throw FileException("Style file <", path, "> does not exist.");

	std::ifstream input(path, std::ifstream::binary | std::ifstream::ate);

	if (input.tellg() != 0x450)
		throw FileException("Style file <", path, "> has length of ",
				std::to_string(input.tellg()), " instead of 1104.");
	input.seekg(0);

	char header[0x10 + 1] = {};
	input.read(header, 0x10);
	if (std::string("honmonostyle 1.0").compare(header))
		throw FileException("Style file <", path, "> has a incorrect header.");

	unsigned int value;

	input.read(reinterpret_cast<char*>(&current_style->Alpha), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->DisabledAlpha), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->WindowPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->WindowRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->WindowBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->WindowMinSize), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->WindowTitleAlign), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->WindowMenuButtonPosition), sizeof(ImGuiDir));
	input.read(reinterpret_cast<char*>(&current_style->ChildRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->ChildBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->PopupRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->PopupBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->FramePadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->FrameRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->FrameBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->ItemSpacing), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->ItemInnerSpacing), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->CellPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->TouchExtraPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->IndentSpacing), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->ColumnsMinSpacing), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->ScrollbarSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->ScrollbarRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->GrabMinSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->GrabRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->LogSliderDeadzone), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->TabRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->TabBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->TabMinWidthForCloseButton), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->ColorButtonPosition), sizeof(ImGuiDir));
	input.read(reinterpret_cast<char*>(&current_style->ButtonTextAlign), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->SelectableTextAlign), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->DisplayWindowPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->DisplaySafeAreaPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&current_style->MouseCursorScale), sizeof(float));

	input.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
	current_style->AntiAliasedLines = value != 0;
	input.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
	current_style->AntiAliasedLinesUseTex = value != 0;
	input.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
	current_style->AntiAliasedFill = value != 0;

	input.read(reinterpret_cast<char*>(&current_style->CurveTessellationTol), sizeof(float));
	input.read(reinterpret_cast<char*>(&current_style->CircleTessellationMaxError), sizeof(float));

	for (std::size_t i = 0; i < ImGuiCol_COUNT; ++i)
		input.read(reinterpret_cast<char*>(&current_style->Colors[i]), sizeof(ImVec4));
}

void StyleContext::saveStyleFile(const std::string& name)
{
	unsigned int value;
	std::ofstream output(styleFolder / (name + ".style"), std::ifstream::binary | std::ofstream::trunc);

	output << "honmonostyle 1.0" << std::flush;
	output.write(reinterpret_cast<const char*>(&current_style->Alpha), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->DisabledAlpha), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->WindowPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->WindowRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->WindowBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->WindowMinSize), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->WindowTitleAlign), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->WindowMenuButtonPosition), sizeof(ImGuiDir));
	output.write(reinterpret_cast<const char*>(&current_style->ChildRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->ChildBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->PopupRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->PopupBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->FramePadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->FrameRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->FrameBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->ItemSpacing), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->ItemInnerSpacing), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->CellPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->TouchExtraPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->IndentSpacing), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->ColumnsMinSpacing), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->ScrollbarSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->ScrollbarRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->GrabMinSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->GrabRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->LogSliderDeadzone), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->TabRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->TabBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->TabMinWidthForCloseButton), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->ColorButtonPosition), sizeof(ImGuiDir));
	output.write(reinterpret_cast<const char*>(&current_style->ButtonTextAlign), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->SelectableTextAlign), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->DisplayWindowPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->DisplaySafeAreaPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&current_style->MouseCursorScale), sizeof(float));

	if (current_style->AntiAliasedLines)
		value = 0 - 1;
	else
		value = 0;
	output.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));
	if (current_style->AntiAliasedLinesUseTex)
		value = 0 - 1;
	else
		value = 0;
	output.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));
	if (current_style->AntiAliasedFill)
		value = 0 - 1;
	else
		value = 0;
	output.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));

	output.write(reinterpret_cast<const char*>(&current_style->CurveTessellationTol), sizeof(float));
	output.write(reinterpret_cast<const char*>(&current_style->CircleTessellationMaxError), sizeof(float));

	for (std::size_t i = 0; i < ImGuiCol_COUNT; ++i)
		output.write(reinterpret_cast<const char*>(&current_style->Colors[i]), sizeof(ImVec4));
}
