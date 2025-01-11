# Simple Mistake Identifier Lookup Engine

 This is a simple C++ software used to identify spelling mistakes of binaries typed in the bash shell, and correct them by suggesting a similar binary currently installed in the operative system.

---

### External Dependencies

- **[nlohmann/json](https://github.com/nlohmann/json)**: C++ JSON library for parsing JSON data.
- **[spdlog](https://github.com/gabime/spdlog)**: C++ logging library.
- **[boost/program_options](https://github.com/boostorg/program_options/tree/develop)**: C++ parameter handler.
- **[SQLiteCpp](https://github.com/SRombauts/SQLiteCpp)**: C++ SQLite3 wrapper.
---

### Installing Dependencies


   On Debian-based systems:

   ```bash
   sudo apt-get install nlohmann-json3-dev libspdlog-dev libboost-all-dev libsqlitecpp-dev
   ```

### Steps of the algorithm:
1) An incorrect command is executed on the shell.
2) The system cannot find the binary in the directories listed in the ***$PATH*** and triggers the ***command_not_found_handle()*** function.
3) The *SMILE* program is executed, passing the not-found command as an argument.
4) *SMILE* scans the system for installed binaries by examining directories specified in its configuration file
5) If enabled on the settings file, the program uses an heuristic approach to filter those binaries of which length and individual characters mismatches exceed a specified threshold.
6) For each resulting found binary, *SMILE* calculates the [Damerau-Leveshtein distance](https://en.wikipedia.org/wiki/Damerau%E2%80%93Levenshtein_distance).
7) Binaries with a Damerau-Levenshtein distance within the defined threshold are suggested to the user as potential corrections.

---

### Configuration
*SMILE* uses a `settings.json` file to define its behavior. Below is an example configuration:

```JSON
{
  "databaseHistoryStorageEnabled": false,   // WIP
  "ignoreMntFromSystemPathVariables": true, // Ignores paths that starts with /mnt/ from the lookup of binaries
  "lengthConditionHeuristic": 2,            // Heuristic length condition to apply prior to the Damerau-Leveshtein distance calculation
  "lengthConditionHeuristicEnabled": true,
  "systemBinariesPath": [                   // List of path for the binaries lookup
    "/usr/local/sbin",
    "/usr/local/bin",
    "/usr/sbin",
    "/usr/bin",
    "/sbin",
    "/bin",
    "/usr/games",
    "/usr/local/games"
  ]
}
```