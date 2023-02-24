#define CATCH_CONFIG_MAIN

#include <catch/catch.hpp>

#include <irap/bin/parse.hpp>
#include <irap/exception.hpp>


TEST_CASE("Reading the header", "[bin][parse]") {
    auto const buffer = std::vector< unsigned char > {
        // Record 1
        0x00, 0x00, 0x00, 0x20, 0xff, 0xff, 0xfc, 0x1c,
        0x00, 0x00, 0x02, 0xd0, 0x48, 0xd6, 0x04, 0x97,
        0x48, 0xd8, 0x72, 0x77, 0x4a, 0xc5, 0x9c, 0x8e,
        0x4a, 0xc5, 0xe2, 0xc5, 0x41, 0x48, 0x00, 0x00,
        0x41, 0x47, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x20,
        // Record 2
        0x00, 0x00, 0x00, 0x10, 0x00, 0x00, 0x01, 0x8f,
        0x42, 0x97, 0xff, 0x24, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10,
        // Record 3
        0x00, 0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x04,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x01, 0x00, 0x00, 0x00, 0x01,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x1c
    };

    SECTION("Header is correctly parsed") {
        irap::bin::Header header;
        irap::bin::read_header(buffer.data(), buffer.size(), header);

        // Record 1 (in order of apperance in file)
        CHECK( header.ny   == 720 );
        CHECK( header.xinc == Approx(      12.5      ).epsilon(0.001) );
        CHECK( header.yinc == Approx(      12.5      ).epsilon(0.001) );
        CHECK( header.xori == Approx(  438308.727184 ).epsilon(0.001) );
        CHECK( header.xmax == Approx(  443283.727178 ).epsilon(0.001) );
        CHECK( header.yori == Approx( 6475335.099521 ).epsilon(0.001) );
        CHECK( header.ymax == Approx( 6484322.597848 ).epsilon(0.001) );

        // Record 2 (in order of apperance in file)
        CHECK( header.nx    == 399 );
        CHECK( header.rot   == Approx( 75.99 ).epsilon(0.001) );
        CHECK( header.x0ori == Approx(  0.00  ).epsilon(0.001) );
        CHECK( header.y0ori == Approx(  0.00  ).epsilon(0.001) );
    }

    SECTION("Pointer is advanced correctly") {
        auto expected = buffer.data() + irap::bin::HEADER_SIZE;

        irap::bin::Header header;
        auto cur = irap::bin::read_header(buffer.data(), buffer.size(), header);

        CHECK((unsigned char*)cur == expected);
    }

    SECTION("Buffer size is correctly checked") {
        irap::bin::Header header;

        CHECK_THROWS_AS(
            irap::bin::read_header(buffer.data(), buffer.size() - 1, header),
            irap::invalid_args
        );
    }
}
