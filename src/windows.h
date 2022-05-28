#ifndef WINDOWS_H
#define WINDOWS_H

#include "storage.h"

namespace graphics
{
	class LibraryWindow
	{
	public:
		LibraryWindow(storage::Library* _library, bool _active = false) :
				active(_active), library(_library), currentShelf(nullptr),
				currentBook(nullptr) { }

		bool& isActive() { return active; }

		void show();
		bool showWindowMenuEntry();

	private:
		void renderMenuBar();
		void renderLibrary(storage::Library* library);
		void renderShelf(storage::LibraryShelf& shelf);
		void renderBook(storage::LibraryBook& book);

		bool active;

		storage::Library* const library;
		storage::LibraryShelf* currentShelf;
		storage::LibraryBook* currentBook;
	};
} // namespace graphics

#endif // WINDOWS_H
