#include <gtest/gtest.h>

#include "networking/data_buffer_serialiser.h"
#include "networking/data_buffer_deserialiser.h"

TEST(data_buffer_seraliser_tests, constructor)
{
    eng::DataBufferSerialiser ser{ };

    ASSERT_TRUE(ser.data().empty());
}

TEST(data_buffer_seraliser_tests, 8bit_int)
{
    std::uint8_t val = 12u;

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<std::uint8_t>(), val);
}

TEST(data_buffer_seraliser_tests, 16bit_int)
{
    std::uint16_t val = 12345u;

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<std::uint16_t>(), val);
}

TEST(data_buffer_seraliser_tests, 32bit_int)
{
    std::uint32_t val = 123456789u;

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<std::uint32_t>(), val);
}

TEST(data_buffer_seraliser_tests, 64bit_int)
{
    std::uint64_t val = 12345678901233456u;

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<std::uint64_t>(), val);
}

TEST(data_buffer_seraliser_tests, enum)
{
    enum class Val : std::uint8_t
    {
        A = 12
    };

    auto val = Val::A;

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<Val>(), val);
}

TEST(data_buffer_seraliser_tests, real)
{
    eng::real val = 1.2345f;

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<eng::real>().value, val.value);
}

TEST(data_buffer_seraliser_tests, vector3)
{
    eng::Vector3 val{ 1.1f, 2.2f, 3.3f };

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<eng::Vector3>(), val);
}

TEST(data_buffer_seraliser_tests, quaternion)
{
    eng::Quaternion val{ 1.1f, 2.2f, 3.3f, 4.4f };

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<eng::Quaternion>(), val);
}

TEST(data_buffer_seraliser_tests, data_buffer)
{
    eng::DataBuffer val{
        static_cast<std::byte>(0x0),
        static_cast<std::byte>(0x1),
        static_cast<std::byte>(0x2)
    };

    eng::DataBufferSerialiser ser{ };

    ser.push(val);
    eng::DataBufferDeserialiser der{ ser.data() };

    ASSERT_EQ(der.pop<eng::DataBuffer>(), val);
}

TEST(data_buffer_seraliser_tests, complex)
{
    eng::DataBuffer val1{
        static_cast<std::byte>(0x0),
        static_cast<std::byte>(0x1),
        static_cast<std::byte>(0x2)
    };
    eng::Vector3 val2{ 1.1f, 2.2f, 3.3 };
    std::int32_t val3 = -4;

    eng::DataBufferSerialiser ser{ };

    ser.push(val1);
    ser.push(val2);
    ser.push(val3);
    eng::DataBufferDeserialiser der{ ser.data() };

    const auto &[pop1, pop2, pop3] = der.pop_tuple<eng::DataBuffer, eng::Vector3, std::int32_t>();
    ASSERT_EQ(pop1, val1);
    ASSERT_EQ(pop2, val2);
    ASSERT_EQ(pop3, val3);
}
