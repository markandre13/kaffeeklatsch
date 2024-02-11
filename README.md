# kaffeeklatsch
executable specifications for c++

```c++
import store;

import kaffeeklatsch;
using namespace kaffeeklatsch;

kaffeeklatsch_spec([] {
    describe("a shopping cart", []{
        it("can be filled with articles", []{
            auto cart = make_shared<Cart>();
            auto article make_shared<Article>("Ketchup", 3.99EUR);
            cart.add(article);
            expect(cart).to.have.sizeOf(1);
            expect(cart.total()).to.equal(3.99EUR);
        });
    });
});

int main(int argc, char *argv[]) { 
    return kaffeeklatsch::run(argc, argv);
}
```

## About

this unit test library is a c++ variant of mocha/chai, which is a variant of rspec.

the mindset behind it is to regard tests not just as mere tests, but foremost as
executable specification and documentation.

* tests can capture the most expensive part of software development: figuring out what
  it is that needs to be build.

* the test's names serve as documentation to the user of the unit under test.

* the test's body is an example to the user on how to use the code being tested.

  one can duplicate such tests to learn how to use the unit by using it or to
  create regression tests with something does not work as expected.

* the test's body serves as executable specification which can be automatically validated.
  whenever the specification diverts from the implementation, the computer will tell you.

* tests, when done right, allow to refactor the whole implementation to keep it readable
  and extenable for future requirements.

## Be Aware...

* ...that unit tests are the least effective way of testing.

* ...that there two kinds of unit tests:
 
  * a _social unit test_ sends and gets data to to and from the unit via it's api.
    but the test does not care what other objects the unit may use under the hood.
    only slow external services might be mocked using validated fakes.
  
  * a _solitary unit test_ sends data to the unit's api and validates what data it
    is sending to other units. those other units are mocked. furthermore, if the
    mock returns data, the exact value should be of no relevance to the test, otherwise
    the test implementation will depend on the unit being mocked.

    this approach can be used in object oriented code, which, due to the
    _information hiding_ principle, has no getters, and instead of asking a unit for
    a value, one tell's a unit to do something. (tell, don't ask rule).

* ...of [context driven testing](https://context-driven-testing.com)

* ...of [shift-left testing](https://en.wikipedia.org/wiki/Shift-left_testing)

* ...of [growing object-oriented software driven by tests](http://www.growing-object-oriented-software.com)

## Further Reading

* steven r. baker's [history of rspec](https://stevenrbaker.com/tech/history-of-rspec.html)
* [rspec style guide](https://rspec.rubystyle.guide/)
* [rspec](https://rspec.info) (ruby)
* [mocha test framework](https://mochajs.org) (javascript/typescript)
* [chai assertion library](https://www.chaijs.com) (javascript/typescript)
* [bandit](https://banditcpp.github.io/bandit/) (c++)

## The Name

"kaffeeklatsch" is an informal social gathering for coffee and conversation. the reason
to choose this name was

* kaffee/coffee: to honour the mocha/chai javascript test tools i've working with for
  several years. and only while working on this library i learned that they were
  inspired by rspec.
* klatsch/gossip: i'm an advocate of xp, double-loop-tdd and 3c and hence to start
  by pairing with the user and writing down the names of acceptance tests.

## Addendum

thank you for your interest. i've tried to do this software in modern c++ but there is
still so much for me to learn.

if you'd like to contribute, please fork on github and send merge requests.

keep hacking,

mark
