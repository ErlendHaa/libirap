#include <irap/bin/parse.hpp>
#include <irap/exception.hpp>

#include <endianness/endianness.h>

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <limits>
#include <string>
#include <type_traits>

namespace irap { namespace bin {

namespace {

#ifdef HOST_BIG_ENDIAN

template< typename T > T hton( T value ) noexcept { return value; }
template< typename T > T ntoh( T value ) noexcept { return value; }

#else

template< typename T >
typename std::enable_if< sizeof(T) == 4, T >::type
hton( T value ) noexcept {
    std::uint32_t v;
    std::memcpy( &v, &value, sizeof( T ) );
    v = bswap32( v );
    std::memcpy( &value, &v, sizeof( T ) );
    return value;
}

template< typename T >
T ntoh( T value ) noexcept {
    return hton( value );
}

#endif

} // namespace

namespace detail {

void const* read_float(void const* src, float* out) noexcept (true)
{
    static_assert(
        std::numeric_limits< float >::is_iec559 && sizeof( float ) == 4,
        "Function assumes IEEE 754 32-bit float"
    );

    std::memcpy( out, src, sizeof( float ) );
    *out = ntoh( *out );
    return (char*)src + sizeof( float );
}

void const* read_int32(void const* src, std::int32_t* out) noexcept (true)
{
    std::uint32_t x;
    std::memcpy( &x, src, sizeof( std::int32_t ) );
    x = ntoh( x );
    std::memcpy( out, &x, sizeof( std::int32_t ) );
    return (char*)src + sizeof( std::int32_t );
}

void* write_float(void* dst, float x) noexcept (true)
{
    static_assert(
        std::numeric_limits< float >::is_iec559 && sizeof( float ) == 4,
        "Function assumes IEEE 754 32-bit float"
    );

    x = hton( x );
    std::memcpy( dst, &x, sizeof( x ) );
    return (char*)dst + sizeof( x );
}

void* write_int32(void* dst, std::int32_t x) noexcept (true)
{
    x = hton( x );
    std::memcpy( dst, &x, sizeof( x ) );
    return (char*)dst + sizeof( x );
}

void const* parse_record_1(void const* src, Header& header) noexcept (false)
{
    std::int32_t head;
    src = read_int32(src, &head);
    if (head != RECORD_1_SIZE) {
        throw irap::parse_error(
            "Incorrect record size in header of record 1, expected: "
            + std::to_string(RECORD_1_SIZE)
            + ", was: "
            + std::to_string(head)
        );
    }
    src = read_int32( src, &header.idflag );
    src = read_int32( src, &header.ny     );
    src = read_float( src, &header.xori   );
    src = read_float( src, &header.xmax   );
    src = read_float( src, &header.yori   );
    src = read_float( src, &header.ymax   );
    src = read_float( src, &header.xinc   );
    src = read_float( src, &header.yinc   );

    std::int32_t foter;
    src = read_int32(src, &foter);
    if (foter != RECORD_1_SIZE) {
        throw irap::parse_error(
            "Incorrect record size in footer of record 1, expected: "
            + std::to_string(RECORD_1_SIZE)
            + ", was: "
            + std::to_string(foter)
        );
    }

    return src;
}

void const* parse_record_2(void const* src, Header& header) noexcept (false)
{
    std::int32_t head;
    src = read_int32(src, &head);
    if (head != RECORD_2_SIZE) {
        throw irap::parse_error(
            "Incorrect record size in header of record 2, expected: "
            + std::to_string(RECORD_2_SIZE)
            + ", was: "
            + std::to_string(head)
        );
    }

    src = read_int32( src, &header.nx    );
    src = read_float( src, &header.rot   );
    src = read_float( src, &header.x0ori );
    src = read_float( src, &header.y0ori );

    std::int32_t foter;
    src = read_int32(src, &foter);
    if (foter != RECORD_2_SIZE) {
        throw irap::parse_error(
            "Incorrect record size in footer of record 2, expected: "
            + std::to_string(RECORD_2_SIZE)
            + ", was: "
            + std::to_string(foter)
        );
    }

    return src;
}

void const* parse_record_3(void const* src, Header& header) noexcept (false)
{
    std::int32_t head;
    src = read_int32(src, &head);
    if (head != RECORD_3_SIZE) {
        throw irap::parse_error(
            "Incorrect record size in header of record 3, expected: "
            + std::to_string(RECORD_3_SIZE)
            + ", was: "
            + std::to_string(head)
        );
    }

    /* All the 7 integers in record 3 are undefined, at least as far as we know.
     * Until a definition or usecase arise these fields are completly ignored.
     */
    src = static_cast< unsigned char const* >(src) + (7 * sizeof(std::int32_t));

    std::int32_t foter;
    src = read_int32(src, &foter);
    if (foter != RECORD_3_SIZE) {
        throw irap::parse_error(
            "Incorrect record size in footer of record 3, expected: "
            + std::to_string(RECORD_3_SIZE)
            + ", was: "
            + std::to_string(foter)
        );
    }

    return src;
}

} // namespace detail

std::size_t map_size(Header const& header) noexcept (true)
{
    return header.nx * header.ny;
}

void const* read_header(
    void const* src,
    std::size_t size,
    Header & header
) noexcept (false)
{
    if (size < HEADER_SIZE) {
        throw irap::invalid_args(
            "Input buffer to small, need atleast "
            + std::to_string(HEADER_SIZE)
            + " bytes to parse the full header"
        );
    }

    src = detail::parse_record_1(src, header);
    src = detail::parse_record_2(src, header);
    src = detail::parse_record_3(src, header);

    return src;
}


void* write_header(
    void* dst,
    std::size_t size,
    Header const& header
) noexcept (false)
{
    throw irap::not_implemented("bin::write_header");
}

std::size_t read_map(
    void const* src,
    std::size_t src_size,
    void*       dst,
    std::size_t dst_size
) noexcept (false)
{
    throw irap::not_implemented("bin::read_map");
}

std::size_t write_map(
    void        const* src,
    std::size_t src_size,
    void*       dst,
    std::size_t dst_size
) noexcept (false)
{
    throw irap::not_implemented("bin::write_map");
}

void corder_map(
    void*         map,
    std::size_t   size,
    Header const& header
) noexcept (false)
{
    throw irap::not_implemented("bin::corder_map");
}

void to_forder(
    void*         map,
    std::size_t   size,
    Header const& header
) noexcept (false)
{
    throw irap::not_implemented("bin::forder_map");
}

} // namespace bin

} // namespace irap
