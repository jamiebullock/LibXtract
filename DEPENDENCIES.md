# Dependencies

The library depends on nothing outside the C99 standard library, plus the
Accelerate framework on macOS. The components below are vendored into the tree
and keep their upstream licence and style. None of them is covered by
`LICENSE`.

## Vendored

### Ooura FFT

- Location: `src/ooura/`
- Source: [Takuya Ooura, General Purpose FFT Package](https://www.kurims.kyoto-u.ac.jp/~ooura/fft.html),
  the one-dimensional package `fft.tgz`
- Version: the package dated 2006-12-28. It has no version number
- Revision: `fft.tgz` with SHA-256
  `52bb637c70b971958ec79c9c8752b1df5ff0218a4db4510e60826e0cb79b5296`.
  `fftsg.c` differs from the file in that archive in white space only.
  `fftsg.h` is this project's declaration of its functions
- Licence: Ooura's terms, quoted under Obligations. No SPDX identifier
- Copyright: Copyright(C) 1996-2001 Takuya OOURA
- Used for: the FFT
- Platforms: Linux and Windows. On macOS the file is compiled and Accelerate
  does the transforms

### dywapitchtrack

- Location: `src/dywapitchtrack/`
- Source: Antoine Schmitt, Dynamic Wavelet Algorithm Pitch Tracking library.
  The files do not give an address
- Version: none recorded in the files
- Revision: none recorded. The copy differs from the one first vendored in
  three ways: the sample buffer parameters are `const`, `abs` is `fabs`, and
  one product is cast to `double`
- Licence: MIT
- Copyright: Copyright (c) 2010 Antoine Schmitt
- Used for: `xtract_wavelet_f0`
- Platforms: all

### c-ringbuf

- Location: `src/c-ringbuf/`
- Source: [dhess/c-ringbuf](https://github.com/dhess/c-ringbuf)
- Version: none. Upstream has no releases
- Revision: derived from
  [`f6648c7`](https://github.com/dhess/c-ringbuf/tree/f6648c7a772011d143dfd1614e53aea3c5d27909).
  `ringbuf_memcpy_from` takes a flag that leaves the buffer's contents in
  place, `ringbuf_read`, `ringbuf_write` and the POSIX headers they need are
  removed, `ringbuf_nextp` asserts a buffer size above zero, and the file
  defines `MIN` where the platform does not
- Licence: CC0-1.0
- Copyright: none claimed. Written in 2011 by Drew Hess and dedicated to the
  public domain
- Used for: `xtract_last_n`
- Platforms: all

### utest.h

- Location: `tests/utest.h`
- Source: [sheredom/utest.h](https://github.com/sheredom/utest.h)
- Version: none. Upstream has no releases
- Revision: [`b9c006f`](https://github.com/sheredom/utest.h/blob/b9c006f31b266408bd53079ce7ad57939a237d9a/utest.h),
  unmodified
- Licence: Unlicense
- Copyright: none claimed. Written by Neil Henning and dedicated to the public
  domain
- Used for: the tests. The library does not contain it
- Platforms: all

### ubench.h

- Location: `bench/ubench.h`
- Source: [sheredom/ubench.h](https://github.com/sheredom/ubench.h)
- Version: none. Upstream has no releases
- Revision: [`2afbd4c`](https://github.com/sheredom/ubench.h/blob/2afbd4ce1108ba2801b5acdc1131da1b4c573a6d/ubench.h),
  unmodified
- Licence: Unlicense
- Copyright: none claimed. Written by Neil Henning and dedicated to the public
  domain
- Used for: the benchmarks. The library does not contain it
- Platforms: all

### dr_wav

- Location: `examples/simpletest/dr_wav.h`
- Source: [mackron/dr_libs](https://github.com/mackron/dr_libs)
- Version: 0.14.5
- Revision: tag `wav-0.14.5`, commit
  [`fa931f3`](https://github.com/mackron/dr_libs/blob/fa931f3285ced10ace628f7f1ac951e1951e7ea6/dr_wav.h),
  unmodified
- Licence: Unlicense or MIT-0, at the user's choice
- Copyright: Copyright 2023 David Reid, under the MIT-0 alternative
- Used for: reading the WAV file in the `simpletest` example. The library
  does not contain it
- Platforms: all

## Obligations

- **dywapitchtrack (MIT).** The copyright and permission notice at the top of
  `src/dywapitchtrack/dywapitchtrack.c` must be included in all copies or
  substantial portions, which includes binary distributions of any build
  that links it. It is the only vendored component whose notice follows
  binaries.
- **Ooura FFT.** The source file carries no licence header. The package's
  `readme.txt` says: "You may use, copy, modify this code for any purpose and
  without fee. You may distribute this ORIGINAL package." The package's web
  page says: "You may use, copy, modify and distribute this code for any
  purpose (include commercial use) and without fee. Please refer to this
  package when you modify this code." The Source link above is that
  reference.
- **c-ringbuf, utest.h, ubench.h, dr_wav.** Public-domain dedications; no
  obligations.
