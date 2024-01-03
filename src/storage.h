#ifndef STORAGE_H
#define STORAGE_H

#include <filesystem>
#include <vector>
#include <string>



// Forward declarations for the library tinyxml2 to avoid importing it unnecessarily
namespace tinyxml2
{
	class XMLDocument;
	class XMLElement;
} // namespace tinyxml2

/**
 * @brief Namespace for all the file handling and data wrangling classes and functions.
 */
namespace storage
{
	/**
	 * @brief This class represents a book inside the library
	 */
	class LibraryBook
	{
		friend class LibraryShelf;

	public:
		/**
		 * @brief Construct a new empty library book
		 */
		LibraryBook() : name("<untitled>"), location("") { }
		/**
		 * @brief Parses a new library book from an XMLElement
		 *
		 * @param xmlElement The XMLElement to gather the relevant data from
		 */
		LibraryBook(tinyxml2::XMLElement* xmlElement);
		/**
		 * @brief Construct a new library book with given values
		 *
		 * @param _name The name of the new library book
		 * @param _location The location of the new library book
		 */
		LibraryBook(std::string _name, std::string _location = "") : name(_name), location(_location) { }

		/**
		 * @brief Get the name of a library book
		 *
		 * @return std::string& Provides a reference to the library book name
		 */
		std::string& getName() { return name; }
		/**
		 * @brief Get the location of a library book
		 *
		 * @return std::string& Provides a reference to the library book location
		 */
		std::string& getLocation() { return location; }

	private:
		/**
		 * @brief Serializes the library book into a XMLElement
		 *
		 * @param document The document which the XMLElement will belong to
		 * @return tinyxml2::XMLElement* Returns a pointer to the new XMLElement
		 */
		tinyxml2::XMLElement* serialize(tinyxml2::XMLDocument* document);

	private:
		/// @brief The name of the library book
		std::string name;
		/// @brief The location of the library book
		std::string location;
	};

	/**
	 * @brief This class represents a shelf inside the library.
	 *
	 * A shelf in the library is simply a collection of books. It is important
	 * that shelf's can contain other shelf's as well.
	 */
	class LibraryShelf
	{
		friend class Library;

	public:
		/**
		 * @brief Construct a new empty library shelf
		 */
		LibraryShelf() : name("<unnamed>") { }
		/**
		 * @brief Parses a new library shelf from an XMLElement
		 *
		 * @param xmlElement The XMLElement to gather the relevant data from
		 */
		LibraryShelf(tinyxml2::XMLElement* xmlElement);
		/**
		 * @brief Construct a new library shelf with given values
		 *
		 * @param _name The name of the new library shelf
		 */
		LibraryShelf(std::string_view _name) : name(_name) { }

		/**
		 * @brief Get the name of a library shelf
		 *
		 * @return std::string& Provides a reference to the library shelf name
		 */
		std::string& getName() { return name; }

		/**
		 * @brief Delete a library shelf from the subshelfs
		 *
		 * This deletes the shelf with all its library books and subshelfs.
		 *
		 * @param shelf A pointer to the library shelf to delete
		 * @return true If the deletion was successful
		 * @return false If no matching item was found
		 */
		bool deleteShelf(LibraryShelf* const shelf);
		/**
		 * @brief Delete a library book from this shelf or its subshelfs
		 *
		 * @param shelf A pointer to the library book to delete
		 * @return true If the deletion was successful
		 * @return false If no matching item was found
		 */
		bool deleteBook(LibraryBook* const book);

		/**
		 * @brief Returns an iterator to the beginning of the books inside the shelf
		 *
		 * The iterator points to the first book inside this shelf. If the vector
		 * is empty, the returned iterator will be equal to end().
		 *
		 * @return Iterator to the first element
		 */
		std::vector<LibraryBook>::iterator begin() { return books.begin(); }
		/**
		 * @brief Returns an iterator to the end of the books inside the shelf
		 *
		 * The iterator points past the last book inside the shelf. This element
		 * acts as a placeholder; attempting to access it results in undefined
		 * behavior.
		 *
		 * @return Iterator to the element following the last element.
		 */
		std::vector<LibraryBook>::iterator end() { return books.end(); }

	private:
		/**
		 * @brief Serializes the library shelf into a XMLElement
		 *
		 * @param document The document which the XMLElement will belong to
		 * @return tinyxml2::XMLElement* Returns a pointer to the new XMLElement
		 */
		tinyxml2::XMLElement* serialize(tinyxml2::XMLDocument* document);

	public:
		/// @brief A list of shelf's contained inside this shelf
		std::vector<LibraryShelf> subshelfs;
		/// @brief A list of books contained inside this shelf
		std::vector<LibraryBook> books;

	private:
		/// @brief The name of the library shelf
		std::string name;
	};

	/**
	 * @brief This class represents a collection of shelf's
	 */
	class Library
	{
	public:
		/**
		 * @brief Construct a new default library object
		 */
		Library();
		/**
		 * @brief Parse a library object from a xml library file
		 *
		 * @param path The path to the file
		 */
		Library(std::filesystem::path path);

		/**
		 * @brief Get the owner of a library shelf
		 *
		 * @return Provides a reference to the library owner
		 */
		std::string& getOwner() { return owner; }

		/**
		 * @brief Delete a library shelf from the library
		 *
		 * This deletes the shelf with all its library books and subshelfs.
		 *
		 * @param shelf A pointer to the library shelf to delete
		 * @return true If the deletion was successful
		 * @return false If no matching item was found
		 */
		bool deleteShelf(LibraryShelf* const shelf);
		/**
		 * @brief Delete a library book from the library
		 *
		 * @param shelf A pointer to the library book to delete
		 * @return true If the deletion was successful
		 * @return false If no matching item was found
		 */
		bool deleteBook(LibraryBook* const book);

		/**
		 * @brief Returns an iterator to the beginning of the shelfs inside the library
		 *
		 * The iterator points to the first shelf inside this library. If the vector
		 * is empty, the returned iterator will be equal to end().
		 *
		 * @return Iterator to the first element
		 */
		std::vector<LibraryShelf>::iterator begin() { return shelfs.begin(); }
		/**
		 * @brief Returns an iterator to the end of the shelf's inside the library
		 *
		 * The iterator points past the last shelf inside the library. This element
		 * acts as a placeholder; attempting to access it results in undefined
		 * behavior.
		 *
		 * @return Iterator to the element following the last element.
		 */
		std::vector<LibraryShelf>::iterator end() { return shelfs.end(); }

		/**
		 * @brief Save the library to the path it was loaded from
		 *
		 * This function only works if the library was parsed from a file.
		 * Otherwise it will silently fail.
		 */
		inline void save() { save(library_path); }
		/**
		 * @brief Save the library to the given file path
		 *
		 * @param path The path to save the library to
		 */
		void save(const std::filesystem::path& path);

	private:
		/**
		 * @brief Serializes the library into a XMLElement
		 *
		 * @param document The document which the XMLElement will belong to
		 * @return tinyxml2::XMLElement* Returns a pointer to the new XMLElement
		 */
		tinyxml2::XMLElement* serialize(tinyxml2::XMLDocument* document);

		/// @brief The path where the library was loaded from
		std::filesystem::path library_path;

	public:
		/// @brief A list of shells contained in this library
		std::vector<LibraryShelf> shelfs;

	private:
		/// @brief The owner of this library
		std::string owner;
	};
} // namespace storage

#endif // STORAGE_H
