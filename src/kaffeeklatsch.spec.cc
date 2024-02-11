import std;
using namespace std;

#include "kaffeeklatsch.hh"
using namespace kaffeeklatsch;

// TODO
// [ ] add test for .undefined()
// [ ] implement timeout & setting timeout
// [ ] run beforeEach & afterEach within Example::evaluate to include errors thrown there into the test's result
// [ ] add tests for describe()/it() without body
// [ ] render skipped group in gray, especially when it has no examples
// [ ] catch and report stdout/stderr
// [ ] use std::stacktrace once available
// [ ] many more matchers
// [ ] eq container, print a diff
// [ ] almost
// [ ] context, subject, shared_context, shared_example and other RSpec stuff (example, specify, focus, ...), feature, scenario
// [ ] exit with -1 when a least one test fails
// [ ] command line arguments
// [ ] disable colors when output is not a tty
// [ ] execute and print the test report in one go... or run threads...
// feature and scenario are a part of Capybara, and not RSpec, and are meant to be used for acceptance tests.
// feature is equivalent to describe / context, and scenario equivalent to it / example.
// https://rspec.rubystyle.guide

enum MyEnum { M0, M1, M2 };

kaffeeklatsch_spec([] {
    describe("runner", [] {
        describe("demo", [] {
            xit("skip", [] {});
            it("fail", [] { expect(false).to.eq(true); });
            it("slow", [] { usleep(40000); });
            it("very slow", [] { usleep(80000); });
        });
        describe("examples and groups", [] {
            it("no descriptions, no examples", [] {
                detail::tmp_spec([] {},
                                 [](const detail::Statistics &statistics) {
                                     expect(statistics.numTotalTestSuites).to.equal(0);
                                     expect(statistics.numTotalTests).to.equal(0);
                                 });
            });
            it("example: it(<description>, <body>)", [] {
                detail::tmp_spec([] { it("example", [] {}); },
                                 [](const detail::Statistics &statistics) {
                                     expect(statistics.numTotalTestSuites).to.equal(0);
                                     expect(statistics.numTotalTests).to.equal(1);
                                 });
            });
            it("group: describe(<description>, <body>)", [] {
                detail::tmp_spec([] { describe("group", [] {}); },
                                 [](const detail::Statistics &statistics) {
                                     expect(statistics.numTotalTestSuites).to.equal(1);
                                     expect(statistics.numTotalTests).to.equal(0);
                                 });
            });
            it("focus on example: fit(<description>, <body>)", [] {
                vector<const char *> log;
                detail::tmp_spec(
                    [&] {
                        describe("group0", [&] {
                            it("test0.0", [&] { log.push_back("0.0"); });
                            fit("test0.1", [&] { log.push_back("0.1"); });
                            it("test0.2", [&] { log.push_back("0.2"); });
                        });
                        describe("group1", [&] {
                            it("test1.0", [&] { log.push_back("1.0"); });
                            it("test1.1", [&] { log.push_back("1.1"); });
                            it("test1.2", [&] { log.push_back("1.2"); });
                        });
                    },
                    [&](const detail::Statistics &statistics) {
                        expect(statistics.numTotalTestSuites).to.equal(1);
                        expect(statistics.numTotalTests).to.equal(1);
                        expect(log).to.equal(vector{"0.1"});
                    });
            });
            it("focus on group: fdescribe(<description>, <body>)", [] {
                vector<const char *> log;
                detail::tmp_spec(
                    [&] {
                        describe("group0", [&] {
                            it("test0.0", [&] { log.push_back("0.0"); });
                            it("test0.1", [&] { log.push_back("0.1"); });
                            it("test0.2", [&] { log.push_back("0.2"); });
                        });
                        fdescribe("group1", [&] {
                            it("test1.0", [&] { log.push_back("1.0"); });
                            it("test1.1", [&] { log.push_back("1.1"); });
                            it("test1.2", [&] { log.push_back("1.2"); });
                        });
                    },
                    [&](const detail::Statistics &statistics) {
                        expect(statistics.numTotalTestSuites).to.equal(1);
                        expect(statistics.numTotalTests).to.equal(3);
                        expect(log).to.equal(vector{"1.0", "1.1", "1.2"});
                    });
            });
            it("skip example: xit(<description>, <body>)", [] {
                vector<const char *> log;
                detail::tmp_spec(
                    [&] {
                        describe("group0", [&] {
                            it("test0.0", [&] { log.push_back("0.0"); });
                            xit("test0.1", [&] { log.push_back("0.1"); });
                            it("test0.2", [&] { log.push_back("0.2"); });
                        });
                        describe("group1", [&] {
                            it("test1.0", [&] { log.push_back("1.0"); });
                            it("test1.1", [&] { log.push_back("1.1"); });
                            it("test1.2", [&] { log.push_back("1.2"); });
                        });
                    },
                    [&](const detail::Statistics &statistics) {
                        expect(statistics.numTotalTestSuites).to.equal(2);
                        expect(statistics.numSkippedTests).to.equal(1);
                        expect(statistics.numPassedTests).to.equal(5);
                        expect(statistics.numTotalTests).to.equal(6);
                        expect(log).to.equal(vector{"0.0", "0.2", "1.0", "1.1", "1.2"});
                    });
            });
            it("skip group: xdescribe(<description>, <body>)", [] {
                vector<const char *> log;
                detail::tmp_spec(
                    [&] {
                        describe("group0", [&] {
                            it("test0.0", [&] { log.push_back("0.0"); });
                            it("test0.1", [&] { log.push_back("0.1"); });
                            it("test0.2", [&] { log.push_back("0.2"); });
                        });
                        xdescribe("group1", [&] {
                            it("test1.0", [&] { log.push_back("1.0"); });
                            it("test1.1", [&] { log.push_back("1.1"); });
                            it("test1.2", [&] { log.push_back("1.2"); });
                        });
                    },
                    [&](const detail::Statistics &statistics) {
                        expect(statistics.numTotalTestSuites).to.equal(2);
                        expect(statistics.numSkippedTests).to.equal(3);
                        expect(statistics.numPassedTests).to.equal(3);
                        expect(statistics.numTotalTests).to.equal(6);
                        expect(log).to.equal(vector{"0.0", "0.1", "0.2"});
                    });
            });
            // TODO: mix skip & focus
            it("beforeAll(<body>), beforeEach(<body>), afterEach(<body>), afterAll(<body>)", [] {
                vector<const char *> log;
                detail::tmp_spec(
                    [&] {
                        beforeAll([&] { log.push_back("beforeAll"); });
                        beforeEach([&] { log.push_back("beforeEach"); });
                        afterEach([&] { log.push_back("afterEach"); });
                        afterAll([&] { log.push_back("afterAll"); });

                        it("test0.0", [&] { log.push_back("0"); });
                        it("test0.1", [&] { log.push_back("1"); });
                        it("test0.2", [&] { log.push_back("2"); });
                    },
                    [&](const detail::Statistics &statistics) {
                        expect(statistics.numTotalTestSuites).to.equal(0);
                        expect(statistics.numTotalTests).to.equal(3);
                        expect(log).to.equal(
                            vector{"beforeAll", "beforeEach", "0", "afterEach", "beforeEach", "1", "afterEach", "beforeEach", "2", "afterEach", "afterAll"});
                    });
            });
        });
    });

    describe("expect(<actual>)", [] {
        describe(".<chain>", [] {
            it("to", [] { expect(1).to.equal(1); });
            it("be", [] { expect(1).be.equal(1); });
            it("been", [] { expect(1).been.equal(1); });
            it("is", [] { expect(1).is.equal(1); });
            it("that", [] { expect(1).that.equal(1); });
            it("which", [] { expect(1).which.equal(1); });
            it("and_", [] { expect(1).and_.equal(1); });
            it("has", [] { expect(1).has.equal(1); });
            it("have", [] { expect(1).have.equal(1); });
            it("with", [] { expect(1).with.equal(1); });
            it("at", [] { expect(1).at.equal(1); });
            it("of", [] { expect(1).of.equal(1); });
            it("same", [] { expect(1).same.equal(1); });
            it("but", [] { expect(1).but.equal(1); });
            it("does", [] { expect(1).does.equal(1); });
            it("still", [] { expect(1).still.equal(1); });
            it("also", [] { expect(1).also.equal(1); });
        });

        describe(".not_()", [] {
            it("not", [] { expect(42).to.not_().equal(19); });
            it("not not", [] { expect(42).to.not_().not_().equal(42); });
            it("not eq and not eq", [] { expect(42).to.not_().equal(6).and_.to.not_().equal(7); });
        });

        describe(".equal(<expected>)", [] {
            it(".eq(...)", [] { expect(1).eq(1); });
            it(".equal(...)", [] { expect(1).to.equal(1); });
            it(".equals(...)", [] { expect(1).equals(1); });
            it("error: expected '...' to equal '...'", [] {
                expect([] {
                    // prettier-ignore
                    expect(M1).eq(M2);
                }).to.throw_(assertion_error("expected 1 to equal 2", "", 0));
            });
        });

        describe("gt", [] {
            it(".gt(...)", [] {
                expect(2).gt(1);
                expect(2).not_().gt(2);
                expect(2).not_().gt(3);
            });
            it(".greaterThan(...)", [] { expect(2).greaterThan(1); });
            it(".above(...)", [] { expect(2).above(1); });
            it("error: expected '...' to be greater than '...'", [] {
                expect([] {
                    // prettier-ignore
                    expect(1).gt(2);
                }).to.throw_(assertion_error("expected 1 to be greater than 2", "", 0));
            });
        });

        describe("gte", [] {
            it(".gte(...)", [] {
                expect(2).gte(1);
                expect(2).gte(2);
                expect(2).not_().gte(3);
            });
            it(".greaterThanOrEqual(...)", [] { expect(2).greaterThanOrEqual(1); });
            it(".least(...)", [] { expect(2).least(1); });
            it("error: expected '...' to be greater than or equal '...'", [] {
                expect([] {
                    // prettier-ignore
                    expect(2).gte(3);
                }).to.throw_(assertion_error("expected 2 to be greater than or equal 3", "", 0));
            });
        });

        describe("lt", [] {
            it(".lt(...)", [] {
                expect(2).not_().lt(1);
                expect(2).not_().lt(2);
                expect(2).lt(3);
            });
            it(".lessThan(...)", [] { expect(1).lessThan(2); });
            it(".below(...)", [] { expect(1).below(2); });
            it("error: expected '...' to be less than '...'", [] {
                expect([] {
                    // prettier-ignore
                    expect(3).lt(2);
                }).to.throw_(assertion_error("expected 3 to be less than 2", "", 0));
            });
        });

        describe("lte", [] {
            it(".lte(...)", [] {
                expect(2).not_().lte(1);
                expect(2).lte(2);
                expect(2).lte(3);
            });
            it(".lessThanOrEqual(...)", [] { expect(1).lessThanOrEqual(2); });
            it(".most(...)", [] { expect(1).to.be.at.most(2); });
            it("error: expected '...' to be less than '...'", [] {
                expect([] {
                    // prettier-ignore
                    expect(3).lte(2);
                }).to.throw_(assertion_error("expected 3 to be less than or equal 2", "", 0));
            });
        });

        describe("within", [] {
            it(".within(<min>, <max>)", [] {
                expect(2).to.be.within(1, 3);
                expect(2).to.be.within(2, 2);
            });
            it("error: expected <value> to be within <min>..<max>", [] {
                expect([] {
                    // prettier-ignore
                    expect(2).within(3, 5);
                }).to.throw_(assertion_error("expected 2 to be within 3..5", "", 0));
            });
        });

        describe(".beTrue()", [] {
            it("okay", [] { expect(true).to.beTrue(); });
            it("error", [] {
                expect([] {
                    // prettier-ignore
                    expect(false).to.beTrue();
                }).to.throw_(assertion_error("expected false to be true", "", 0));
            });
        });

        describe(".beFalse()", [] {
            it("okay", [] { expect(false).to.beFalse(); });
            it("error", [] {
                expect([] {
                    // prettier-ignore
                    expect(true).to.beFalse();
                }).to.throw_(assertion_error("expected true to be false", "", 0));
            });
        });

        describe(".undefined() of std::optional<T>", [] {
            it("okay", [] { expect(std::optional<int>{}).to.be.undefined(); });
            it("error", [] {
                expect([] {
                    // prettier-ignore
                    expect(std::optional<int>{8}).to.be.undefined();
                }).to.throw_(assertion_error("expected 8 to be undefined", "", 0));
            });
        });

        describe("container", [] {
            describe(".sizeOf(<expected>)", [] {
                it(".sizeOf(...)", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect(v1).to.have.sizeOf(4);
                });
                it(".not.sizeOf(...)", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect(v1).to.not_().sizeOf(5);
                });
                it("error .sizeOf(...): expected to contain '...'", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect([&] {
                        // prettier-ignore
                        expect(v1).to.have.sizeOf(5);
                    }).to.throw_(assertion_error("expected size 4 to equal 5", "", 0));
                });
                it("error .not.sizeOf(...): expected to not contain '...'", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect([&] {
                        // prettier-ignore
                        expect(v1).to.not_().have.sizeOf(4);
                    }).to.throw_(assertion_error("expected size 4 to not equal 4", "", 0));
                });
            });
            describe(".empty()", [] {
                it(".empty(...)", [] {
                    std::vector<int> v1;
                    expect(v1).to.be.empty();
                });
                it(".not.empty()", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect(v1).to.not_().empty();
                });
                it("error .empty(): expected to be empty", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect([&] {
                        // prettier-ignore
                        expect(v1).to.be.empty();
                    }).to.throw_(assertion_error("expected to be empty", "", 0));
                });
                it("error .not.sizeOf(...): expected to not contain '...'", [] {
                    std::vector<int> v1;
                    expect([&] {
                        // prettier-ignore
                        expect(v1).to.be.not_().empty();
                    }).to.throw_(assertion_error("expected to be not empty", "", 0));
                });
            });

            describe(".contains(<expected>)", [] {
                it(".contain(<element>)", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect(v1).to.contain(3);
                });
                it(".not.contain(<element>)", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect(v1).to.not_().contain(5);
                });
                it("error .contain(...): expected to contain '...'", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect([&] {
                        // prettier-ignore
                        expect(v1).contains(5);
                    }).to.throw_(assertion_error("expected to contain 5", "", 0));
                });
                it("error .not.contain(...): expected to not contain '...'", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect([&] {
                        // prettier-ignore
                        expect(v1).does.not_().contain(3);
                    }).to.throw_(assertion_error("expected to not contain 3", "", 0));
                });
                it(".contains(<element>)", [] {
                    std::vector<int> v1{1, 2, 3, 4};
                    expect(v1).contains(3);
                });
            });
        });

        describe(".throws(<expected>)", [] {
            it("error: no exception being thrown", [] {
                expect([] {
                    // prettier-ignore
                    expect([] {}).to.throw_(std::exception());
                }).to.throw_(assertion_error("no exception", "", 0));
            });
            it("error: exception thrown is a sub class of expected exception", [] {
                expect([] {
                    // prettier-ignore
                    expect([] {
                        // prettier-ignore
                        throw std::exception();
                    }).to.throw_(std::logic_error("yikes"));
                }).to.throw_(assertion_error("wrong exception", "", 0));
            });
            it("error: exception thrown is a super class of expected exception", [] {
                expect([] {
                    // prettier-ignore
                    expect([] {
                        // prettier-ignore
                        throw std::logic_error("yikes");
                    }).to.throw_(std::exception());
                }).to.throw_(assertion_error("wrong exception", "", 0));
            });
            it("error: exception thrown has a different what()", [] {
                expect([] {
                    // prettier-ignore
                    expect([] {
                        // prettier-ignore
                        throw std::logic_error("d'oh!");
                    }).to.throw_(std::logic_error("yikes"));
                }).to.throw_(assertion_error("expected what() 'd'oh!' to equal 'yikes'", "", 0));
            });
            it("okay: exception thrown is same class of expected exception and has same what()", [] {
                expect([] {
                    // prettier-ignore
                    throw std::logic_error("yikes");
                }).to.throw_(std::logic_error("yikes"));
            });
            it("okay: exception thrown is same class of expected exception and has no what()", [] {
                expect([] {
                    // prettier-ignore
                    throw std::string();
                }).to.throw_(std::string());
            });
            it("okay: not_().throw_()", [] {
                // prettier-ignore
                expect([] {}).to.not_().throw_();
            });
            it("okay: throw_()", [] {
                expect([] {
                    // prettier-ignore
                    throw std::exception();
                }).to.throw_();
            });
            it("error: not_().throw_()", [] {
                expect([] {
                    // prettier-ignore
                    expect([] { throw std::exception(); }).to.not_().throw_();
                }).to.throw_(assertion_error("expected to not throw", "", 0));
            });
            it("error: throw_()", [] {
                expect([] {
                    // prettier-ignore
                    expect([] {}).to.throw_();
                }).to.throw_(assertion_error("expected to throw", "", 0));
            });
            // TODO: don't automatically compare the what() message, use 'throws(..., message)' instead?
        });
    });
    describe("details", [] {
        describe("to_str(...)", [] {
            it("to_str(8) -> 8", [] { expect(to_str(8)).to.equal("8"); });
            it("to_str(false) -> true", [] { expect(to_str(false)).to.equal("false"); });
            it("to_str(true) -> false", [] { expect(to_str(true)).to.equal("true"); });
            it("to_str('c') -> 'c'", [] { expect(to_str('c')).to.equal("'c'"); });
            it("to_str(\"hello\") -> \"c string\"", [] { expect(to_str("c string")).to.equal("\"c string\""); });
            it("to_str(string(\"c++ string\")) -> \"c++ string\"", [] { expect(to_str("c++ string")).to.equal("\"c++ string\""); });
            it("to_str(vector{1,2,3,4}) -> object", [] { expect(to_str(vector{1, 2, 3, 4})).to.equal("object"); });
        });
    });
});
