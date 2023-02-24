#ifndef LIBIRAP_EXCEPTION_HPP
#define LIBIRAP_EXCEPTION_HPP

#include <cstring>
#include <stdexcept>
#include <string>


namespace irap {

/** \defgroup exception API exceptions */
/** \addtogroup exception
 * @{
 */

/** Returned when the parsed file doesn't match expectations */
struct parse_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/** Returned when functionality is not implemented */
struct not_implemented : public std::logic_error {
    using std::logic_error::logic_error;
    explicit not_implemented( const std::string& msg ) :
        logic_error( "Not implemented yet: " + msg )
    {}
};

/** Returned whenever an invalid argument is passed to the function */
struct invalid_args : public std::runtime_error {
    using std::runtime_error::runtime_error;
};

/** Returned when there is a problem a phyiscal device
 *
 * Depending on the device this might be recoverable, but it means a read or
 * write operation could not be performed correctly.
 */
struct io_error : public std::runtime_error {
    using std::runtime_error::runtime_error;
    explicit io_error( int no ) : runtime_error( std::strerror( no ) ) {}
};

/** @} */

} // namespace irap

#endif //LIBIRAP_EXCEPTION_HPP
