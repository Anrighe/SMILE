# Simple Mistake Identifier Lookup Engine

 This is a simple C++ software used to identify spelling mistakes of binaries typed in the bash shell, and correct them by suggesting a similar binary currently installed in the operative system.

---

### External Dependencies

- **[nlohmann/json](https://github.com/nlohmann/json)**: C++ JSON library for parsing JSON data.
- **[spdlog](https://github.com/gabime/spdlog)**: C++ logging library.
- **[boost/program_options](https://github.com/boostorg/program_options/tree/develop)**: C++ parameter handler.
---

### Installing Dependencies


   On Debian-based systems:

   ```bash
   sudo apt-get install nlohmann-json3-dev libspdlog-dev libboost-all-dev
   ```

---

### Steps of the algorithm:
1) An incorrect command (do I also want to consider built-ins? Still need to decide) is executed on the shell.
2) The binary couldn't be found in the ***$PATH*** variable, consequently the ***command_not_found_handle()*** function is executed.
3) A new version of this function runs the *SMILE* program.
4) *SMILE* searches which binaries is currently installed in the system.
5) For each binary in the system, the [Jaccard similarity coefficient](https://en.wikipedia.org/wiki/Jaccard_index).
6) For all results of the Jaccard similarity coefficient computation, if they are greater than a specific threshold, the [Damerau–Levenshtein](https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance) distance will be calculated between the user inserted command and the current binary.
7) For all results of the Damerau–Levenshtein computation, if the edit distance is less than a specific threshold, they will be suggested to the user.