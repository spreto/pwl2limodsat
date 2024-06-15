# pwl2limodsat

**pwl2limodsat** is a builder of representations modulo satisfiability of piecewise linear functions into Łukasiewicz Infinitely-valued Logic.

## Theoretical References

Preto, S. and Finger, M. 2020. An efficient algorithm for representing piecewise linear functions into logic. *Electronic Notes
in Theoretical Computer Science*, 351:167–186. Proceedings of LSFA 2020, the 15th International Workshop on Logical
and Semantic Frameworks, with Applications (LSFA 2020).
[doi.org/10.1016/j.entcs.2020.08.009](https://doi.org/10.1016/j.entcs.2020.08.009)

Finger, M. and Preto, S. 2020. Probably partially true: Satisfiability for Łukasiewicz infinitely-valued probabilistic logic and related topics. *Journal of Automated Reasoning*, 64(7):1269–1286.
[doi.org/10.1007/s10817-020-09558-9](https://doi.org/10.1007/s10817-020-09558-9)

## Installation

You must have in your computer the compilers **gcc** and **g++**, the **GMP** and **zlib** libraries and the **SoPlex** callable library [(soplex.zib.de)](https://soplex.zib.de/).

To compile **pwl2limodsat**, all you have to do is type, at the root of the distribution directory:

> $ make

To remove all compiled files, just type:

> $ make clean

## Usage

Typing the following command at the root of the distribution directory, **pwl2limodsat** takes as input file *file.pwl* and produces output *file.out*:

> $ ./bin/Release/pwl2limodsat file.pwl

A .pwl file may have two configurations: truncated linear functions and general rational McNaughton functions in regional format. Commented lines begin with *c*. Examples are found in folder *tests/*.

### Funding

This work was supported by grant #2021/03117-2, São Paulo Research Foundation (FAPESP).

This work was carried out at the Center for Artificial Intelligence (C4AI-USP), with support by the São Paulo Research Foundation (FAPESP), grant #2019/07665-4, and by the IBM Corporation.
