#pragma once

#include <cstdint>
#include <ios>
#include <sstream>
#include <string>
#include <variant>

#include "exception.hpp"
#include "function.hpp"
#include "location_data.hpp"
#include "object.hpp"

namespace eng
{

namespace detail
{

/**
 * Helper class which inherits from all supplied template types and uses their
 * operator() overloads. This allows us to simplify std::visit by supplying
 * lambdas with the required argument types, as a lambda has an operator()
 * this class can inherit from it.
 */
template<class... Ts>
struct overload : Ts...
{
    using Ts::operator()...;
};

/**
 * User defined deduction guide to tell the compiler how to correctly deduce
 * supplied lambda arguments as template types.
 */
template<class... Ts>
overload(Ts...) -> overload<Ts...>;

}

/**
 * This class stores any value supported by the scripting language and is
 * effectively how variables are represented in the virtual machine.
 */
class value
{
    public:

        // member types
        using variant_type = std::variant<float, std::string, bool, object*, function>;

        /**
         * Construct a new value object with the supplied object.
         *
         * @param value
         *   Object to store.
         */
        template<class T>
        explicit value(const T &value)
            : value_(value)
        { }

        // default
        value() = default;
        ~value() = default;
        value(const value&) = default;
        value& operator=(const value&) = default;
        value(value&&) = default;
        value& operator=(value&&) = default;

        /**
         * Get a reference to the stored value.
         *
         * Throws an exception if the requested type is incorrect.
         *
         * @returns
         *   Reference to stored value.
         */
        template<class T>
        T& get()
        {
            if(!is_type<T>())
            {
                throw exception("getting incorrect type");
            }

            return std::get<T>(value_);
        }

        /**
         * Get a const reference to the stored value.
         *
         * Throws an exception if the requested type is incorrect.
         *
         * @returns
         *   Const reference to stored value.
         */
        template<class T>
        const T& get() const
        {
            if(!is_type<T>())
            {
                throw exception("getting incorrect type");
            }

            return std::get<T>(value_);
        }

        /**
         * Set value.
         *
         * @param v
         *   Universal reference to value.
         */
        template<class T>
        void set(T &&v)
        {
            value_ = std::forward<T>(v);
        }

        /**
         * Check if type is currently being stored.
         *
         * @returns
         *   True if requested type is stored, false otherwise.
         */
        template<class T>
        bool is_type() const
        {
            return std::holds_alternative<T>(value_);
        }

        /**
         * Check if this value stores the same type as another.
         *
         * @param other
         *   Value to compare types with.
         *
         * @returns
         *   True if both values store the same type, false otherwise.
         */
        bool is_same(const value &other) const
        {
            return value_.index() == other.value_.index();
        }

        /**
         * Equality operator, checks if both values store the same thing. Throws
         * an exception if both values store different types.
         *
         * @param other
         *   Value to compare with.
         *
         * @returns True if both values store the same thing, false otherwise.
         */
        bool operator==(const value &other) const
        {
            if(!is_same(other))
            {
                throw exception("value type mismatch");
            }

            return value_ == other.value_;
        }

        /**
         * Inequality operator, checks if both values store different things.
         * Throws an exception if both values store different types.
         *
         * @param other
         *   Value to compare with.
         *
         * @returns True if both values store different things, false otherwise.
         */
        bool operator!=(const value &other) const
        {
            return !(*this == other);
        }

        /**
         * Less than operator, only supports float, bool and std::string. Throws
         * an exception if types mismatch or comparing unsupported types.
         *
         * @param other
         *   Value to compare with.
         *
         * @returns
         *   True if this value is less than the supplied one, false otherwise.
         */
        bool operator<(const value &other) const
        {
            if(!is_same(other))
            {
                throw exception("value type mismatch");
            }

            // handler for each supported type
            return std::visit(
                detail::overload{
                    [&other](float val) -> bool { return val < other.get<float>(); },
                    [&other](bool val) -> bool { return val < other.get<bool>(); },
                    [&other](std::string val) -> bool { return val < other.get<std::string>(); },
                    [](object*) -> bool { throw exception("cannot compare objects"); },
                    [](function) -> bool { throw exception("cannot compare functions"); },
                },
                value_);
        }

        /**
         * More than operator, only supports float, bool and std::string. Throws
         * an exception if types mismatch or comparing unsupported types.
         *
         * @param other
         *   Value to compare with.
         *
         * @returns
         *   True if this value is more than the supplied one, false otherwise.
         */
        bool operator>(const value &other) const
        {
            return other < *this;
        }

        /**
         * More than or equal operator, only supports float, bool and
         * std::string. Throws an exception if types mismatch or comparing
         * unsupported types.
         *
         * @param other
         *   Value to compare with.
         *
         * @returns
         *   True if this value is more than or equal the supplied one, false
         *   otherwise.
         */
        bool operator>=(const value &other) const
        {
            return !(*this < other);
        }

        /**
         * Less than or equal operator, only supports float, bool and
         * std::string. Throws an exception if types mismatch or comparing
         * unsupported types.
         *
         * @param other
         *   Value to compare with.
         *
         * @returns
         *   True if this value is less than or equal the supplied one, false
         *   otherwise.
         */
        bool operator<=(const value &other) const
        {
            return !(*this > other);
        }

        /**
         * Add another value to this, only supports float and std::string.
         * Throws an exception if types mismatch or adding unsupported types.
         *
         * @param other
         *   Value to add to this.
         *
         * @returns
         *   Reference to this object after adding.
         */
        value& operator+=(const value &other)
        {
            // handler for each supported type
            value_ = std::visit(
                detail::overload{
                    [&other](float val) -> variant_type { return variant_type{ val + other.get<float>() }; },
                    [](bool) -> variant_type { throw exception("cannot add bools"); },
                    [&other](std::string val) -> variant_type { return variant_type{ val + other.get<std::string>() }; },
                    [](object*) -> variant_type { throw exception("cannot add objects"); },
                    [](function) -> variant_type { throw exception("cannot add functions"); },
                },
                value_);

            return *this;
        }

        /**
         * Add two values, only supports float and std::string. Throws an
         * exception if types mismatch or adding unsupported types.
         *
         * @param other
         *   Value to add to this.
         *
         * @returns
         *   New value which is result of this added to supplied value.
         */
        value operator+(const value &other) const
        {
            value tmp{ *this };
            tmp += other;
            return tmp;
        }

        /**
         * Subtract another value from this, only supports float. Throws an
         * exception if types mismatch or adding unsupported types.
         *
         * @param other
         *   Value to subtract from this.
         *
         * @returns
         *   Reference to this object after subtracting.
         */
        value& operator-=(const value &other)
        {
            // handler for each supported type
            value_ = std::visit(
                detail::overload{
                    [&other](float val) -> variant_type { return variant_type{ val - other.get<float>() }; },
                    [](bool) -> variant_type { throw exception("cannot sub bools"); },
                    [](std::string) -> variant_type { throw exception("cannot sub strings"); },
                    [](object*) -> variant_type { throw exception("cannot sub objects"); },
                    [](function) -> variant_type { throw exception("cannot sub functions"); },
                },
                value_);

            return *this;
        }

        /**
         * Subtract two values, only supports float. Throws an exception if
         * types mismatch or adding unsupported types.
         *
         * @param other
         *   Value to subtract from this.
         *
         * @returns
         *   New value which is result of this subtracted from supplied value.
         */
        value operator-(const value &other) const
        {
            value tmp{ *this };
            tmp -= other;
            return tmp;
        }

        /**
         * Multiply another value with this, only supports float. Throws an
         * exception if types mismatch or adding unsupported types.
         *
         * @param other
         *   Value to multiply with this.
         *
         * @returns
         *   Reference to this object after multiplying.
         */
        value& operator*=(const value &other)
        {
            // handler for each supported type
            value_ = std::visit(
                detail::overload{
                    [&other](float val) -> variant_type { return variant_type{ val * other.get<float>() }; },
                    [](bool) -> variant_type { throw exception("cannot mul bools"); },
                    [](std::string) -> variant_type { throw exception("cannot mul strings"); },
                    [](object*) -> variant_type { throw exception("cannot mul objects"); },
                    [](function) -> variant_type { throw exception("cannot mul functions"); },
                },
                value_);

            return *this;
        }

        /**
         * Multiply two values, only supports float. Throws an exception if
         * types mismatch or adding unsupported types.
         *
         * @param other
         *   Value to multiply with this.
         *
         * @returns
         *   New value which is result of this multiplied with supplied value.
         */
        value operator*(const value &other) const
        {
            value tmp{ *this };
            tmp *= other;
            return tmp;
        }

        /**
         * Divide another value with this, only supports float. Throws an
         * exception if types mismatch or adding unsupported types.
         *
         * @param other
         *   Value to divide with this.
         *
         * @returns
         *   Reference to this object after dividing.
         */
        value& operator/=(const value &other)
        {
            // handler for each supported type
            value_ = std::visit(
                detail::overload{
                    [&other](float val) -> variant_type { return variant_type{ val / other.get<float>() }; },
                    [](bool) -> variant_type { throw exception("cannot div bools"); },
                    [](std::string) -> variant_type { throw exception("cannot div strings"); },
                    [](object*) -> variant_type { throw exception("cannot div objects"); },
                    [](function) -> variant_type { throw exception("cannot div functions"); },
                },
                value_);

            return *this;
        }

        /**
         * Divide two values, only supports float. Throws an exception if
         * types mismatch or adding unsupported types.
         *
         * @param other
         *   Value to divide with this.
         *
         * @returns
         *   New value which is result of this divided with supplied value.
         */
        value operator/(const value &other) const
        {
            value tmp{ *this };
            tmp /= other;
            return tmp;
        }

        /**
         * Write value to a stream, useful for debugging.
         *
         * @param out
         *   Stream to write to.
         *
         * @param v
         *   value to write to stream.
         *
         * @returns
         *   Reference to input stream.
         */
        friend std::ostream& operator<<(std::ostream &out, const value &v)
        {
            // handler for each supported type
            std::visit(
                detail::overload{
                    [&out](float val) { out << std::to_string(val); },
                    [&out](bool val) { out << std::boolalpha << val << std::noboolalpha; },
                    [&out](std::string val) { out << val; },
                    [&out](object *val)
                    {
                        out << "object(" << val << ")" << std::endl;
                        for(const auto &[key, value] : *val)
                        {
                            out << key << " : " << value << std::endl;
                        }
                    },
                    [&out](function val)
                    {
                        out << "function (" << val.num_args << "): " << val.code;
                    }
                },
                v.value_);

            return out;
        }

    private:

        /** variant holding stored value. */
        variant_type value_;
};

}

