#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
using namespace std;

// Structure to represent a Trade
struct TradeEntry
{
    string makerOrderId;
    string takerOrderId;
    float price;
    int size;
};

// Structure to represent an order entry in Order Book
struct OrderBookEntry
{
    string bidAsk; // Whether the order is ASK or BID
    float price;
    int size;
    string orderId;
};

void handleAdd(vector<string> order);
void handleDelete(vector<string> order);
void addTrade(string makerOrderId, string takerOrderId, float price, float size);
vector<string> createModifiedOrder(const OrderBookEntry &entryItem, const string &companyName, const string &bidAsk);

// Class to manage the Order Book for a company
class OrderBook
{
public:
    string companyName;
    vector<OrderBookEntry> entries; // Order Entries

    // Constructor Function, to create Order Book Object for a company, with the first order entry
    OrderBook(string company, vector<OrderBookEntry> newEntry)
    {
        companyName = company;
        entries = newEntry;
    }

    // Class method to add new Entry
    void appendEntry(OrderBookEntry entryItem) { entries.push_back(entryItem); }

    // Class Method to execute trades initated by an ASK order
    void executeAskTrade(OrderBookEntry entryItem, string id)
    {
        int entry_id; // buyer order id
        for (int i = 0; i < entries.size(); i++)
        {
            if (entries[i].orderId.compare(id) == 0)
            {
                entry_id = i;
            }
        }

        // seller is able to partially satify the requirements
        if (entryItem.size > entries[entry_id].size)
        {
            entryItem.size -= entries[entry_id].size;
            addTrade(entryItem.orderId, entries[entry_id].orderId, entryItem.price, entries[entry_id].size);
            deleteOrder(entries[entry_id].orderId);

            // modify the original order to find another order or store the updated order
            vector<string> modifiedOrder = createModifiedOrder(entryItem, companyName, "ASK");
            handleAdd(modifiedOrder);
        }

        // seller is able to satify the buyer requirements and has surprlus stocks
        else if (entryItem.size < entries[entry_id].size)
        {
            entries[entry_id].size -= entryItem.size;
            addTrade(entryItem.orderId, entries[entry_id].orderId, entryItem.price, entries[entry_id].size);
        }

        // no surplus stocks
        else if (entryItem.size == entries[entry_id].size)
        {
            deleteOrder(entries[entry_id].orderId);
            addTrade(entryItem.orderId, entries[entry_id].orderId, entryItem.price, entries[entry_id].size);
        }

        saveOrderBook();
    }

    // Class Method to execute trades initated by a BID order
    void executeBidTrade(OrderBookEntry entryItem, string id)
    {
        int entry_id; // seller_id
        for (int i = 0; i < entries.size(); i++)
        {
            if (entries[i].orderId.compare(id) == 0)
            {
                entry_id = i;
            }
        }

        // if the given order is not able to satisy the requirements
        if (entries[entry_id].size < entryItem.size)
        {
            entryItem.size -= entries[entry_id].size;
            addTrade(entryItem.orderId, entries[entry_id].orderId, entryItem.price, entries[entry_id].size);
            deleteOrder(entries[entry_id].orderId);

            // modify the original order to find another order or store the updated order
            vector<string> modifiedOrder = createModifiedOrder(entryItem, companyName, "BID");
            handleAdd(modifiedOrder);
        }

        else if (entries[entry_id].size > entryItem.size)
        {
            entries[entry_id].size -= entryItem.size;
            addTrade(entryItem.orderId, entries[entry_id].orderId, entryItem.price, entryItem.size);
        }

        else if (entryItem.size == entries[entry_id].size)
        {
            deleteOrder(entries[entry_id].orderId);
            addTrade(entryItem.orderId, entries[entry_id].orderId, entryItem.price, entries[entry_id].size);
        }
        saveOrderBook();
    }

    // Class Method to Delete an Existing Order
    void deleteOrder(string orderId)
    {
        vector<OrderBookEntry>::iterator it;

        bool orderFound = false;

        for (int i = 0; i < entries.size(); i++)
        {
            if (entries[i].orderId.compare(orderId) == 0)
            {
                orderFound = true;
                it = entries.begin() + i;
                entries.erase(it);
                break;
            }
        }

        if (orderFound)
        {
            saveOrderBook();
        }
        else
        {
            cout << "No Order Found!" << endl;
        }
    }

    // Class Method to open the company's Order Book and update and save
    void saveOrderBook()
    {
        ofstream outfile;
        outfile.open(companyName + ".txt");

        if (outfile.is_open())
        {
            outfile << "Company Name: " << companyName << endl
                    << endl;
            outfile << "BID/ASK"
                    << "   "
                    << "PRICE"
                    << "   "
                    << "SIZE"
                    << "   "
                    << "ORDER ID" << endl;
            for (int i = 0; i < entries.size(); ++i)
            {
                outfile << "(" << entries[i].bidAsk << ")    " << entries[i].price << "   " << entries[i].size << "      " << entries[i].orderId << endl;
            }
            outfile.close();
        }
        else
        {
            cout << "File Not Found!" << endl;
        }
    }
};

vector<OrderBook> database; // Vector with type OrderBook to store the data of all the comapnies
vector<TradeEntry> trades;  // Vector wtih type TradeEntry to store the data of all the trades occured

// Returns a Modified Order for the system to handle. Basically, to handle the partial fulfilled orders
vector<string> createModifiedOrder(const OrderBookEntry &entryItem, const string &companyName, const string &bidAsk)
{
    vector<string> modifiedOrder;
    modifiedOrder.push_back("ADD");
    modifiedOrder.push_back(entryItem.orderId);
    modifiedOrder.push_back(to_string(entryItem.price));
    modifiedOrder.push_back(to_string(entryItem.size));
    modifiedOrder.push_back(companyName);
    modifiedOrder.push_back(bidAsk);

    return modifiedOrder;
}

// Add the trades to trades vector and also saves the data to the Trade.txt
void addTrade(string makerOrderId, string takerOrderId, float price, float size)
{
    TradeEntry entry;
    entry.makerOrderId = makerOrderId;
    entry.takerOrderId = takerOrderId;
    entry.price = price;
    entry.size = size;
    trades.push_back(entry);

    ofstream tradeOutfile;
    tradeOutfile.open("TRADES.txt");
    if (tradeOutfile.is_open())
    {

        for (int i = 0; i < trades.size(); i++)
        {
            tradeOutfile << "TRADE " << trades[i].makerOrderId << " " << trades[i].takerOrderId << " " << trades[i].price << " " << trades[i].size << endl;
        }
        tradeOutfile.close();
    }
}

// Handles the ADD Orders
void handleAdd(vector<string> order)
{
    string companyName = order[4];

    // create an order Book Entry from this:
    OrderBookEntry orderBookItem;
    orderBookItem.bidAsk = order[5];
    orderBookItem.orderId = order[1];
    orderBookItem.price = stof(order[2]);
    orderBookItem.size = stoi(order[3]);

    // Creates an empty Order Book for the company
    vector<OrderBookEntry> newBook;
    newBook.push_back(orderBookItem);

    // Create a new Company Order Book
    OrderBook companyBook(companyName, newBook);

    bool companyFound = false;
    int database_id;

    // Attempt to find a pre-existing company book, if exisits
    for (int i = 0; i < database.size(); i++)
    {
        if (database[i].companyName.compare(companyName) == 0)
        {
            database_id = i;
            companyBook = database[i];
            companyFound = true;
            break;
        }
    }

    if (companyFound == false)
    {
        database.push_back(companyBook);
        companyBook.saveOrderBook();
    }

    else
    {
        if (order[5].compare("BID") == 0)
        {
            bool askFound = false; // to check if there are any suitable sellers
            for (int i = 0; i < database[database_id].entries.size(); i++)
            {
                if (database[database_id].entries[i].bidAsk.compare("ASK") == 0 && database[database_id].entries[i].price <= orderBookItem.price)
                {
                    askFound = true; // seller found, trade will occur
                    database[database_id].executeBidTrade(orderBookItem, database[database_id].entries[i].orderId);
                }
            }
            if (askFound == false)
            {
                database[database_id].appendEntry(orderBookItem); // trade will not occur, so wil store this order in the order book
                database[database_id].saveOrderBook();
            }
        }

        else if (order[5].compare("ASK") == 0)
        {
            bool buyer_found = false; // to check if there are any buyers

            for (int i = 0; i < database[database_id].entries.size(); i++)
            {
                if (database[database_id].entries[i].bidAsk.compare("BID") == 0 && database[database_id].entries[i].price >= orderBookItem.price)
                {
                    buyer_found = true; // buyer fond, so trade will occur
                    database[database_id].executeAskTrade(orderBookItem, database[database_id].entries[i].orderId);
                }
            }
            if (buyer_found == false)
            {
                database[database_id].appendEntry(orderBookItem); // trade will not occur, so wil store this order in the order book
                database[database_id].saveOrderBook();
            }
        }
    }
}

// Handles the DELETE Orders
void handleDelete(vector<string> order)
{
    string inputOrderId = order[2];
    string companyName = order[1];

    bool companyFound = false;

    for (int i = 0; i < database.size(); i++)
    {
        if (database[i].companyName.compare(companyName) == 0)
        {
            companyFound = true;
            database[i].deleteOrder(inputOrderId);
            break;
        }
    }

    if (companyFound == false)
    {
        // Company Not Found
        cout << "Company " << companyName << "Not Found!" << endl;
    }
}

int main()
{
    ifstream file("orders.txt");
    string line;
    if (file.is_open())
    {
        while (getline(file, line))
        {
            stringstream ss(line);
            vector<string> order;
            string word;
            while (ss >> word)
                order.push_back(word);
            if (order[0].compare("ADD") == 0)
            {
                handleAdd(order);
            }
            else if (order[0].compare("DELETE") == 0)
            {
                handleDelete(order);
            }
        }

        file.close();
    }
}