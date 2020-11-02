#include "global.h"
/**
 * @brief 
 * SYNTAX: LOAD relation_name
 */
bool syntacticParseINSERT()
{
    logger.log("syntacticParseINSERT");
        if (tokenizedQuery.size() != 4 || tokenizedQuery[1]!="INTO" || tokenizedQuery[3]!="VALUES")
        {
            cout << "SYNTAX ERROR" << endl;
            return false;
        }
        parsedQuery.queryType = INSERT;
        parsedQuery.loadRelationName = tokenizedQuery[2];
        parsedQuery.values=tokenizedQuery[4];
        return true;
}

bool semanticParseINSERT()
{
    logger.log("semanticParseINSERT");
    if (!tableCatalogue.isTable(parsedQuery.loadRelationName))
    {
        cout << "Table doesn't exists" << endl;
        return false;
    }
    vector<int>row_values;
    string token;
    stringstream s(parsedQuery.values);
    while(getline(s,token,','))
    {
        row_values.push_back(stoi(token));
    }
    Table *tb=tableCatalogue.getTable(parsedQuery.loadRelationName);
    if(tb->columnCount!=row_values.size())
    {
     cout << "Number of values specified doesn't match with original table" << endl;
        return false;   
    }
    return true;
}

void executeINSERT()
{
    logger.log("executeINSERT");

    vector<int>row_values;
    string token;
    stringstream s(parsedQuery.values);
    while(getline(s,token,','))
    {
        row_values.push_back(stoi(token));
    }
    Table *table=tableCatalogue.getTable(parsedQuery.loadRelationName);
    //Inserting row in table
    if(table->insert(row_values))
    {
       cout << "Inserted values. Column Count: " << table->columnCount << " Row Count: " << table->rowCount << endl;
    }
    //Inserting row into Index if IndexingStrategy is HASH
    if(table->indexingStrategy==HASH)
    {
       table->hash(row_values,table->bucket_size,table->columnCount,-1);
    }
    return ;
}
