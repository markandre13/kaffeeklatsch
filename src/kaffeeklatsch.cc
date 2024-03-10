#include "kaffeeklatsch.hh"

namespace kaffeeklatsch {

enum Status { STATUS_PASSED, STATUS_FAILED, STATUS_SKIPPED };

namespace colour {

auto reset = "\x1b[0m";

auto black = "\x1b[30m";
auto red = "\x1b[31m";
auto green = "\x1b[32m";
auto yellow = "\x1b[33m";
auto blue = "\x1b[34m";
auto magenta = "\x1b[35m";
auto cyan = "\x1b[36m";
auto white = "\x1b[37m";

auto boldBlack = "\x1b[30;1m";
auto boldRed = "\x1b[31;1m";
auto boldGreen = "\x1b[32;1m";
auto boldYellow = "\x1b[33;1m";
auto boldBlue = "\x1b[34;1m";
auto boldMagenta = "\x1b[35;1m";
auto boldCyan = "\x1b[36;1m";
auto boldWhite = "\x1b[37;1m";

auto bold = "\x1b[1m";
auto underline = "\x1b[4m";
auto noUnderline = "\x1b[24m";
auto reversed = "\x1b[7m";

auto grey = "\x1b[90m";
auto BrightBlue = "\x1b[94m";

};  // namespace colour

namespace detail {

static ExampleGroup* currentSuite = nullptr;

std::string formatStatus(Status status, const std::string& name) {
    switch (status) {
        case STATUS_PASSED:
            return std::format("{}✔ {}{}", colour::green, name, colour::reset);
        case STATUS_FAILED:
            return std::format("{}✖ {}{}", colour::red, name, colour::reset);
        case STATUS_SKIPPED:
            return std::format("{}✖ {}{}", colour::grey, name, colour::reset);
    }
}

static auto slow = 75ms;

std::string formatDuration(std::chrono::nanoseconds duration) {
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(duration);
    if (ms >= slow) return std::format("{} ({}){}", colour::red, ms, colour::reset);
    if (ms >= slow / 2) return std::format("{} ({}){}", colour::yellow, ms, colour::reset);
    return "";
}

void report(Statistics* statistics) {
    currentSuite->scan();
    currentSuite->evaluate(statistics);
    --statistics->numTotalTestSuites;  // remove the root group
    statistics->numTotalTests = statistics->numPassedTests + statistics->numSkippedTests + statistics->numFailedTests;

    std::println("TEST REPORT\n");
    currentSuite->report("");

    if (statistics->numTotalTests > 0) {
        std::println("");
    }
    auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(statistics->totalDuration);
    std::println("{}Finished {} tests in {} test suites in {}", colour::green, statistics->numTotalTests, statistics->numTotalTestSuites, ms);
    std::println("");

    if (statistics->numTotalTests > 0) {
        std::println("{}{}SUMMARY:{}\n", colour::boldWhite, colour::underline, colour::reset);
    }
    if (statistics->numPassedTests != 0) {
        std::println("{}", formatStatus(STATUS_PASSED, std::format("{} tests completed", statistics->numPassedTests)));
    }
    if (statistics->numSkippedTests != 0) {
        std::println("{}", formatStatus(STATUS_SKIPPED, std::format("{} tests skipped", statistics->numSkippedTests)));
    }
    if (statistics->numFailedTests != 0) {
        std::println("{}", formatStatus(STATUS_FAILED, std::format("{} tests failed", statistics->numFailedTests)));
    }
    if (statistics->numTotalTests > 0) {
        std::println("");
    }

    if (statistics->numFailedTests != 0) {
        std::println("{}{}FAILED TESTS:{}\n", colour::boldWhite, colour::underline, colour::reset);
        currentSuite->reportFailures("");
        std::println("");
    }
    std::println("{}", colour::reset);
}

Item::~Item() {}

void ExampleGroup::scan() {
    for (auto& item : items) {
        item->m_skip = item->m_skip || m_skip;
        item->scan();
        m_has_focus_child = m_has_focus_child || item->m_has_focus_child || item->m_focus;
    }
}

void Example::scan() {}

void ExampleGroup::evaluate(Statistics* statistics) {
    ++statistics->numTotalTestSuites;
    for (auto& call : beforeAll) {
        call();
    }
    for (auto& item : items) {
        if (m_has_focus_child && !(item->m_has_focus_child || item->m_focus)) {
            continue;
        }
        item->evaluate(statistics);
    }
    for (auto& call : afterAll) {
        call();
    }
}

void Example::evaluateBeforeEach(ExampleGroup *group) {
    if (!group) {
        return;
    }
    if (group->parent) {
        evaluateBeforeEach(group->parent);
    }
    for (auto& call : group->beforeEach) {
        call();
    }
}

void Example::evaluateAfterEach(ExampleGroup *group) {
    if (!group) {
        return;
    }
    for (auto& call : group->afterEach) {
        call();
    }
    if (group->parent) {
        evaluateAfterEach(group->parent);
    }
}

void Example::evaluate(Statistics* statistics) {
    auto begin = std::chrono::high_resolution_clock::now();
    try {
        if (m_skip) {
            skipped = true;
        } else {
            evaluateBeforeEach(parent);
            body();
            evaluateAfterEach(parent);
        }
    } catch (assertion_error const& ex) {
        passed = false;
        error = ex;
    } catch (std::exception const& ex) {
        passed = false;
        error = assertion_error(ex.what(), "unknown", 0);
    } catch (...) {
        passed = false;
        error = assertion_error("catch all", "unknown", 0);
    }
    auto end = std::chrono::high_resolution_clock::now();
    duration = end - begin;
    statistics->totalDuration += duration;
    if (skipped) {
        ++statistics->numSkippedTests;
    } else {
        if (passed) {
            ++statistics->numPassedTests;
        } else {
            ++statistics->numFailedTests;
        }
    }
}

void ExampleGroup::report(const std::string& indent) {
    auto nextIndent = indent + "  ";
    std::println("{}{}{}{}", indent, colour::boldWhite, name, colour::reset);
    for (auto& item : items) {
        if (m_has_focus_child && !(item->m_has_focus_child || item->m_focus)) {
            continue;
        }
        item->report(nextIndent);
    }
}

void Example::report(const std::string& indent) {
    auto status = STATUS_FAILED;
    if (skipped) {
        status = STATUS_SKIPPED;
    } else {
        if (passed) {
            status = STATUS_PASSED;
        }
    }
    std::println("{}{}{}", indent, formatStatus(status, name), formatDuration(duration));
}

void ExampleGroup::reportFailures(const std::string& path) {
    for (auto& item : items) {
        item->reportFailures(path.size() == 0 ? name : std::format("{} > {}", path, name));
    }
}

void Example::reportFailures(const std::string& path) {
    if (!passed) {
        std::println("  {}∙ {} > {}{}", colour::red, path, name, colour::reset);
        std::println("    {}:{}: {}", error.filename, error.line, error.what());
    }
}

// this one is for testing purposes
void tmp_spec(std::function<void()> body, std::function<void(const Statistics&)> verify) {
    auto previousSuite = currentSuite;
    // std::println(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>");
    detail::ExampleGroup root(nullptr, "", [] {});
    detail::currentSuite = &root;
    body();
    Statistics statistics;
#if 0    
    detail::report(&statistics);
#else
    // FIXME: duplicate code from detail::report()
    currentSuite->scan();
    currentSuite->evaluate(&statistics);
    --statistics.numTotalTestSuites;  // remove the root group
    statistics.numTotalTests = statistics.numPassedTests + statistics.numSkippedTests + statistics.numFailedTests;
#endif
    currentSuite = previousSuite;

    verify(statistics);
    // std::println("<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<");
}

}  // namespace detail

int run(int, char*[]) {
    std::println("{}TEST RUN:\n\n{}START:{}\n", colour::boldWhite, colour::underline, colour::reset);
    detail::ExampleGroup root(nullptr, "", [] {});
    detail::currentSuite = &root;
    for (auto& suite : detail::specs()) {
        suite();
    }
    detail::Statistics statistics;
    detail::report(&statistics);
    detail::currentSuite = nullptr;
    return 0;
}

detail::ExampleGroup& describe(const std::string& groupname, std::function<void()> body) {
    auto parentSuite = detail::currentSuite;
    auto group = std::make_unique<detail::ExampleGroup>(parentSuite, groupname, body);
    detail::ExampleGroup* ptr = group.get();
    detail::currentSuite = ptr;
    parentSuite->items.push_back(std::move(group));
    body();
    detail::currentSuite = parentSuite;
    return *ptr;
}

detail::ExampleGroup& fdescribe(const std::string& groupname, std::function<void()> body) { return describe(groupname, body).only(); }
detail::ExampleGroup& xdescribe(const std::string& groupname, std::function<void()> body) { return describe(groupname, body).skip(); }
detail::ExampleGroup& describe(const std::string& groupname) { return xdescribe(groupname, []{}); }

detail::Example& it(const std::string& examplename, std::function<void()> body) {
    auto parentSuite = detail::currentSuite;
    auto example = std::make_unique<detail::Example>(parentSuite, examplename, body);
    detail::Example* ptr = example.get();
    parentSuite->items.push_back(std::move(example));
    return *ptr;
}

detail::Example& fit(const std::string& examplename, std::function<void()> body) { return it(examplename, body).only(); }
detail::Example& xit(const std::string& examplename, std::function<void()> body) { return it(examplename, body).skip(); }
detail::Example& it(const std::string& examplename) { return xit(examplename, []{}); }

void beforeAll(std::function<void()> body) { detail::currentSuite->beforeAll.push_back(body); }
void beforeEach(std::function<void()> body) { detail::currentSuite->beforeEach.push_back(body); }
void afterEach(std::function<void()> body) { detail::currentSuite->afterEach.push_back(body); }
void afterAll(std::function<void()> body) { detail::currentSuite->afterAll.push_back(body); }

}  // namespace kaffeeklatsch
