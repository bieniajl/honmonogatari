#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#include <stdexcept>

namespace storage
{
	/**
	 * @brief Generall exception for errors occuring during file handling in any way.
	 */
	class FileError : public std::runtime_error
	{
		using std::runtime_error::runtime_error;

	public:
		/**
		 * @brief Construct a new file error exception
		 *
		 * This constructor allows easy construction of the what error message
		 * which is attached to the file error exception. All given parameters
		 * must be addable to a std::string. This constructor adds them all up
		 * in the given order and uses the resulting string as the what value
		 * for the underlying std::runtime_error.
		 *
		 * @tparam T Type of the parameter pack what
		 * @param what The values which are added up on a string
		 */
		template<typename... T>
		FileError(T... what) : std::runtime_error((std::string() += ... += what)) { }
	};

	/**
	 * @brief This exception is usually thrown when the system is unable to open a file.
	 */
	class OpenError : public FileError
	{
		using storage::FileError::FileError;
	};

	/**
	 * @brief This exception is usually thrown when any kind of error occurs while reading a file.
	 */
	class ReadError : public FileError
	{
		using storage::FileError::FileError;
	};

	/**
	 * @brief This exception is usually thrown when any kind of error occurs while parsing a file.
	 */
	class ParsingError : public FileError
	{
		using storage::FileError::FileError;
	};
} // namespace storage

#endif // EXCEPTIONS_H
