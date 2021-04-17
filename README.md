# LogParser
Log parser implementation (test assignment as a part of hiring process).

## Discription
Console application that parses a text file that consists of a set of lines. Each line ends with a line feed character.
The analysis consists in finding strings that contain/do not contain (depending on the user's choice) the specified combinations of characters (also specified by the user). The number of filters you can set is unlimited. The result of the program (lines from the input file that satisfy the filters) will be saved to a separate file.

## Usage
LogParser.exe path-to-logging-file [-v][--invert-match] path-to-list-of-patterns

## Implementation details
- C++17
- Windows API
- the Boyer–Moore string-search algorithm https://en.wikipedia.org/wiki/Boyer%E2%80%93Moore_string-search_algorithm

## Known limitations
- platform dependency
- no regex search
- UTF-8 filepaths only
