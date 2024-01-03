#include <functional>

#include "storage.h"
#include "tinyxml2.h"
#include "exceptions.h"

storage::LibraryBook::LibraryBook(tinyxml2::XMLElement* xmlElement) : name("<untitled>"), location("")
{
	{ // Try to load the book name
		const char* attrib = xmlElement->Attribute("name");
		if (attrib != nullptr) name = attrib;
	}

	{ // Try to load the book location
		const char* attrib = xmlElement->Attribute("location");
		if (attrib != nullptr) location = attrib;
	}
}

tinyxml2::XMLElement* storage::LibraryBook::serialize(tinyxml2::XMLDocument* document)
{
	tinyxml2::XMLElement* xmlElement = document->NewElement("book");

	xmlElement->SetAttribute("name", name.c_str());
	xmlElement->SetAttribute("location", location.c_str());

	return xmlElement;
}

storage::LibraryShelf::LibraryShelf(tinyxml2::XMLElement* xmlElement) : name("<unnamed>")
{
	// Try to load the shelf name
	const char* attrib = xmlElement->Attribute("name");
	if (attrib != nullptr) name = attrib;

	// Parse all available sub shelf's
	for (tinyxml2::XMLElement* subShelf = xmlElement->FirstChildElement("shelf");
			subShelf != nullptr; subShelf = subShelf->NextSiblingElement("shelf"))
		subshelfs.emplace_back(subShelf);

	// Parse all available books
	for (tinyxml2::XMLElement* book = xmlElement->FirstChildElement("book");
			book != nullptr; book = book->NextSiblingElement("book"))
		books.emplace_back(book);
}

bool storage::LibraryShelf::deleteShelf(LibraryShelf* const shelf)
{
	for (auto it = subshelfs.begin(); it != subshelfs.end(); ++it)
	{
		if (&(*it) == shelf)
		{
			subshelfs.erase(it);
			return true;
		}
	}

	for (auto& subShelf : subshelfs)
		if (subShelf.deleteShelf(shelf))
			return true;

	return false;
}

bool storage::LibraryShelf::deleteBook(LibraryBook* const book)
{
	for (auto it = books.begin(); it != books.end(); ++it)
	{
		if (&(*it) == book)
		{
			books.erase(it);
			return true;
		}
	}

	for (auto& subShelf : subshelfs)
		if (subShelf.deleteBook(book))
			return true;

	return false;
}

tinyxml2::XMLElement* storage::LibraryShelf::serialize(tinyxml2::XMLDocument* document)
{
	tinyxml2::XMLElement* xmlElement = document->NewElement("shelf");

	xmlElement->SetAttribute("name", name.c_str());

	for (auto& subShelf : subshelfs)
		xmlElement->InsertEndChild(subShelf.serialize(document));
	for (auto& book : books)
		xmlElement->InsertEndChild(book.serialize(document));

	return xmlElement;
}

storage::Library::Library() : library_path(""), owner("unknown")
{
	shelfs.emplace_back("default");
	shelfs.back().books.emplace_back("New Book");
}

storage::Library::Library(std::filesystem::path path) : library_path(path), owner("unknown")
{
	tinyxml2::XMLDocument document;
	tinyxml2::XMLError load_error = document.LoadFile(path.c_str());

	// Error handling
	switch (load_error)
	{
	case tinyxml2::XML_ERROR_FILE_COULD_NOT_BE_OPENED:
		throw OpenError();
	case tinyxml2::XML_ERROR_FILE_READ_ERROR:
		throw ReadError();
	case tinyxml2::XML_ERROR_PARSING:
	case tinyxml2::XML_ERROR_PARSING_ELEMENT:
	case tinyxml2::XML_ERROR_PARSING_ATTRIBUTE:
	case tinyxml2::XML_ERROR_PARSING_TEXT:
	case tinyxml2::XML_ERROR_PARSING_CDATA:
	case tinyxml2::XML_ERROR_PARSING_COMMENT:
	case tinyxml2::XML_ERROR_PARSING_DECLARATION:
	case tinyxml2::XML_ERROR_PARSING_UNKNOWN:
		throw ParsingError();

	case tinyxml2::XML_ERROR_FILE_NOT_FOUND:
	case tinyxml2::XML_ERROR_EMPTY_DOCUMENT:
		{
			Library newLibrary;
			newLibrary.save(path);
		}
		if (document.LoadFile(path.c_str()) != tinyxml2::XML_SUCCESS)
			throw FileError("Error while loading the new library file: ", path);

	case tinyxml2::XML_SUCCESS:
		break;

	default:
		throw FileError("Unexpected error state while loading the library file: ", path);
	}

	// Try to get the root xml element for the library
	tinyxml2::XMLElement* xmlElement = document.FirstChildElement("library");
	if (xmlElement == nullptr) throw ParsingError("No valid library in library file: ", path);

	{ // Load the library owner
		const char* attrib = xmlElement->Attribute("owner");
		if (attrib != nullptr) owner = attrib;
	}

	// Parse all available shelfs
	for (tinyxml2::XMLElement* shelf = xmlElement->FirstChildElement("shelf");
			shelf != nullptr; shelf = shelf->NextSiblingElement("shelf"))
		shelfs.emplace_back(shelf);
}

bool storage::Library::deleteShelf(LibraryShelf* const shelf)
{
	for (auto& _shelf : shelfs)
		if (_shelf.deleteShelf(shelf))
			return true;
	return false;
}

bool storage::Library::deleteBook(LibraryBook* const book)
{
	for (auto& shelf : shelfs)
		if (shelf.deleteBook(book))
			return true;
	return false;
}

void storage::Library::save(const std::filesystem::path& path)
{
	tinyxml2::XMLDocument document;

	document.InsertFirstChild(document.NewDeclaration());
	document.InsertEndChild(serialize(&document));

	document.SaveFile(path.c_str());
}

tinyxml2::XMLElement* storage::Library::serialize(tinyxml2::XMLDocument* document)
{
	tinyxml2::XMLElement* xmlElement = document->NewElement("library");

	xmlElement->SetAttribute("owner", owner.c_str());

	for (auto& shelf : shelfs)
		xmlElement->InsertEndChild(shelf.serialize(document));

	return xmlElement;
}
