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
    std::string insertCommandQuery;

    /* ---------- UPDATE ----------*/
    std::string updateNewCommandUseQuery;

    /* ---------- DELETE ----------*/
    std::string deleteCommandQuery;

    template <typename... Args>
    SQLite::Statement prepareStatement (const std::string& command, Args&&... args) {
        SQLite::Statement query(*db, command);
        bindArguments(query, 1, std::forward<Args>(args)...);
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

        /* ---------- CREATE ----------*/
        createHistoryTableQuery = "CREATE TABLE IF NOT EXISTS history (                     \
                                    command VARCHAR(256) PRIMARY KEY,                       \
                                    execution_counter INTEGER DEFAULT 0 NOT NULL,           \
                                    last_execution TEXT DEFAULT 'N/A')";

        /* ---------- SELECT ----------*/
        getAllQuery = "SELECT * FROM history";

        getCommandQuery = "SELECT * FROM history WHERE command = '?'";

        /* ---------- INSERT ----------*/
        insertCommandQuery = "INSERT INTO history VALUES(?, 0, current_timestamp)";

        /* ---------- UPDATE ----------*/
        updateNewCommandUseQuery = "UPDATE history SET (execution_counter = (SELECT execution_counter FROM history WHERE command = ?), last_execution = current_timestamp) \
                                    WHERE command = ?";

        /* ---------- DELETE ----------*/
        deleteCommandQuery = "DELETE FROM history WHERE command = ?";
    }

    SQLite::Statement createHistoryTable() { return prepareStatement(createHistoryTableQuery); }
    SQLite::Statement getAll() { return prepareStatement(getAllQuery); }
    SQLite::Statement getCommand() { return prepareStatement(getCommandQuery); }
    SQLite::Statement getInsertCommandQuery() { return prepareStatement(insertCommandQuery); }
    SQLite::Statement getuUpdateNewCommandUseQuery() { return prepareStatement(updateNewCommandUseQuery); }
    SQLite::Statement getuDeleteCommandQuery() { return prepareStatement(deleteCommandQuery); }
};