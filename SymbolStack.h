#ifndef SYMBOL_STACK_H_
#define SYMBOL_STACK_H_

#include <iostream>
#include <map>
#include <string>
#include <queue>
#include <pthread.h>
#include <cmath>
#include <fstream>
#include <cstdlib>
#include <stdio.h>
#include <string.h>
#include <sstream>

#define PRICE_PRECISION 2

using namespace std;

class logger {
	public:
	static pthread_mutex_t logLock;
	static string numToStr(int num) {
		ostringstream oOStrStream;
		oOStrStream << num;
		return oOStrStream.str();
	}
	static void log(string logStr)	{
		pthread_mutex_lock(&logLock);
		cout << logStr << endl;
		pthread_mutex_unlock(&logLock);
	}
};

struct OrderKey{
    unsigned int px;
    unsigned long long priority;
    OrderKey(unsigned int px, unsigned long long priority) {
        this->px = px;
        this->priority = priority;
    }
};

struct BuyOrderKeyComp {
   bool operator() (const OrderKey& lhs, const OrderKey& rhs) const {
           if(lhs.px > rhs.px) {
                   return true;
           } else if(lhs.px < rhs.px) {
                   return false;
           } else {
                   return (lhs.priority < rhs.priority);
           }
           return true;
   }
};

struct SellOrderKeyComp
{
   bool operator() (const OrderKey& lhs, const OrderKey& rhs) const
   {
           if(lhs.px > rhs.px) {
                   return false;
           } else if(lhs.px < rhs.px) {
                   return true;
           } else {
                   return (lhs.priority < rhs.priority);
           }
           return true;

   }
};

class Order {
    public:
        Order(string sym, bool side, int qnty, double px, string userId) :
            sym(sym), side(side), qnty(qnty), px(px), userId(userId), key(0,0) {}
        bool isBuy() { return side; }
        OrderKey generateOrderPriorityKey(unsigned long long priority);
        unsigned int getPriceKey() { return px * (pow(10,PRICE_PRECISION)); }
        void setPriorityKey(OrderKey ordKey) { key = ordKey; }
        OrderKey getPriorityKey() { return key; }
        bool isSameSide(Order *order) { return side == order->side; }
        double getPx() { return px; }
        unsigned int getQnty() { return qnty; }
        void setQnty(unsigned int setQnty) { qnty = setQnty; }
        string &getUserId() { return userId; }
        string &getSymbol() { return sym; }
    private:
        OrderKey key;
        string sym;
        bool side;
        unsigned int qnty;
        double px;
        string userId;
};

typedef map <OrderKey, Order *, BuyOrderKeyComp> BuyOrderBook;
typedef map <OrderKey, Order *, BuyOrderKeyComp>::iterator BuyOrderBookItr;
typedef map <OrderKey, Order *, SellOrderKeyComp> SellOrderBook;
typedef map <OrderKey, Order *, SellOrderKeyComp>::iterator SellOrderBookItr;

class SymbolStack {
    public:
        SymbolStack();
        ~SymbolStack();
        int queueOrder(Order *order);
        Order * fetchOrder();
        void match(Order *order);
        friend void * run(void *);
        void matchBuySide(Order *order);
        void matchSellSide(Order *order);
        unsigned long long getQueueCount();
        void insertOrderInOrderBook(Order *order);
        void removeOrderFromOrderBook(Order *order);
        bool init();
        void join();
    private:
        pthread_t thread;
        pthread_mutex_t orderQueueLock;
        pthread_cond_t haltMatch;
        queue<Order *> orderQueue;
        BuyOrderBook buyBook;
        SellOrderBook sellBook;
        unsigned long long priorityCntr;
};

#endif
