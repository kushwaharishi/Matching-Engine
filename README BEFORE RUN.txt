README

Implementation Internals
1. Creates seperate matching engine instance per symbol for load balancing.
2. Creates and maintains seperate order books for buy and sell sorted with best pricing and order time.
3. Matches orders on the basis of best price and order arrival time.
4. Main program reads the orders from the file and push them in the corresponding matching engine queue, each matching engine runs a seperate thread to read orders from the queue and matches it with the order book.


Capabilities
1. Reads orders from a file for trading.
2. Best price matching : Mathes orders on the basis of best price and order arrival time.
3. Load balancing : Creates seperate matching engine per symbol for load balacing.
4. Logs trade and partial trade events.
5. Multithreaded application for better throughput.


Notes
1. Haven't used smart pointers and didn't find a read need.
2. Simple logger with cout, after making it thread safe. This may cause some cost on speed
3. Orders do trade on the basis of best price and arrival timing but do not consider the userId