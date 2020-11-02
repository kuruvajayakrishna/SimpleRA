#include "global.h"

/**
 * @brief Construct a new Table:: Table object
 *
 */
Table::Table()
{
    logger.log("Table::Table");
}

/**
 * @brief Construct a new Table:: Table object used in the case where the data
 * file is available and LOAD command has been called. This command should be
 * followed by calling the load function;
 *
 * @param tableName 
 */
Table::Table(string tableName)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/" + tableName + ".csv";
    this->tableName = tableName;
}

/**
 * @brief Construct a new Table:: Table object used when an assignment command
 * is encountered. To create the table object both the table name and the
 * columns the table holds should be specified.
 *
 * @param tableName 
 * @param columns 
 */
Table::Table(string tableName, vector<string> columns)
{
    logger.log("Table::Table");
    this->sourceFileName = "../data/temp/" + tableName + ".csv";
    this->tableName = tableName;
    this->columns = columns;
    this->columnCount = columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (4 * columnCount));
    this->writeRow<string>(columns);
}

/**
 * @brief The load function is used when the LOAD command is encountered. It
 * reads data from the source file, splits it into blocks and updates table
 * statistics.
 *
 * @return true if the table has been successfully loaded 
 * @return false if an error occurred 
 */
bool Table::load()
{
    logger.log("Table::load");
    fstream fin(this->sourceFileName, ios::in);
    string line;
    if (getline(fin, line))
    {
        fin.close();
        if (this->extractColumnNames(line))
            if (this->blockify())
                return true;
    }
    fin.close();
    return false;
}

/**
 * @brief Function extracts column names from the header line of the .csv data
 * file. 
 *
 * @param line 
 * @return true if column names successfully extracted (i.e. no column name
 * repeats)
 * @return false otherwise
 */
bool Table::extractColumnNames(string firstLine)
{
    logger.log("Table::extractColumnNames");
    unordered_set<string> columnNames;
    string word;
    stringstream s(firstLine);
    while (getline(s, word, ','))
    {
        word.erase(std::remove_if(word.begin(), word.end(), ::isspace), word.end());
        if (columnNames.count(word))
            return false;
        columnNames.insert(word);
        this->columns.emplace_back(word);
    }
    this->columnCount = this->columns.size();
    this->maxRowsPerBlock = (uint)((BLOCK_SIZE * 1000) / (4 * this->columnCount));
    return true;
}

/**
 * @brief This function splits all the rows and stores them in multiple files of
 * one block size. 
 *
 * @return true if successfully blockified
 * @return false otherwise
 */
bool Table::blockify()
{
    logger.log("Table::blockify");
    ifstream fin(this->sourceFileName, ios::in);
    string line, word;
    vector<int> row(this->columnCount, 0);
    vector<vector<int>> rowsInPage(this->maxRowsPerBlock, row);
    int pageCounter = 0;
    unordered_set<int> dummy;
    dummy.clear();
    this->distinctValuesInColumns.assign(this->columnCount, dummy);
    this->distinctValuesPerColumnCount.assign(this->columnCount, 0);
    getline(fin, line);
    while (getline(fin, line))
    {
        stringstream s(line);
        for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
        {
            if (!getline(s, word, ','))
                return false;
            row[columnCounter] = stoi(word);
            rowsInPage[pageCounter][columnCounter] = row[columnCounter];
        }
        pageCounter++;
        this->updateStatistics(row);
        if (pageCounter == this->maxRowsPerBlock)
        {
            bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
            this->blockCount++;
            this->rowsPerBlockCount.emplace_back(pageCounter);
            pageCounter = 0;
        }
    }
    if (pageCounter)
    {
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, pageCounter);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(pageCounter);
        pageCounter = 0;
    }

    if (this->rowCount == 0)
        return false;
    this->distinctValuesInColumns.clear();
    return true;
}

/**
 * @brief Given a row of values, this function will update the statistics it
 * stores i.e. it updates the number of rows that are present in the column and
 * the number of distinct values present in each column. These statistics are to
 * be used during optimisation.
 *
 * @param row 
 */
void Table::updateStatistics(vector<int> row)
{
    this->rowCount++;
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (!this->distinctValuesInColumns[columnCounter].count(row[columnCounter]))
        {
            this->distinctValuesInColumns[columnCounter].insert(row[columnCounter]);
            this->distinctValuesPerColumnCount[columnCounter]++;
        }
    }
}

/**
 * @brief Checks if the given column is present in this table.
 *
 * @param columnName 
 * @return true 
 * @return false 
 */
bool Table::isColumn(string columnName)
{
    logger.log("Table::isColumn");
    for (auto col : this->columns)
    {
        if (col == columnName)
        {
            return true;
        }
    }
    return false;
}

/**
 * @brief Renames the column indicated by fromColumnName to toColumnName. It is
 * assumed that checks such as the existence of fromColumnName and the non prior
 * existence of toColumnName are done.
 *
 * @param fromColumnName 
 * @param toColumnName 
 */
void Table::renameColumn(string fromColumnName, string toColumnName)
{
    logger.log("Table::renameColumn");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (columns[columnCounter] == fromColumnName)
        {
            columns[columnCounter] = toColumnName;
            break;
        }
    }
    return;
}

/**
 * @brief Function prints the first few rows of the table. If the table contains
 * more rows than PRINT_COUNT, exactly PRINT_COUNT rows are printed, else all
 * the rows are printed.
 *
 */
void Table::print()
{
    logger.log("Table::print");
    uint count = min((long long)PRINT_COUNT, this->rowCount);

    //print headings
    this->writeRow(this->columns, cout);

    Cursor cursor(this->tableName, 0,0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, cout);
    }
    printRowCount(this->rowCount);
}

void Table::linear_hashing(string column_name,long long M)
{
    logger.log("Table::linear_hashing");
    this->split_ptr=0;
    this->bucket_size=M;
    for(int i=0;i<M;i++)
    {
       string bucket_name=this->tableName+"_"+"bucket"+"_"+to_string(i);
       this->bucket_list.push_back({bucket_name,{}});
    }
    this->indexedColumn=column_name;
    for(int i=0;i<columns.size();i++)
    {
        if(this->indexedColumn==columns[i])
        {
            this->column_index=i;
            break;
        }
    }
    uint count = min((long long)PRINT_COUNT, this->rowCount);
    Cursor cursor(this->tableName, 0,0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < count; rowCounter++)
    {
        row = cursor.getNext();
        long long bucket_no=hash(row,this->bucket_size,count,-1);
    }
}
vector<string> Table::get_bucket_entries(string bucket_name)
{
    ifstream fin("../data/temp/"+bucket_name,ios::out);
    vector<string>rows;
    string line;
    while(getline(fin,line))
    {
       rows.push_back(line);
    }
    fin.close();
    return rows;
}
long long Table::hash(vector<int>row,long long hash_value,uint count,long long prev_index)
{
  long long bucket_no = row[this->column_index]%hash_value;
  if(prev_index==bucket_no) return bucket_no;
  if(bucket_no < split_ptr)
  {
     return hash(row,2*hash_value,count,prev_index);
  }
  pair<string,vector<string>> bucket_info=bucket_list[bucket_no];
  vector<string>rows=get_bucket_entries(bucket_info.first);
  if(rows.size() >= this->bucket_capacity)//overflow
  {
    vector<string>overflow_buckets=bucket_info.second;
    bool insert_flag=false;
    for(int i=0;i<overflow_buckets.size();i++)
      {
          string bucket_name=overflow_buckets[i];
          vector<string>rows=get_bucket_entries(bucket_name);
          if(rows.size()< bucket_capacity)
          {
                   
                   ofstream fout("../data/temp/"+bucket_name,ios::app);
                   //cout<<"11"<<endl;
                   for(int i=0;i<row.size();i++)
                   {
                       if(i!=0)
                         fout<<",";
                       //cout<<row[i]<<",";
                       fout<<row[i];
                   }
                   //cout<<endl;
                   fout<<endl;
                   fout.close();
                   insert_flag=true;
                   break;
          }
      }   
      if(insert_flag==false)
      {
          string new_overflow_bucket=bucket_list[bucket_no].first+"_"+"overflow"+"_"+to_string(overflow_buckets.size());
          overflow_buckets.push_back(new_overflow_bucket);
          bucket_list[bucket_no].second.push_back(new_overflow_bucket);
          ofstream fout("../data/temp/"+new_overflow_bucket,ios::out);
          //cout<<"22"<<" "<<row.size()<<" "<<count<<endl;
          for(int i=0;i<row.size();i++)
             {
                 if(i!=0)
                   fout<<",";
                 cout<<row[i]<<",";
                 fout<<row[i];
              }
              cout<<endl;
           fout<<endl;
           fout.close();
      }
    string bucket_name=this->tableName+"_"+"bucket"+"_"+to_string(this->bucket_size+this->split_ptr);
    cout<<bucket_name<<endl;
    this->bucket_list.push_back({bucket_name,{}});
    re_hashing(split_ptr,2*this->bucket_size,count);
    split_ptr=(split_ptr+1)%this->bucket_size;
    if(split_ptr==0)
    {
        this->bucket_size=2*this->bucket_size;
    }
  }
  else
  {
     string bucket_name=bucket_info.first;
     ofstream foutput("../data/temp/"+bucket_name,ios::app);
     for(int i=0;i<row.size();i++)
     {
         if (i != 0)
            foutput << ",";
         foutput << row[i];
     }
     foutput<<endl;
     foutput.close();
  }
  return bucket_no;
}
bool Table::update_bucket_entries(long long bucket_no,long long overflow_bucket_no,vector<int>row)
{
    pair<string,vector<string>>bucket_info=this->bucket_list[bucket_no];
    vector<string>rows=get_bucket_entries(bucket_info.first);
    if(rows.size()< this->bucket_capacity)
    {
        string bucket_name=bucket_info.first;
        ofstream foutput("../data/temp/"+bucket_name,ios::app);
        for(int i=0;i<row.size();i++)
        {
            if (i != 0)
                foutput << ",";
            foutput << row[i];
        }
        foutput<<endl;
        foutput.close();
        return true;
    }
    else
    {
        vector<string>overflow_buckets=bucket_info.second;
        for(int i=0;i<overflow_bucket_no;i++)
        {
            vector<string>rows=get_bucket_entries(overflow_buckets[i]);
            if(rows.size()< this->bucket_capacity)
            {
                ofstream foutput("../data/temp/"+overflow_buckets[i],ios::app);
                for(int i=0;i<row.size();i++)
                {
                    if (i != 0)
                        foutput << ",";
                    foutput << row[i];
                }
                foutput<<endl;
                foutput.close();
                return true;
            }
        }
    }
    return false;
    
}
void Table::re_hashing(long long bucket_no,long long hash_value,uint count)
{
    pair<string,vector<string>>bucket_info=this->bucket_list[bucket_no];
    vector<string>rows=get_bucket_entries(bucket_info.first);  
    for(int i=0;i<rows.size();)
    {
        vector<int>row;
        stringstream s(rows[i]);
        string token;
        while(getline(s,token,','))
        {
            row.push_back(stoi(token));
        }
        if(bucket_no==hash(row,hash_value,count,bucket_no))
        {
           i++;
        }
        else
        {
            rows.erase(rows.begin()+i);
        }
    }
     ofstream foutput("../data/temp/"+bucket_info.first,ios::out);
     for(int i=0;i<rows.size();i++)
     {
        // cout<<rows[i]<<endl;
         foutput << rows[i];
         foutput<<endl;
     }
     foutput.close();
    vector<string>overflow_buckets=bucket_info.second;
    int i=0;
    while(i<overflow_buckets.size())
    {
        cout<<"entered rehasing of overflow buckets"<<endl;
        string overflow_bucket_name=overflow_buckets[i];
        vector<string>rows=get_bucket_entries(overflow_bucket_name);
        for(int j=0;j<rows.size();)
        {
          cout<<rows[i]<<endl;
          vector<int>row;
          stringstream s(rows[i]);
          string token;
          while(getline(s,token,','))
          {
            row.push_back(stoi(token));
          }
          if(bucket_no==hash(row,hash_value,count,bucket_no))
           {
              if(update_bucket_entries(bucket_no,i,row)){
                rows.erase(rows.begin()+j);
              }
              else j++;
           }
           else
           {
            rows.erase(rows.begin()+j);
           }
        }
        if(rows.size()==0)
        {
            ofstream foutput("../data/temp/"+overflow_bucket_name,ios::out);
            for(int i=0;i<rows.size();i++)
            {
                foutput << rows[i];
                foutput<<endl;
            }
            foutput.close();
            overflow_buckets.erase(overflow_buckets.begin()+i);
            //std::filesystem::remove(overflow_bucket_name);
        }
        else{
            ofstream foutput("../data/temp/"+overflow_bucket_name,ios::out);
            for(int i=0;i<rows.size();i++)
            {
                foutput << rows[i];
                foutput<<endl;
            }
            foutput.close();
        i++;
        }
    }
    bucket_list[bucket_no].second=overflow_buckets;
}
/**
 * @brief This function helps in inserting values into table
 * 
 * 
 * 
*/
bool Table::insert(vector<int>row)
{
    vector<uint>rows_per_block=this->rowsPerBlockCount;
    //Incase if last page is not filled completely
    if(rows_per_block[rows_per_block.size()-1]< this->maxRowsPerBlock)
    {
        string page_name="../data/temp/"+this->tableName+ "_Page" + to_string(this->blockCount-1);
        ofstream fout(page_name,ios::app);
        for(int i=0;i<row.size();i++)
        {
            if(i!=0)
              fout<<", ";
            fout<<row[i];
        }
        fout<<endl;
        fout.close();
        this->rowCount++;
        this->rowsPerBlockCount[rows_per_block.size()-1]++;
        return true;
    }
    else // If all pages are filled i.e rows==rows_per_block  then create new page and insert values
    {
        vector<vector<int>>rowsInPage;
        rowsInPage.push_back(row);
        bufferManager.writePage(this->tableName, this->blockCount, rowsInPage, 1);
        this->blockCount++;
        this->rowsPerBlockCount.emplace_back(1);
    }
    return false;
}

/**
 * @brief This function returns one row of the table using the cursor object. It
 * returns an empty row is all rows have been read.
 *
 * @param cursor 
 * @return vector<int> 
 */
void Table::getNextPage(Cursor *cursor)
{
    logger.log("Table::getNext");

        if (cursor->pageIndex < this->blockCount - 1)
        {
            cursor->nextPage(cursor->pageIndex+1);
        }
}



/**
 * @brief called when EXPORT command is invoked to move source file to "data"
 * folder.
 *
 */
void Table::makePermanent()
{
    logger.log("Table::makePermanent");
    if(!this->isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
    string newSourceFile = "../data/" + this->tableName + ".csv";
    ofstream fout(newSourceFile, ios::out);

    //print headings
    this->writeRow(this->columns, fout);

    Cursor cursor(this->tableName, 0,0);
    vector<int> row;
    for (int rowCounter = 0; rowCounter < this->rowCount; rowCounter++)
    {
        row = cursor.getNext();
        this->writeRow(row, fout);
    }
    fout.close();
}

/**
 * @brief Function to check if table is already exported
 *
 * @return true if exported
 * @return false otherwise
 */
bool Table::isPermanent()
{
    logger.log("Table::isPermanent");
    if (this->sourceFileName == "../data/" + this->tableName + ".csv")
    return true;
    return false;
}

/**
 * @brief The unload function removes the table from the database by deleting
 * all temporary files created as part of this table
 *
 */
void Table::unload(){
    logger.log("Table::~unload");
    for (int pageCounter = 0; pageCounter < this->blockCount; pageCounter++)
        bufferManager.deleteFile(this->tableName, pageCounter);
    if (!isPermanent())
        bufferManager.deleteFile(this->sourceFileName);
}

/**
 * @brief Function that returns a cursor that reads rows from this table
 * 
 * @return Cursor 
 */
Cursor Table::getCursor()
{
    logger.log("Table::getCursor");
    Cursor cursor(this->tableName, 0,0);
    return cursor;
}
/**
 * @brief Function that returns the index of column indicated by columnName
 * 
 * @param columnName 
 * @return int 
 */
int Table::getColumnIndex(string columnName)
{
    logger.log("Table::getColumnIndex");
    for (int columnCounter = 0; columnCounter < this->columnCount; columnCounter++)
    {
        if (this->columns[columnCounter] == columnName)
            return columnCounter;
    }
}
