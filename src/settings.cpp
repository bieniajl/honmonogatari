#include <cmath>
#include <fstream>
#include <iostream>

#include "settings.h"

std::filesystem::path getConfigPath()
{
	if (const char* xdg_config_home = std::getenv("XDG_CONFIG_HOME"))
		return std::filesystem::path(xdg_config_home) / "honmonogatari";

	if (const char* home = std::getenv("HOME"))
		return std::filesystem::path(home) / ".config" / "honmonogatari";

	return "./honmonogatari";
}

std::filesystem::path getSharePath()
{
#ifdef DEBUG
	return "./res";
#else
	if (std::filesystem::is_directory("/usr/share/honmonogatari"))
		return "/usr/share/honmonogatari";
	if (std::filesystem::is_directory("/usr/local/share/honmonogatari"))
		return "/usr/local/share/honmonogatari";
	return "./res";
#endif
}

ImGuiStyle readStyleFile(const std::filesystem::path& path)
{
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
	ImGuiStyle style;

	input.read(reinterpret_cast<char*>(&style.Alpha), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.DisabledAlpha), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.WindowPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.WindowRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.WindowBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.WindowMinSize), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.WindowTitleAlign), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.WindowMenuButtonPosition), sizeof(ImGuiDir));
	input.read(reinterpret_cast<char*>(&style.ChildRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.ChildBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.PopupRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.PopupBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.FramePadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.FrameRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.FrameBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.ItemSpacing), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.ItemInnerSpacing), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.CellPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.TouchExtraPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.IndentSpacing), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.ColumnsMinSpacing), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.ScrollbarSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.ScrollbarRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.GrabMinSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.GrabRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.LogSliderDeadzone), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.TabRounding), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.TabBorderSize), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.TabMinWidthForCloseButton), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.ColorButtonPosition), sizeof(ImGuiDir));
	input.read(reinterpret_cast<char*>(&style.ButtonTextAlign), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.SelectableTextAlign), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.DisplayWindowPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.DisplaySafeAreaPadding), sizeof(ImVec2));
	input.read(reinterpret_cast<char*>(&style.MouseCursorScale), sizeof(float));

	input.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
	style.AntiAliasedLines = value != 0;
	input.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
	style.AntiAliasedLinesUseTex = value != 0;
	input.read(reinterpret_cast<char*>(&value), sizeof(unsigned int));
	style.AntiAliasedFill = value != 0;

	input.read(reinterpret_cast<char*>(&style.CurveTessellationTol), sizeof(float));
	input.read(reinterpret_cast<char*>(&style.CircleTessellationMaxError), sizeof(float));

	for (std::size_t i = 0; i < ImGuiCol_COUNT; ++i)
		input.read(reinterpret_cast<char*>(&style.Colors[i]), sizeof(ImVec4));

	return style;
}

void writeStyleFile(const std::filesystem::path& path, const ImGuiStyle& style)
{
	unsigned int value;
	std::ofstream output(path, std::ifstream::binary | std::ofstream::trunc);

	output << "honmonostyle 1.0" << std::flush;
	output.write(reinterpret_cast<const char*>(&style.Alpha), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.DisabledAlpha), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.WindowPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.WindowRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.WindowBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.WindowMinSize), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.WindowTitleAlign), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.WindowMenuButtonPosition), sizeof(ImGuiDir));
	output.write(reinterpret_cast<const char*>(&style.ChildRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.ChildBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.PopupRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.PopupBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.FramePadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.FrameRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.FrameBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.ItemSpacing), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.ItemInnerSpacing), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.CellPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.TouchExtraPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.IndentSpacing), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.ColumnsMinSpacing), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.ScrollbarSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.ScrollbarRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.GrabMinSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.GrabRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.LogSliderDeadzone), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.TabRounding), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.TabBorderSize), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.TabMinWidthForCloseButton), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.ColorButtonPosition), sizeof(ImGuiDir));
	output.write(reinterpret_cast<const char*>(&style.ButtonTextAlign), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.SelectableTextAlign), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.DisplayWindowPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.DisplaySafeAreaPadding), sizeof(ImVec2));
	output.write(reinterpret_cast<const char*>(&style.MouseCursorScale), sizeof(float));

	if (style.AntiAliasedLines)
		value = 0 - 1;
	else
		value = 0;
	output.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));
	if (style.AntiAliasedLinesUseTex)
		value = 0 - 1;
	else
		value = 0;
	output.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));
	if (style.AntiAliasedFill)
		value = 0 - 1;
	else
		value = 0;
	output.write(reinterpret_cast<const char*>(&value), sizeof(unsigned int));

	output.write(reinterpret_cast<const char*>(&style.CurveTessellationTol), sizeof(float));
	output.write(reinterpret_cast<const char*>(&style.CircleTessellationMaxError), sizeof(float));

	for (std::size_t i = 0; i < ImGuiCol_COUNT; ++i)
		output.write(reinterpret_cast<const char*>(&style.Colors[i]), sizeof(ImVec4));
}

StyleContext::StyleContext()
{
	std::filesystem::create_directories(getConfigPath() / "styles");

	if (std::filesystem::is_regular_file(getConfigPath() / "styles" / "default.style"))
	{
		loaded_style = readStyleFile(getConfigPath() / "styles" / "default.style");
		ImGui::GetStyle() = loaded_style;
	}
}

void StyleContext::ShowStyleEditorWindow(bool* p_open)
{
	if (ImGui::Begin("Style Editor", p_open))
	{
		if (ImGui::BeginPopupModal("Set Name", NULL, ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::Text("Please set the name of the current style:");
			ImGui::EndPopup();
		}

		ImGuiStyle& style = ImGui::GetStyle();
		ImGui::PushItemWidth(std::max(100.0f, ImGui::GetWindowWidth() - 190.0f));

		ImGui::Text("Style Editor Buttons go here for style and state management");

		ImGui::Separator();

		if (ImGui::BeginTabBar("##style_editor_tabs", ImGuiTabBarFlags_None))
		{
			if (ImGui::BeginTabItem("Sizes"))
			{
				ImGui::Text("Main");
				ImGui::SliderFloat2("WindowPadding", (float*)&style.WindowPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("FramePadding", (float*)&style.FramePadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("CellPadding", (float*)&style.CellPadding, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemSpacing", (float*)&style.ItemSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("ItemInnerSpacing", (float*)&style.ItemInnerSpacing, 0.0f, 20.0f, "%.0f");
				ImGui::SliderFloat2("TouchExtraPadding", (float*)&style.TouchExtraPadding, 0.0f, 10.0f, "%.0f");
				ImGui::SliderFloat("IndentSpacing", &style.IndentSpacing, 0.0f, 30.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarSize", &style.ScrollbarSize, 1.0f, 20.0f, "%.0f");
				ImGui::SliderFloat("GrabMinSize", &style.GrabMinSize, 1.0f, 20.0f, "%.0f");
				ImGui::Text("Borders");
				ImGui::SliderFloat("WindowBorderSize", &style.WindowBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("ChildBorderSize", &style.ChildBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("PopupBorderSize", &style.PopupBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("FrameBorderSize", &style.FrameBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::SliderFloat("TabBorderSize", &style.TabBorderSize, 0.0f, 1.0f, "%.0f");
				ImGui::Text("Rounding");
				ImGui::SliderFloat("WindowRounding", &style.WindowRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ChildRounding", &style.ChildRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("FrameRounding", &style.FrameRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("PopupRounding", &style.PopupRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("ScrollbarRounding", &style.ScrollbarRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("GrabRounding", &style.GrabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("LogSliderDeadzone", &style.LogSliderDeadzone, 0.0f, 12.0f, "%.0f");
				ImGui::SliderFloat("TabRounding", &style.TabRounding, 0.0f, 12.0f, "%.0f");
				ImGui::Text("Alignment");
				ImGui::SliderFloat2("WindowTitleAlign", (float*)&style.WindowTitleAlign, 0.0f, 1.0f, "%.2f");
				int window_menu_button_position = style.WindowMenuButtonPosition + 1;
				if (ImGui::Combo("WindowMenuButtonPosition", (int*)&window_menu_button_position, "None\0Left\0Right\0"))
					style.WindowMenuButtonPosition = window_menu_button_position - 1;
				ImGui::Combo("ColorButtonPosition", (int*)&style.ColorButtonPosition, "Left\0Right\0");
				ImGui::SliderFloat2("ButtonTextAlign", (float*)&style.ButtonTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::SliderFloat2("SelectableTextAlign", (float*)&style.SelectableTextAlign, 0.0f, 1.0f, "%.2f");
				ImGui::Text("Safe Area Padding");
				ImGui::SliderFloat2("DisplaySafeAreaPadding", (float*)&style.DisplaySafeAreaPadding, 0.0f, 30.0f, "%.0f");
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Colors"))
			{
				filter.Draw("Filter colors", ImGui::GetFontSize() * 16);

				ImGui::BeginChild("##maincolors", ImVec2(0, 0), true,
						ImGuiWindowFlags_AlwaysVerticalScrollbar
						| ImGuiWindowFlags_NavFlattened);
				ImGui::PushItemWidth(-160);

				for (std::size_t i = 0; i < ImGuiCol_COUNT; ++i)
				{
					if (!filter.PassFilter(ImGui::GetStyleColorName(i)))
						continue;

					ImGui::PushID(i);
					ImGui::ColorEdit4("##color", &style.Colors[i].x,
							ImGuiColorEditFlags_AlphaPreviewHalf
							| ImGuiColorEditFlags_AlphaBar);
					ImGui::SameLine(0.0f, style.ItemInnerSpacing.x);
					ImGui::TextUnformatted(ImGui::GetStyleColorName(i));
					ImGui::PopID();
				}

				ImGui::PopItemWidth();
				ImGui::EndChild();
				ImGui::EndTabItem();
			}

			if (ImGui::BeginTabItem("Rendering"))
			{
				ImGui::Checkbox("Anti-aliased lines", &style.AntiAliasedLines);
				ImGui::Checkbox("Anti-aliased lines use texture", &style.AntiAliasedLinesUseTex);
				ImGui::Checkbox("Anti-aliased fill", &style.AntiAliasedFill);

				ImGui::PushItemWidth(ImGui::GetFontSize() * 8);
				ImGui::DragFloat("Curve Tessellation Tolerance", &style.CurveTessellationTol, 0.02f, 0.10f, 10.0f, "%.2f");
				if (style.CurveTessellationTol < 0.10f) style.CurveTessellationTol = 0.10f;

				ImGui::DragFloat("Circle Tessellation Max Error", &style.CircleTessellationMaxError , 0.005f, 0.10f, 5.0f, "%.2f", ImGuiSliderFlags_AlwaysClamp);
				if (ImGui::IsItemActive())
				{
					ImGui::SetNextWindowPos(ImGui::GetCursorScreenPos());
					ImGui::BeginTooltip();
					ImGui::TextUnformatted("(R = radius, N = number of segments)");
					ImGui::Spacing();
					ImDrawList* draw_list = ImGui::GetWindowDrawList();
					const float min_widget_width = ImGui::CalcTextSize("N: MMM\nR: MMM").x;
					for (int n = 0; n < 8; n++)
					{
						const float RAD_MIN = 5.0f;
						const float RAD_MAX = 70.0f;
						const float rad = RAD_MIN + (RAD_MAX - RAD_MIN) * (float)n / (8.0f - 1.0f);

						ImGui::BeginGroup();

						ImGui::Text("R: %.f\nN: %d", rad, draw_list->_CalcCircleAutoSegmentCount(rad));

						const float canvas_width = std::max(min_widget_width, rad * 2.0f);
						const float offset_x     = std::floor(canvas_width * 0.5f);
						const float offset_y     = std::floor(RAD_MAX);

						const ImVec2 p1 = ImGui::GetCursorScreenPos();
						draw_list->AddCircle(ImVec2(p1.x + offset_x, p1.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
						ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

						const ImVec2 p2 = ImGui::GetCursorScreenPos();
						draw_list->AddCircleFilled(ImVec2(p2.x + offset_x, p2.y + offset_y), rad, ImGui::GetColorU32(ImGuiCol_Text));
						ImGui::Dummy(ImVec2(canvas_width, RAD_MAX * 2));

						ImGui::EndGroup();
						ImGui::SameLine();
					}
					ImGui::EndTooltip();
				}

				ImGui::DragFloat("Global Alpha", &style.Alpha, 0.005f, 0.20f, 1.0f, "%.2f");
				ImGui::DragFloat("Disabled Alpha", &style.DisabledAlpha, 0.005f, 0.0f, 1.0f, "%.2f");

				ImGui::PopItemWidth();
				ImGui::EndTabItem();
			}

			ImGui::EndTabBar();
		}

		ImGui::PopItemWidth();
	}
	ImGui::End();
}
