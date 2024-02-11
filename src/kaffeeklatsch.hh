#pragma once

// kaffeeklatsch: a c++ mocha/chai variant or rspec
// https://stevenrbaker.com/tech/history-of-rspec.html

#include <chrono>
#include <cstring>
#include <exception>
#include <format>
#include <functional>
#include <iostream>
#include <print>
#include <sstream>
#include <string>
#include <vector>

namespace kaffeeklatsch {

// TODO: move more into details::

struct assertion_error : public std::runtime_error {
    public:
        const char* filename;
        unsigned line;
        assertion_error() : runtime_error("assertion error") {}
        assertion_error(const std::string& what, const char* filename, unsigned line) : runtime_error(what), filename(filename), line(line) {}
};

// https://stackoverflow.com/questions/61199610/concept-to-check-if-a-class-is-streamable
template <typename T>
class is_streamable {
        template <typename U>  // must be template to get SFINAE fall-through...
        static auto test(const U* u) -> decltype(std::cout << *u);
        static auto test(...) -> std::false_type;

    public:
        enum { value = !std::is_same_v<decltype(test((T*)0)), std::false_type> };
};

template <typename T>
std::string to_str(T value) {
    if constexpr (is_streamable<T>::value) {
        std::stringstream out;
        out << value;
        return out.str();
    } else {
        return "object";
    }
}
inline std::string to_str(char value) { return std::format("'{}'", value); }
inline std::string to_str(bool value) { return value ? "true" : "false"; }
inline std::string to_str(const char* value) { return std::format("\"{}\"", value); }
inline std::string to_str(const std::string& value) { return std::format("\"{}\"", value); }
template <typename T>
std::string to_str(std::optional<T> value) {
    return value.has_value() ? to_str(value.value()) : "undefined";
}

template <typename T>
class Assertion {
        T m_value;
        bool m_negate = false;
        const char* filename;
        unsigned line;

    public:
        Assertion(T value, const char* filename, unsigned line) : m_value(value), filename(filename), line(line) {}
        Assertion() = delete;
        Assertion(const Assertion&) = delete;
        Assertion(Assertion&&) = delete;
        Assertion& operator=(const Assertion&) = delete;
        Assertion& operator=(Assertion&&) = delete;

        //
        // chainables to improve the readability of the assertions
        //
        Assertion &to{*this}, &be{*this}, &been{*this}, &is{*this}, &that{*this}, &which{*this}, &and_{*this}, &has{*this}, &have{*this}, &with{*this},
            &at{*this}, &of{*this}, &same{*this}, &but{*this}, &does{*this}, &still{*this}, &also{*this};

        //
        // eq
        //
        Assertion& eq(T value) {
            if (m_negate) {
                if (m_value == value) {
                    throw assertion_error(std::format("expected {} to not equal {}", to_str(m_value), to_str(value)), filename, line);
                }
            } else {
                if (m_value != value) {
                    throw assertion_error(std::format("expected {} to equal {}", to_str(m_value), to_str(value)), filename, line);
                }
            }
            m_negate = false;
            return *this;
        }
        Assertion& equal(T value) { return eq(value); }
        Assertion& equals(T value) { return eq(value); }

        Assertion& undefined() {
            if (m_value.has_value()) {
                throw assertion_error(std::format("expected {} to be undefined", to_str(m_value)), filename, line);
            }
            return *this;
        }
        Assertion& beTrue() {
            if (!m_value) {
                throw assertion_error(std::format("expected {} to be true", to_str(m_value)), filename, line);
            }
            return *this;
        }
        Assertion& beFalse() {
            if (m_value) {
                throw assertion_error(std::format("expected {} to be false", to_str(m_value)), filename, line);
            }
            return *this;
        }

        //
        // gt
        //
        Assertion& gt(auto value) {
            if (m_negate) {
                if (m_value > value) {
                    throw assertion_error(std::format("expected {} to be not greater than {}", to_str(m_value), to_str(value)), filename, line);
                }
            } else {
                if (!(m_value > value)) {
                    throw assertion_error(std::format("expected {} to be greater than {}", to_str(m_value), to_str(value)), filename, line);
                }
            }
            m_negate = false;
            return *this;
        }
        Assertion& greaterThan(auto value) { return gt(value); }
        Assertion& above(auto value) { return gt(value); }

        //
        // gte
        //
        Assertion& gte(auto value) {
            if (m_negate) {
                if (m_value >= value) {
                    throw assertion_error(std::format("expected {} to be not greater than or equal {}", to_str(m_value), to_str(value)), filename, line);
                }
            } else {
                if (!(m_value >= value)) {
                    throw assertion_error(std::format("expected {} to be greater than or equal {}", to_str(m_value), to_str(value)), filename, line);
                }
            }
            m_negate = false;
            return *this;
        }
        Assertion& greaterThanOrEqual(auto value) { return gte(value); }
        Assertion& least(auto value) { return gte(value); }

        //
        // lt
        //
        Assertion& lt(auto value) {
            if (m_negate) {
                if (m_value < value) {
                    throw assertion_error(std::format("expected {} to be not less than {}", to_str(m_value), to_str(value)), filename, line);
                }
            } else {
                if (!(m_value < value)) {
                    throw assertion_error(std::format("expected {} to be less than {}", to_str(m_value), to_str(value)), filename, line);
                }
            }
            m_negate = false;
            return *this;
        }
        Assertion& lessThan(auto value) { return lt(value); }
        Assertion& below(auto value) { return lt(value); }

        //
        // lte
        //
        Assertion& lte(auto value) {
            if (m_negate) {
                if (m_value <= value) {
                    throw assertion_error(std::format("expected {} to be less than or equal {}", to_str(m_value), to_str(value)), filename, line);
                }
            } else {
                if (!(m_value <= value)) {
                    throw assertion_error(std::format("expected {} to be less than or equal {}", to_str(m_value), to_str(value)), filename, line);
                }
            }
            m_negate = false;
            return *this;
        }
        Assertion& lessThanOrEqual(auto value) { return lte(value); }
        Assertion& most(auto value) { return lte(value); }

        //
        // within
        //
        template <typename A>
        Assertion& within(A min, A max) {
            if (m_negate) {
                if (min <= m_value && m_value <= max) {
                    throw assertion_error(std::format("expected {} to be within {}..{}", to_str(m_value), to_str(min), to_str(max)), filename, line);
                }
            } else {
                if (!(min <= m_value && m_value <= max)) {
                    throw assertion_error(std::format("expected {} to be within {}..{}", to_str(m_value), to_str(min), to_str(max)), filename, line);
                }
            }
            m_negate = false;
            return *this;
        }

        //
        // container
        //
        Assertion& sizeOf(size_t size) {
            if (m_negate) {
                if (m_value.size() == size) {
                    throw assertion_error(std::format("expected size {} to not equal {}", m_value.size(), size), filename, line);
                }
            } else {
                if (m_value.size() != size) {
                    throw assertion_error(std::format("expected size {} to equal {}", m_value.size(), size), filename, line);
                }
            }
            m_negate = false;
            return *this;
        }
        Assertion& empty() {
            if (m_negate) {
                if (m_value.empty()) {
                    throw assertion_error("expected to be not empty", filename, line);
                }
            } else {
                if (!m_value.empty()) {
                    throw assertion_error("expected to be empty", filename, line);
                }
            }
            m_negate = false;
            return *this;
        }
        Assertion& contain(auto value) {
            bool contains = false;
            for (auto& it : m_value) {
                if (it == value) {
                    contains = true;
                    break;
                }
            }
            if (m_negate) {
                if (contains) {
                    throw assertion_error(std::format("expected to not contain {}", to_str(value)), filename, line);
                }
            } else {
                if (!contains) {
                    throw assertion_error(std::format("expected to contain {}", to_str(value)), filename, line);
                }
            }
            m_negate = false;
            return *this;
        }
        Assertion& contains(auto value) { return contain(value); }

        //
        // not
        //
        Assertion& not_() {
            this->m_negate = !this->m_negate;
            return *this;
        }

        //
        // throw
        //
        template <typename A>
        Assertion& throw_(A expect) {
            if (m_negate) {
                throw assertion_error(".not.throw(<expected>) is not implemented yet", filename, line);
            }
            try {
                m_value();
            } catch (const A& caught) {
                if (typeid(caught).name() != typeid(expect).name()) {
                    throw assertion_error(std::format("wrong exception"), filename, line);
                }
                const char* what0 = nullptr;
                const char* what1 = nullptr;
                try {
                    throw caught;
                } catch (const std::exception& e) {
                    what0 = e.what();
                } catch (...) {
                }
                try {
                    throw expect;
                } catch (const std::exception& e) {
                    what1 = e.what();
                } catch (...) {
                }
                if (what0 && what1 && std::strcmp(what0, what1) != 0) {
                    throw assertion_error(std::format("expected what() '{}' to equal '{}'", what0, what1), filename, line);
                }
                return *this;
            } catch (...) {
                throw assertion_error(std::format("wrong exception"), filename, line);
            }
            throw assertion_error(std::format("no exception"), filename, line);
        }
        Assertion& throws() { return throw_(); }

        Assertion& throw_() {
            try {
                m_value();
            } catch (...) {
                if (m_negate) {
                    throw assertion_error("expected to not throw", filename, line);
                }
                return *this;
            }
            if (!m_negate) {
                throw assertion_error("expected to throw", filename, line);
            }
            return *this;
        }
};

#define expect(V) _expect(V, __FILE__, __LINE__)

template <typename T>
Assertion<T> _expect(T value, const char* filename, unsigned line) {
    return Assertion(value, filename, line);
}

namespace detail {

using namespace std::chrono_literals;

struct Statistics {
        unsigned numTotalTests = 0;
        unsigned numPassedTests = 0;
        unsigned numSkippedTests = 0;
        unsigned numFailedTests = 0;
        unsigned numTotalTestSuites = 0;
        std::chrono::nanoseconds totalDuration = 0ns;
};

struct Item {
        Item(const std::string name, std::function<void()> body) : name(name), body(body) {}
        virtual ~Item();

        virtual void scan() = 0;
        virtual void evaluate(Statistics* statistics) = 0;
        virtual void report(const std::string& indent) = 0;
        virtual void reportFailures(const std::string& path) = 0;

        std::string name;
        std::function<void()> body;
        bool m_focus = false;
        bool m_has_focus_child = false;
        bool m_skip = false;
        bool m_has_skip_parent = false;
};

struct Example : Item {
        Example(const std::string name, std::function<void()> body) : Item(name, body) {}
        Example& only() {
            m_focus = true;
            return *this;
        }
        Example& skip() {
            m_skip = true;
            return *this;
        }
        // protected:
        void scan();
        void evaluate(Statistics* statistics);
        void report(const std::string& indent);
        void reportFailures(const std::string& path);

        bool passed = true;
        bool skipped = false;
        std::chrono::nanoseconds duration;
        assertion_error error;
};

struct ExampleGroup : Item {
        ExampleGroup(const std::string name, std::function<void()> body) : Item(name, body) {}
        ExampleGroup& only() {
            m_focus = true;
            return *this;
        }
        ExampleGroup& skip() {
            m_skip = true;
            return *this;
        }
        // protected
        void scan();
        void evaluate(Statistics* statistics);
        void report(const std::string& indent);
        void reportFailures(const std::string& path);
        std::vector<std::unique_ptr<Item>> items;
        std::vector<std::function<void()>> beforeAll;
        std::vector<std::function<void()>> beforeEach;
        std::vector<std::function<void()>> afterEach;
        std::vector<std::function<void()>> afterAll;
};

using spec_registry = std::vector<std::function<void()>>;

inline detail::spec_registry& specs() {
    static detail::spec_registry registry;
    return registry;
}

struct spec_registrar {
        spec_registrar(std::function<void()> func) { kaffeeklatsch::detail::specs().push_back(func); }
};

void tmp_spec(std::function<void()> body, std::function<void(const Statistics&)> eval);

};  // namespace detail

int run(int argc, char* argv[]);

// TODO: variants without body which will default to being skipped
detail::ExampleGroup& describe(const std::string& suitename, std::function<void()> body);
detail::ExampleGroup& fdescribe(const std::string& suitename, std::function<void()> body);
detail::ExampleGroup& xdescribe(const std::string& suitename, std::function<void()> body);
detail::ExampleGroup& describe(const std::string& suitename);
detail::Example& it(const std::string& testname, std::function<void()> body);
detail::Example& fit(const std::string& testname, std::function<void()> body);
detail::Example& xit(const std::string& testname, std::function<void()> body);
detail::Example& it(const std::string& testname);
void beforeEach(std::function<void()> body);
void afterEach(std::function<void()> body);
void beforeAll(std::function<void()> body);
void afterAll(std::function<void()> body);

};  // namespace kaffeeklatsch

#define KAFFEEKLATSCH_CONCAT2(a, b) a##b
#define KAFFEEKLATSCH_CONCAT(a, b) KAFFEEKLATSCH_CONCAT2(a, b)
#define KAFFEEKLATSCH_ADD_COUNTER(a) KAFFEEKLATSCH_CONCAT(a, __COUNTER__)

#define kaffeeklatsch_spec static kaffeeklatsch::detail::spec_registrar KAFFEEKLATSCH_ADD_COUNTER(kaffeeklatsch_registrar_)
