#include "SymbolStack.h"

using namespace std;

pthread_mutex_t logger::logLock = PTHREAD_MUTEX_INITIALIZER;

int main()
{
    map<string, SymbolStack*> SymbolStackCollection;
    map<string, SymbolStack*>::iterator symbolStackItr;

    string line;
    ifstream myfile ("orders.txt");
    while (getline(myfile, line)) {

        vector<string> data;
        char * pch = strtok ((char *)line.c_str(), ",");
        while(pch != NULL) {
            data.push_back(string(pch));
            pch = strtok(NULL, ",");
        }

        bool side = (data[1] == "B") ? true : false;
        unsigned int qnty = atoi(data[2].c_str());
        double px = atof(data[3].c_str());
        Order *order = new Order(data[0].c_str(), side, qnty, px, data[4].c_str());

        //cout << "Received Order : Symbol[" << data[0] << "], Side[" << data[1] << "], Qnty[" << data[2] << "], Px[" << data[3] << "], UserId[" << data[4] << "]" << endl;
		logger::log(string("Received Order : Symbol[" + data[0] + "], Side[" + data[1] + "], Qnty[" + data[2] + "], Px[" + data[3] + "], UserId[" + data[4] + "]"));

        string &symbol = order->getSymbol();
        symbolStackItr = SymbolStackCollection.find(symbol);
        if(symbolStackItr == SymbolStackCollection.end()) {
            //cout << "Creating a new symbol stach for symbol[" << symbol << "]" << endl;
            logger::log(string("Creating a new symbol stack for symbol[" + symbol + "]"));
            //create a new symbolStack
            SymbolStack *s = new SymbolStack();
            SymbolStackCollection[symbol] = s;
            if(!s->init()) {
                //cout << "ERROR : Cannot create matching thread." << endl;
                logger::log(string("ERROR : Cannot create matching thread."));
            }
        }

        SymbolStackCollection[symbol]->queueOrder(order);
    }

    //wait for all the threads to finish
    for(symbolStackItr = SymbolStackCollection.begin(); symbolStackItr != SymbolStackCollection.end(); symbolStackItr++) {
        (symbolStackItr->second)->join();
    }

    return 0;
}
