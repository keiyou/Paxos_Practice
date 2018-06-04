CXX=clang++
CXXFLAGS=-g -std=c++11 -Wall -pedantic

objects = main.o Server.o BankServer.o  ServerDecorator.o PaxosDecorator.o Operation.o BankServerInterface.o Utility.o PaxosUtility.o

server: $(objects)
	$(CXX) -pthread -o server $(CXXFLAGS) $(objects)

main.o: main.cpp Server.hpp BankServer.hpp PaxosDecorator.hpp BankServerInterface.hpp Utility.hpp
	$(CXX) -c $(CXXFLAGS) main.cpp

main1.o: main1.cpp Server.hpp BankServer.hpp PaxosDecorator.hpp BankServerInterface.hpp Utility.hpp
	$(CXX) -c $(CXXFLAGS) main1.cpp

main2.o: main2.cpp Server.hpp BankServer.hpp PaxosDecorator.hpp BankServerInterface.hpp Utility.hpp
	$(CXX) -c $(CXXFLAGS) main2.cpp

Server.o: Server.cpp Utility.hpp
	$(CXX) -c $(CXXFLAGS) Server.cpp

BankServer.o: BankServer.cpp Utility.hpp
	$(CXX) -c $(CXXFLAGS) BankServer.cpp

ServerDecorator.o: ServerDecorator.cpp Server.hpp
	$(CXX) -c $(CXXFLAGS) ServerDecorator.cpp

PaxosDecorator.o: PaxosDecorator.cpp ServerDecorator.hpp BlockChain.hpp LimitedQueue.hpp Operation.hpp Utility.hpp
	$(CXX) -c $(CXXFLAGS) PaxosDecorator.cpp

PaxosUtility.o: PaxosUtility.cpp ServerDecorator.hpp BlockChain.hpp LimitedQueue.hpp Operation.hpp Utility.hpp
	$(CXX) -c $(CXXFLAGS) PaxosUtility.cpp

Operation.o: Operation.cpp BankServer.hpp
	$(CXX) -c $(CXXFLAGS) Operation.cpp

BankServerInterface.o: BankServerInterface.cpp BankServer.hpp PaxosDecorator.hpp
	$(CXX) -c $(CXXFLAGS) BankServerInterface.cpp

Utility.o: Utility.cpp
	$(CXX) -c $(CXXFLAGS) Utility.cpp

.PHONY : clean

clean:
	rm server *.o
