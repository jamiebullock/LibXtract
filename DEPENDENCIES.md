# Dependencies

The library depends on nothing outside the C99 standard library, plus the
Accelerate framework on macOS. Everything below is vendored into the tree and
keeps its upstream licence and style; none of it is covered by `LICENSE`.

| Component | Location | Upstream | Licence | Used for |
| --- | --- | --- | --- | --- |
| Ooura FFT | `src/ooura/` | [Takuya Ooura, General Purpose FFT Package](https://www.kurims.kyoto-u.ac.jp/~ooura/fft.html) | Ooura's terms (see below) | FFT on Linux and Windows |
| dywapitchtrack | `src/dywapitchtrack/` | Antoine Schmitt, dynamic wavelet pitch tracker | MIT, Copyright (c) 2010 Antoine Schmitt | `xtract_wavelet_f0` |
| c-ringbuf | `src/c-ringbuf/` | [Drew Hess, c-ringbuf](https://github.com/dhess/c-ringbuf) | CC0-1.0 | `xtract_last_n_state` |
| utest.h | `tests/utest.h` | [Neil Henning, utest.h](https://github.com/sheredom/utest.h) | Unlicense | Tests only |
| ubench.h | `bench/ubench.h` | [Neil Henning, ubench.h](https://github.com/sheredom/ubench.h) | Unlicense | Benchmarks only |
| dr_wav | `examples/simpletest/dr_wav.h` | [David Reid, dr_libs](https://github.com/mackron/dr_libs), tag `wav-0.14.5` | Public domain (Unlicense) or MIT-0, at the user's choice | Example only |

## Obligations

- **dywapitchtrack (MIT).** The copyright and permission notice at the top of
  `src/dywapitchtrack/dywapitchtrack.c` must be included in all copies or
  substantial portions, which includes binary distributions of any build
  that links it. It is the only vendored component whose notice follows
  binaries.
- **Ooura FFT.** The package permits use, copying and modification for any
  purpose without fee, and asks that modified copies refer to the original
  package. The vendored `fftsg.c` is the upstream source re-braced to the
  project's style and is otherwise unmodified; the upstream link above is
  that reference. The package carries no licence header in the source file;
  its terms are in the upstream `readme.txt`.
- **c-ringbuf, utest.h, ubench.h, dr_wav.** Public-domain dedications; no
  obligations.
