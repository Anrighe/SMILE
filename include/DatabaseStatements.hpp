#include <string>
#include <SQLiteCpp/SQLiteCpp.h>


class DatabaseStatements {

private:

    SQLite::Database * db;

    /* ---------- CREATE ----------*/
    std::string createHistoryTableQuery;

    /* ---------- SELECT ----------*/
    std::string getAllQuery;
    std::string getCommandQuery;

    /* ---------- INSERT ----------*/


    /* ---------- DELETE ----------*/

    template <typename... Args>
    SQLite::Statement prepareStatement (const std::string& command, Args&&... args) {

        SQLite::Statement query(*db, command);
        std::cout<<"CREATED QUERY " + command + " \n";
        bindArguments(query, 1, std::forward<Args>(args)...);
        std::cout<<"RETURNING\n";
        return query;
    }

    void bindArguments(SQLite::Statement& query [[maybe_unused]], int index [[maybe_unused]]) { /* Base case: no more arguments to bind */ }

    template<typename T, typename... Args>
    void bindArguments(SQLite::Statement& query, int index, T&& firstArg, Args&&... remainingArgs) {
        query.bind(index, std::forward<T>(firstArg));
        bindArguments(query, index + 1, std::forward<Args>(remainingArgs)...);
    }

public:
    DatabaseStatements(SQLite::Database& database) { 
        db = &database; 

        createHistoryTableQuery = "CREATE TABLE IF NOT EXISTS history (                     \
                                    command VARCHAR(256) PRIMARY KEY,                       \
                                    execution_counter INTEGER DEFAULT 0 NOT NULL,           \
                                    last_execution TEXT DEFAULT 'N/A')";

        getAllQuery = "SELECT * FROM history";

        getCommandQuery = "SELECT * FROM history WHERE command = '?'";
    }

    SQLite::Statement createHistoryTable() { return prepareStatement(createHistoryTableQuery); }
    SQLite::Statement getAll() { return prepareStatement(getAllQuery); }
    SQLite::Statement getCommand() { return prepareStatement(getCommandQuery); }

};