#include "SymbolStack.h"

using namespace std;

OrderKey Order::generateOrderPriorityKey(unsigned long long priority) {
    OrderKey ordKey = OrderKey(getPriceKey(), priority);
    setPriorityKey(ordKey);
    return ordKey;
}

SymbolStack::SymbolStack() : priorityCntr(0) {
    pthread_mutex_init(&orderQueueLock, NULL);
    pthread_cond_init(&haltMatch, NULL);
}

SymbolStack::~SymbolStack() {
    pthread_mutex_destroy(&orderQueueLock);
    pthread_cond_destroy(&haltMatch);
}

unsigned long long SymbolStack::getQueueCount() {
    return orderQueue.size();
}

Order * SymbolStack::fetchOrder() {
    pthread_mutex_lock(&orderQueueLock);
    while(!getQueueCount()) {
        pthread_cond_wait(&haltMatch, &orderQueueLock);
    }
    Order *order = orderQueue.front();
    orderQueue.pop();
    pthread_mutex_unlock(&orderQueueLock);
    return order;
}

void SymbolStack::insertOrderInOrderBook(Order *order) {
    OrderKey key = order->generateOrderPriorityKey(++priorityCntr);
    if(order->isBuy()) {
        buyBook[key] = order;
    } else {
        sellBook[key] = order;
    }
}

void SymbolStack::removeOrderFromOrderBook(Order *order) {
    OrderKey key = order->getPriorityKey();
    if(order->isBuy()) {
        buyBook.erase(key);
    } else {
        sellBook.erase(key);
    }
    delete order;
}

void SymbolStack::matchBuySide(Order *order) {
    SellOrderBook &oppoSideOrderBook = sellBook;

    SellOrderBookItr matchOrderItr = oppoSideOrderBook.begin();
    while(true) {

        //if no market depth
        if(matchOrderItr == oppoSideOrderBook.end()) {
            insertOrderInOrderBook(order);
            break;
        }

        Order *matchOrder = matchOrderItr->second;
        if(order->getPx() >= matchOrder->getPx()) { // Trades
            unsigned int orderQnty = order->getQnty();
            unsigned int matchOrderQnty = matchOrder->getQnty();
            if(orderQnty < matchOrderQnty) {
                //cout << "TRADE : Full Fill - Trader[" << order->getUserId() << "] for Qnty[" << orderQnty << "]" << endl;
                //cout << "TRADE : Partial Fill - Trader[" << matchOrder->getUserId() << "] for Qnty[" << orderQnty << "]" << endl;
                logger::log(string("TRADE : Full Fill - Trader[" + order->getUserId() + "] for Qnty[" + logger::numToStr(orderQnty) + "]"));
                logger::log(string("TRADE : Partial Fill - Trader[" + matchOrder->getUserId() + "] for Qnty[" + logger::numToStr(orderQnty) + "]"));
                //Reduce the qnty of the sell order in order book
                matchOrder->setQnty(matchOrderQnty - orderQnty);
                break;
            } else if(orderQnty == matchOrderQnty) {
                //cout << "TRADE : Full Fill - Trader[" << order->getUserId() << "] for Qnty[" << orderQnty << "]" << endl;
                //cout << "TRADE : Full Fill - Trader[" << matchOrder->getUserId() << "] for Qnty[" << orderQnty << "]" << endl;
                logger::log(string("TRADE : Full Fill - Trader[" + order->getUserId() + "] for Qnty[" + logger::numToStr(orderQnty) + "]"));
                logger::log(string("TRADE : Full Fill - Trader[" + matchOrder->getUserId() + "] for Qnty[" + logger::numToStr(orderQnty) + "]"));
                //remove the sell order from order book
                removeOrderFromOrderBook(matchOrder);
                break;
            } else { // orderQnty > matchOrderQnty
                //cout << "TRADE : Partial Fill - Trader[" << order->getUserId() << "] for Qnty[" << matchOrderQnty << "]" << endl;
                //cout << "TRADE : Full Fill - Trader[" << matchOrder->getUserId() << "] for Qnty[" << matchOrderQnty << "]" << endl;
                logger::log(string("TRADE : Partial Fill - Trader[" + order->getUserId() + "] for Qnty[" + logger::numToStr(matchOrderQnty) + "]"));
                logger::log(string("TRADE : Full Fill - Trader[" + matchOrder->getUserId() + "] for Qnty[" + logger::numToStr(matchOrderQnty) + "]"));
                removeOrderFromOrderBook(matchOrder);
                order->setQnty(orderQnty - matchOrderQnty);
                matchOrderItr = oppoSideOrderBook.begin();
                continue;
            }
        } else {
            //No match insert order in order book
            insertOrderInOrderBook(order);
            break;
        }
    }
}

void SymbolStack::matchSellSide(Order *order) {
    BuyOrderBook &oppoSideOrderBook = buyBook;

    BuyOrderBookItr matchOrderItr = oppoSideOrderBook.begin();
    while(true) {

        //if no market depth
        if(matchOrderItr == oppoSideOrderBook.end()) {
            insertOrderInOrderBook(order);
            break;
        }

        Order *matchOrder = matchOrderItr->second;
        if(order->getPx() <= matchOrder->getPx()) { // Trades
            unsigned int orderQnty = order->getQnty();
            unsigned int matchOrderQnty = matchOrder->getQnty();
            if(orderQnty < matchOrderQnty) {
                //cout << "TRADE : Full Fill - Trader[" << order->getUserId() << "] for Qnty[" << orderQnty << "]" << endl;
                //cout << "TRADE : Partial Fill - Trader[" << matchOrder->getUserId() << "] for Qnty[" << orderQnty << "]" << endl;
                logger::log(string("TRADE : Full Fill - Trader[" + order->getUserId() + "] for Qnty[" + logger::numToStr(orderQnty) + "]"));
                logger::log(string("TRADE : Partial Fill - Trader[" + matchOrder->getUserId() + "] for Qnty[" + logger::numToStr(orderQnty) + "]"));
                //Reduce the qnty of the sell order in order book
                matchOrder->setQnty(matchOrderQnty - orderQnty);
                break;
            } else if(orderQnty == matchOrderQnty) {
                //cout << "TRADE : Full Fill - Trader[" << order->getUserId() << "] for Qnty[" << orderQnty << "]" << endl;
                //cout << "TRADE : Full Fill - Trader[" << matchOrder->getUserId() << "] for Qnty[" << orderQnty << "]" << endl;
                logger::log(string("TRADE : Full Fill - Trader[" + order->getUserId() + "] for Qnty[" + logger::numToStr(orderQnty) + "]"));
                logger::log(string("TRADE : Full Fill - Trader[" + matchOrder->getUserId() + "] for Qnty[" + logger::numToStr(orderQnty) + "]"));
                //remove the sell order from order book
                removeOrderFromOrderBook(matchOrder);
                break;
            } else { // orderQnty > matchOrderQnty
                //cout << "TRADE : Partial Fill - Trader[" << order->getUserId() << "] for Qnty[" << matchOrderQnty << "]" << endl;
                //cout << "TRADE : Full Fill - Trader[" << matchOrder->getUserId() << "] for Qnty[" << matchOrderQnty << "]" << endl;
                logger::log(string("TRADE : Partial Fill - Trader[" + order->getUserId() + "] for Qnty[" + logger::numToStr(matchOrderQnty) + "]"));
                logger::log(string("TRADE : Full Fill - Trader[" + matchOrder->getUserId() + "] for Qnty[" + logger::numToStr(matchOrderQnty) + "]"));
                removeOrderFromOrderBook(matchOrder);
                order->setQnty(orderQnty - matchOrderQnty);
                matchOrderItr = oppoSideOrderBook.begin();
                continue;
            }
        } else {
            //No match insert order in order book
            insertOrderInOrderBook(order);
            break;
        }
    }
}

void SymbolStack::match(Order *order) {

    if(order->isBuy()) {
        matchBuySide(order);
    } else {
        matchSellSide(order);
    }
}

int SymbolStack::queueOrder(Order *order) {
    pthread_mutex_lock(&orderQueueLock);
    orderQueue.push(order);
    pthread_mutex_unlock(&orderQueueLock);

    pthread_cond_signal(&haltMatch);

    //TODO : calling run temporarily; later it will be called by thread
    //run(NULL);

    return 0;
}

void * run(void * obj_ptr) {
    while(1)
    {
        Order *order = ((SymbolStack *)obj_ptr)->fetchOrder();
        if(!order) {
            return NULL;
        }

        ((SymbolStack *)obj_ptr)->match(order);
    }
}

bool SymbolStack::init() {
    return (pthread_create(&thread, NULL, &run, this) == 0);
}

void SymbolStack::join() {
    (void) pthread_join(thread, NULL);
}

