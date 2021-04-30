# pwl2limodsat

**pwl2limodsat** is a builder of representations modulo satisfiability of piecewise linear functions into Łukasiewicz Infinitely-valued Logic.

## Theoretical References

Preto, S. and Finger, M. 2020. An efficient algorithm for representing piecewise linear functions into logic. *Electronic Notes
in Theoretical Computer Science*, 351:167–186. Proceedings of LSFA 2020, the 15th International Workshop on Logical
and Semantic Frameworks, with Applications (LSFA 2020).
[doi.org/10.1016/j.entcs.2020.08.009](https://doi.org/10.1016/j.entcs.2020.08.009)

Finger, M. and Preto, S. 2020. Probably partially true: Satisfiability for Łukasiewicz infinitely-valued probabilistic logic and related topics. *Journal of Automated Reasoning*, 64(7):1269–1286.
[doi.org/10.1007/s10817-020-09558-9](https://doi.org/10.1007/s10817-020-09558-9)

## Usage

In the following command, **pwl2limodsat** take as input file *file*.pwl and produces output *file*.out:

> pwl2limodsat file.pwl

A .pwl file may have two configurations: for truncated linear functions or for general rational McNaughton functions in regional format. Also, commented linear begin with *c*. Examples are found in folder *tests/*.
