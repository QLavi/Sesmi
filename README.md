# Sesmi
Regex Parser and NFA generator and simulator

## Build
```bash
$ make
$ make dot # Run the program and generate AST
$ python3 run_tests.py # Run regex tests and check if it crashes or not
```

## Notes
Sesmi Parses Regular expressions (only chars, |, ?, +, * operators supported)\
and generates Abstract Syntax Tree which later generates Non-Deterministic Finite Automaton\
for the regex.
AST and NFA visualization is done by generating code for Graphviz's Dot Language
NOTE: NFA visualization support is removed and will be added later.

## TODO:
- [x] generate NFA using AST
- [x] simulate NFA to do pattern matching
- [ ] do subset construction to generate DFA from NFA
- [ ] simulate DFA to do pattern matching
