//1. Put this file in the "codam" folder of the "main" server folder.
//2. Add the below line to "modlist.gsc":
//[[ register ]]("SQLite test", codam\sqlitetest::main);

main(phase, register)
{
    switch(phase)
    {
        case "init": _init(register); break;
    }
}
_init(register)
{
    if(isDefined(level.sqlitetest))
        return;
    level.sqlitetest = true;

    [[ register ]]("StartGameType", ::doTests, "thread");
}

doTests(a0, a1, a2, a3, a4, a5, a6, a7, a8, a9, b0, b1, b2, b2, b4, b5, b6, b7, b8, b9)
{
    printLn("####### doTests");

    dbName = "my_database.db";
    tableName = "my_table";
    playerName = "Mike";
    playerRank = "Pro";
    dbId = sqlite_open(dbName); // Will be created if doesn't exist

    if(dbId)
    {
        printLn("####### Opened database " + dbName + ", ID = " + dbId);
        
        query_checkTableExists = "SELECT name FROM sqlite_master WHERE type='table' AND name='" + tableName + "';";
        response_checkTableExists = sqlite_query(dbId, query_checkTableExists);
        if(isDefined(response_checkTableExists))
        {
            if(isDefined(response_checkTableExists[0]))
            {
                for(i = 0; i < response_checkTableExists.size; i++)
                {
                    printLn("####### response_checkTableExists[i] = " + response_checkTableExists[0][i]);

                    if(response_checkTableExists[0][i] == tableName)
                    {
                        printLn("####### Found table " + response_checkTableExists[0][i]);

                        // Retrieve a row
                        query_retrieveRow = "SELECT rank FROM " + tableName + " WHERE name = '" + playerName + "'";
                        response_retrieveRow = sqlite_query(dbId, query_retrieveRow);

                        if(isDefined(response_retrieveRow))
                        {
                            if(isDefined(response_retrieveRow[0]))
                            {
                                for(i = 0; i < response_retrieveRow.size; i++)
                                {
                                    printLn("####### response_retrieveRow[i] = " + response_retrieveRow[0][i]);
                                }
                            }
                            else
                            {
                                printLn("####### NO response_retrieveRow[0]");
                            }
                        }
                        else
                        {
                            printLn("####### NO isDefined response_retrieveRow");
                        }
                    }
                }
            }
            else
            {
                printLn("####### NO response_checkTableExists[0]");
                // Table doesn't exist

                // Create a table
                query_createTable = "CREATE TABLE " + tableName + " (id INTEGER PRIMARY KEY, name TEXT, rank TEXT);";
                sqlite_query(dbId, query_createTable);

                // Insert a row
                query_insertRow = "INSERT INTO my_table (name, rank) VALUES ('" + playerName + "', '" + playerRank + "');";
                sqlite_query(dbId, query_insertRow);

                printLn("####### Attempted to create a table and insert a row, please restart the server to check the result.");
            }
        }
        else
        {
            printLn("####### NO isDefined response_checkTableExists");
        }
    }
}
