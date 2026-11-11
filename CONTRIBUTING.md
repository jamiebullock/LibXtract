# Contributing to LibXtract

Bug reports, fixes, tests and documentation improvements are welcome. This
file covers how to get a change accepted and the terms it is accepted under.

## Before you start

- Open an issue for a bug, a regression, a missing test or a documentation
  gap. Include the platform, the compiler, and the smallest input that shows
  the problem.
- For a new feature, open an issue first to discuss it. The library keeps a
  small, uniform API and not every feature belongs in it.
- Security problems: report them privately through GitHub's vulnerability
  reporting, and do not open a public issue. See `SECURITY.md`.

## Building and testing

The README covers the prerequisites and the CMake presets. Before opening a
pull request, run from the repository root:

```
cmake --preset default
cmake --build --preset default
ctest --preset default
cmake --build --preset default --target format-check
cmake --build --preset default --target analyze
cmake --build --preset default --target prose-check
```

The `analyze` target runs the clang static analyzer and must be clean for any
change under `src/`. The `format` target applies the project's clang-format
configuration; CI rejects a pull request that the `format-check` target fails
on. The `prose-check` target needs Vale and is described under
[Prose checks](#prose-checks).

## Code

### Language

LibXtract is written in **C99**. Declarations are placed at the top of the
block (C89 style) as a project convention; both the standard and the
convention are enforced by the build flags (`-std=c99 -pedantic
-Wdeclaration-after-statement -Wstrict-prototypes`). Thread-local storage
uses a portability macro that resolves to C11 `_Thread_local`,
`__declspec(thread)` (MSVC) or `__thread` (GCC/Clang).

### Initialisation

Initialise a variable at its declaration when the initial value is known and
meaningful (accumulators to zero, copies of parameters); otherwise declare it
uninitialised and assign on every path before use. Never defensively zero a
variable that is unconditionally assigned later: dead stores hide
missing-assignment bugs from the compiler. This is checked by
`-Wconditional-uninitialized` (clang builds) and by the `analyze` target,
which runs the clang static analyzer over the first-party sources and fails
on any finding. CI runs it on Linux and macOS.

### Formatting

Formatting follows the Allman brace style (the opening brace on its own
line) with four-space indentation, no tabs, and the pointer asterisk bound
to the variable (`const double *data`). It is the standard clang-format
`Microsoft` preset, captured in `.clang-format` (the only deviations are
disabling include sorting and line reflow). Build the `format-check` target
(or run `clang-format --dry-run --Werror <file>`) to check, and the `format`
target (or `clang-format -i <file>`) to apply it. CI runs clang-format
20.1.8, and other versions format some constructs differently; configure
with `-DXTRACT_CLANG_FORMAT=<path>` to run the targets with a chosen binary.

### Conventions

- Feature functions share the signature
  `int xtract_foo(const double *data, const int N, const void *argv, double *result)`
  and return an `XTRACT_` status code.
- Public declarations in `include/xtract/` carry a Doxygen block that states
  the contract: what the caller passes, what comes back, and the rules for
  using it. Leave the implementation out of the block, and match the
  wording of the surrounding blocks.
- Comments in `.c` files are block comments, and each one says something the
  code does not: a formula, a citation, a hidden constraint. Do not describe
  what the code used to do or why a change was made; that belongs in the
  commit message.
- Tests exercise the production code path against values derived from the
  documented formulae, not values copied from the current output.
- Bundled third-party code under `src/`, `tests/utest.h`, `bench/ubench.h`
  and `examples/simpletest/dr_wav.h` keeps its upstream style and licence.
  Do not reformat it or add a project header to it.

## Pull requests

- Branch from `develop` and name the branch `<type>/<short-description>`, where
  type is `feat`, `fix`, `chore` or `refactor`.
- Prefix the pull request title the same way: `fix: ...`, `feat: ...`.
- If the pull request resolves an issue, put `Closes #N` in the description
  so the issue closes on merge.
- Fill in the template. The Summary opens with "This PR" and a verb in lower
  case, and states what the change does and why in plain prose. Changes is
  one bullet per change.
- Keep a pull request to one change. A fix and an unrelated refactor are two
  pull requests.
- The maintainer merges. Do not merge your own pull request.

## Changelog

`CHANGELOG.md` records what each release changes for someone who uses the
library. Its entries are written in the pull request that makes the release,
so no other pull request touches it. The format is
[Common Changelog](https://common-changelog.org):

- A release is a `## [X.Y.Z] - YYYY-MM-DD` heading, newest first, with a
  compare link at the foot of the file.
- Under the heading come `###` sections from Changed, Added, Removed and
  Fixed, in that order. A release may carry one notice, a single line in
  italics directly under its heading, and has no other introductory text.
- An entry is one line: a phrase of at most 20 words with no closing full
  stop.
- An entry opens with a verb in the imperative, such as Add, Change, Fix or
  Remove. The verbs that the check accepts are listed under
  [Changelog format](https://github.com/jamiebullock/vale-style#changelog-format)
  in the vale-style README.
- An entry ends with a link to its pull request, in parentheses:
  `([#123](https://github.com/jamiebullock/LibXtract/pull/123))`.
- A change that breaks a caller's code or build opens with `**Breaking:**`
  and a lower-case verb, and comes first in its section.
- An entry says what changed for the user. The reason, the mechanism and the
  history are in the pull request.
- Record only what a user can observe: the API, behaviour, results, the
  build and install, the licence. Leave out CI, tests, tooling and
  refactoring.
- List a new function by name. Its arguments are documented in the header.
- A fix that changes a result or a return code for a call that already
  worked goes under Changed, because a caller may depend on the old value.
  Fixed holds crashes, memory errors and results that were unusable.
- A deprecation goes under Changed and opens with Deprecate.
- Correct or narrow an entry that is inaccurate. Do not add exceptions and
  qualifications to it.

An entry that explains the mechanism:

```
- Fix the phase from `xtract_spectrum` having the opposite sign on Linux
  and Windows, where the FFT returns the complex conjugate of the transform
  computed on macOS. The imaginary part is now negated there so that every
  platform agrees.
```

The same change as it belongs in the file:

```
- Change the phase sign of `xtract_spectrum` on Linux and Windows to match macOS ([#246](https://github.com/jamiebullock/LibXtract/pull/246))
```

An entry that restates the header:

```
- Add `xtract_median(data, N, NULL, result)`, which returns the median of
  the N values, or the mean of the two middle values when N is even.
```

The same change as it belongs in the file:

```
- Add `xtract_median` ([#229](https://github.com/jamiebullock/LibXtract/pull/229))
```

The `prose-check` target tests the rules that a script can test, and the
`version-check` target tests that the newest release is the version in
`VERSION`. CI runs both on every pull request.

## Prose checks

[Vale](https://vale.sh) checks the prose in the tree: the Markdown files,
the changelog, the comments in the sources and the licence headers. The
rules come from the [vale-style](https://github.com/jamiebullock/vale-style)
package, at the release that `.vale.ini` names, and the package's README
describes them. `.vale.ini` adds the project's own settings to them:

- `.vale/styles/config/vocabularies/LibXtract/accept.txt` holds the terms of
  the field and the names of authors that the dictionary does not. Add a
  word there when Vale reports a correct spelling.
- `.vale/styles/LibXtract/` holds the rules that apply to this project only.
- The bundled third-party code is not checked.

The `prose-check` target downloads the package and checks every file that
git tracks. An error fails the check, and CI rejects a pull request that it
fails on. A warning is reported and does not fail it. CI runs Vale 3.26.0;
configure with `-DXTRACT_VALE=<path>` to run the target with a chosen
binary.

CI also checks the title and the description of a pull request: the title
as the subject of a commit message, and the description against the
template. To run the same check before opening the pull request, write the
title to a file named `COMMIT_EDITMSG` and the description to a file named
`PR_BODY.md`, both outside the working tree, and pass the two paths to
`vale` from the repository root.

## Licensing of contributions

LibXtract is licensed under the zlib licence (see `LICENSE`). By submitting a
contribution, whether as a pull request, a patch or a code suggestion in an
issue, you agree that:

1. **Origin.** You wrote the contribution, or you have the right to submit it
   under the terms below. If it includes code you did not write, you say so
   in the pull request and identify its source and licence.
2. **Licence.** Your contribution is licensed under the zlib licence, the same
   terms as the rest of the project.
3. **Relicensing.** You grant the project's primary copyright holder,
   Jamie Bullock, permission to relicense your contribution, together with
   the rest of the project, under any licence approved by the Open Source
   Initiative. This exists so that the project can move to a different open
   source licence in future without tracking down every past contributor.
   It does not permit relicensing under proprietary terms.

You keep the copyright in your contribution. Nothing here is an assignment.

If you cannot agree to these terms, for example because your employer holds
the rights to your work, say so in the pull request and we will work out
whether the contribution can be accepted.

## Contributions made with AI tools

Contributions produced with the help of a code-generation tool are accepted
on the same terms. You are responsible for them exactly as if you had typed
them: you must have reviewed and understood the code, it must meet the
standards above, and the origin clause applies to any third-party code the
tool may have reproduced. Note in the pull request which tool was used.
