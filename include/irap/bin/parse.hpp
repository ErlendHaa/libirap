#ifndef LIBIRAP_BIN_PARSE_HPP
#define LIBIRAP_BIN_PARSE_HPP

#include <cstddef>
#include <cstdint>

/** Low level, zero-allocation parsing routines for binary irap
 */

namespace irap { namespace bin {

/** \defgroup bin-parse Binary IRAP parsing */
/** \addtogroup bin-parse
 * @{
 */

/** Size of the header record 1
 *
 * Size *does not* include leading- and trailing record length (4 bytes each).
 */
static constexpr const std::size_t RECORD_1_SIZE = 32;

/** Size of the header record 2
 *
 * Size *does not* include leading- and trailing record length (4 bytes each).
 */
static constexpr const std::size_t RECORD_2_SIZE = 16;

/** Size of the header record 3
 *
 * Size *does not* include leading- and trailing record length (4 bytes each).
 */
static constexpr const std::size_t RECORD_3_SIZE = 28;

/** Total size of the IRAP binary header
 *
 * Size of all the 3 fortran records that constitute the full header. Including each
 * records leading- and trailing lengths.
 */
static constexpr const std::size_t HEADER_SIZE =
    RECORD_1_SIZE + 8 +
    RECORD_2_SIZE + 8 +
    RECORD_3_SIZE + 8
;

/** Binary IRAP header */
struct Header {
    /** Binary IRAP ID flag */
    std::int32_t idflag;

    /** Number of samples in Y direction */
    std::int32_t ny;

    /** X-coordinate of origin */
    float xori;

    /** Max X-coordinate */
    float xmax;

    /** Y-coordinate of origin */
    float yori;

    /** Max Y-coordinate */
    float ymax;

    /** X increment */
    float xinc;

    /** Y increment */
    float yinc;

    /** Number of samples in Y direction */
    std::int32_t nx;

    /** Rotation */
    float rot;

    /** X-coordinate of origin */
    float x0ori;

    /** Y-coordinate of origin */
    float y0ori;
};

/** @} */

/* Primitive type parsing is mainly inteded for internal use, but can provide
 * useful for applications as well so we make them public, but slightly hidden.
 */
namespace detail {

// Primitive type parsers
void const* read_float(void const* src, float&        out) noexcept (true);
void const* read_int32(void const* src, std::int32_t& out) noexcept (true);

void const* write_float(void const* dst, float        x) noexcept (true);
void const* write_int32(void const* dst, std::int32_t x) noexcept (true);


// H consteader record parsers
void const* parse_record_1(void const* src, Header& header) noexcept (false);
void const* parse_record_2(void const* src, Header& header) noexcept (false);
void const* parse_record_3(void const* src, Header& header) noexcept (false);

} // namespace detail

/** \addtogroup bin-parse
 * @{
 */


/** Calculate number of elements in the irap map
 *
 * Use this to allocate appropriate dst buffers for reading the map into.
 * Essentially this is header.nx * header.ny.
 *
 * \param header Binary Header for the file
 *
 * \retval Number of element in the irap map
 */
std::size_t map_size(Header const& header) noexcept (true);


/** Read IRAP binary header from `src`
 *
 * \param src    buffer to parse the header from. The buffer must be _atleast_
 *               `HEADER_SIZE` bytes
 * \param size   size of the `src` buffer
 * \param header `header` instance to write into
 *
 * \retval First byte in the `src` buffer after the parsed header.
 */
void const* read_header(
    void const* src,
    std::size_t size,
    Header & header
) noexcept (false);


/** Write IRAP binary header to `dst`
 *
 * \param dst    buffer to write the header to. The buffer must be _atleast_
 *               `HEADER_SIZE` bytes
 * \param size   size of the `dst` buffer
 * \param header `header` to write
 *
 * \retval First byte in the `dst` buffer after the written header.
 */
void* write_header(
    void const* dst,
    std::size_t size,
    Header const& header
) noexcept (false);


/** Read IRAP map from src into dst
 *
 * Parses up to `min(src_size, dst_size)` bytes. The IRAP binary map is
 * F-order and `read_map` does *not* convert the map to C-order. See
 * corder_map().
 *
 * It's completly valid to read the map in chunks, e.g. for streaming. The
 * caller must manually advance the buffer pointer between invocations.
 *
 * \param src      Buffer to parse IRAP map from
 * \param src_size Size of the `src`-buffer
 * \param src      Buffer to write IRAP map to
 * \param dst_size Size of the `dst`-buffer
 *
 * \retval Number of bytes read
 */
std::size_t read_map(
    void const* src,
    std::size_t src_size,
    void*       dst,
    std::size_t dst_size
) noexcept (false);


/** Write IRAP map to dst
 *
 * Writes up to `min(src_size, dst_size)` bytes. This function expects the
 * F-ordering in the src buffer. If your data is C-ordered, see
 * forder_map().
 *
 * \param src      Buffer to the IRAP map
 * \param src_size Size of the `src`-buffer
 * \param src      Buffer to write IRAP map to
 * \param dst_size Size of the `dst`-buffer
 *
 * \retval Number of bytes read
 */
std::size_t write_map(
    void const* src,
    std::size_t src_size,
    void*       dst,
    std::size_t dst_size
) noexcept (false);

/** Inplace conversion from F-order to C-order
 *
 * \param map      Pointer to the map
 * \param src_size Size of the map
 * \param header   File header
 */
void corder_map(
    void*         map,
    std::size_t   size,
    Header const& header
) noexcept (false);

/** Inplace conversion from C-order to F-order
 *
 * \param map      Pointer to the map
 * \param src_size Size of the map
 * \param header   File header
 */
void forder_map(
    void*         map,
    std::size_t   size,
    Header const& header
) noexcept (false);

/** @} */

} // namespace bin

} // namespace irap

#endif // LIBIRAP_BIN_PARSE_HPP
