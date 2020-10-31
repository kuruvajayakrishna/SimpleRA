#include"bufferManager.h"
/**
 * @brief The cursor is an important component of the system. To read from a
 * table, you need to initialize a cursor. The cursor reads rows from a page one
 * at a time.
 *
 */
class Cursor{
    public:
    Page page;
    int pageIndex;
    string entityName;
    int pagePointer;

    public:
    Cursor(string entityName, int pageIndex,int entityType);
    vector<int> getNext();
    void nextPage(int pageIndex);
    Page getPageContainingElement(int rowIndex,int colIndex);
    //int getNextElement();
};