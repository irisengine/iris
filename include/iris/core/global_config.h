#pragma once

#include <any>
#include <map>
#include <optional>
#include <string>

namespace iris
{

class GlobalConfig
{
  public:
    template <class T>
    static T get(const std::string &key)
    {
        const auto &config = instance();
        return config.get_impl<T>(key);
    }

    template <class T>
    static std::optional<T> try_get(const std::string &key)
    {
        const auto &config = instance();
        return config.try_get_impl<T>(key);
    }

    template <class T>
    static void set(const std::string &key, T &&value)
    {
        auto &config = instance();
        config.set_impl(key, std::forward<T>(value));
    }

  private:
    GlobalConfig()
        : entries_()
    {
    }

    static GlobalConfig &instance()
    {
        static GlobalConfig config;
        return config;
    }

    template <class T>
    T get_impl(const std::string &key) const
    {
        return std::any_cast<T>(entries_.at(key));
    }

    template <class T>
    std::optional<T> try_get_impl(const std::string &key) const
    {
        std::optional<T> entry;

        const auto value = entries_.find(key);
        if (value != std::cend(entries_))
        {
            if (value->second.type() == typeid(T))
            {
                entry = get_impl<T>(key);
            }
        }

        return entry;
    }

    template <class T>
    void set_impl(const std::string &key, T &&value)
    {
        entries_[key] = std::forward<T>(value);
    }

    std::map<std::string, std::any> entries_;
};

}
