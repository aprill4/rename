# Rename

A brief introduction

## Compilation

## Add to path

## Usage

## matching files

2D map representes parsing table

### int or char

`std::map<int, std::map<int, Item*> > parsing_table`
`std::map<char, std::map<char, Item*> > parsing_table`

- [ ] int 
  - pros: easy to use, like parsing[0][s_a]
  - cons: filename is a string, when computing I need to convert char to int to find in map.

- [x] char
  - pros: no need to convert char to int when finding item in parsing table
  - cons: need to add quotes to index, but not a big deal.

### a and b and c ?

- compare a and the first len(a) chars of filename, if they are th e same, then jump cur_char point to the len(a)th position of filename

- compare c and the last len(c) chars of filename, if they are the same, then don't visit the last len(c) chars
  - no!

- b can be any char
