README

Run
command: ./matching
This executable expects the orders.txt file present in the same dir containing the orders.
Please check the uploaded orders.txt file for the format.


Implementation Internals
1. Creates seperate matching engine instance per symbol for load balancing.
2. Creates and maintains seperate order books for buy and sell sorted with best price and order arrival time.
3. Matches orders on the basis of best price and order arrival time.
4. Each matching engine contains a queue which is populated by the main program, matching engines run a seperate thread to read orders from the queue and matches it with the order book.


Capabilities
1. Reads orders from a file as input.
2. Best price matching : Matches orders on the basis of best price and order arrival time to market.
3. Load balancing : Creates seperate matching engine instances per symbol for load balacing.
4. Logs order arrival, trades, partial trades events.
5. Multithreaded application for better throughput.
6. make file included


Notes
1. Haven't used smart pointers as didn't find an instense need.
2. Simple logger with cout, had to make it thread safe. This may cause some cost on speed.
3. Orders do trade on the basis of best price and arrival timing but does not consider the userId.
