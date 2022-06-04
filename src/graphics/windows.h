#ifndef WINDOWS_H
#define WINDOWS_H

#include <list>

#include "TextEditor.h"
#include "storage.h"
#include "settings.h"



namespace graphics
{
	class StaticWindow
	{
		friend class ViewportRenderer;

	public:
		StaticWindow(bool active = false) : _active(active) { }

		operator bool() { return _active; }

		virtual std::string_view getName() { return "Unnamed Static Window"; }

		bool& isActive() { return _active; }

		virtual void render(bool* open = nullptr) = 0;

	private:
		bool _active;
	};

	class ViewportRenderer
	{
	public:
		ViewportRenderer() :
#ifdef DEBUG
				active_demo_window(false), active_stack_tool_window(false),
#endif
				active_metrics_window(false), active_about_window(false) { }

		void registerStaticWindow(StaticWindow* window) { staticWindows.push_back(window); }
		void registerSettingsWindow(StaticWindow* window) { settingsWindows.push_back(window); }

		void renderMainMenuBar();
		void renderWindows();

	private:
		std::list<StaticWindow*> staticWindows;
		std::list<StaticWindow*> settingsWindows;

#ifdef DEBUG
		bool active_demo_window;
		bool active_stack_tool_window;
#endif
		bool active_metrics_window;
		bool active_about_window;
	};

	class LibraryWindow : public StaticWindow
	{
	public:
		LibraryWindow(storage::Library* _library, bool active = false) :
				StaticWindow(active), library(_library), currentShelf(nullptr),
				currentBook(nullptr) { }

		std::string_view getName() { return "Library Viewer"; }
		void render(bool* open);

	private:
		void renderMenuBar();
		void renderLibrary(storage::Library* library);
		void renderShelf(storage::LibraryShelf& shelf);
		void renderBook(storage::LibraryBook& book);

		storage::Library* const library;
		storage::LibraryShelf* currentShelf;
		storage::LibraryBook* currentBook;
	};

	class StyleEditorWindow : public StaticWindow
	{
	public:
		StyleEditorWindow(StyleContext* context) : _context(context), style_index(-1)
#ifdef DEBUG
				, show_style_Editor(false)
#endif
				{ }

		std::string_view getName() { return "Style Editor"; }
		void render(bool* open);

	private:
		StyleContext* const _context;
		ImGuiTextFilter _filter;
		int style_index;

#ifdef DEBUG
		bool show_style_Editor;
#endif
	};

	/**
	 * @todo Remove me
	 */
	class EditorWindowTest : public StaticWindow
	{
	public:
		std::string_view getName() { return "Editor Test"; }
		void render(bool* open);

	private:
		TextEditor editor;
	};

	/**
	 * @todo Remove me
	 */
	class MarkdownWindowTest : public StaticWindow
	{
	public:
		std::string_view getName() { return "Markdown Test"; }
		void render(bool* open);
	};
} // namespace graphics

#endif // WINDOWS_H
