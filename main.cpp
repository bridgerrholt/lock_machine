#include <iostream>
#include <string>
#include <thread>
#include <vector>

#include "include/lock_machine.h"

class OutputObj
{
	public:
		std::string prefix;

		void output(std::string const & str) const
		{
			std::cout << prefix << ": " << str << '\n';
		}
};


void OutputOnThreads(
	LockMachine & lockMachine, OutputObj const & outObj, std::size_t iterations)
{
	std::vector<std::thread> threads;
	threads.reserve(iterations);

	for (std::size_t i {0}; i < iterations; i++)
	{
		/*std::thread th {[&](){
			lockMachine.lockFunc(outObj, [&](){
				outObj.output(std::to_string(i));
			});
		}};*/

		threads.emplace_back([&lockMachine, &outObj, i](){
			lockMachine.lockFunc(outObj, [&](){
				outObj.output(std::to_string(i));
			});
		});
	}

	for (auto & t : threads)
	{
		t.join();
	}
}


int main()
{
	LockMachine lockMachine;

	OutputObj out1 {"abcdefghijklmnopqrsout1"}, out2 {"out2"};

	std::thread thread1 {
		OutputOnThreads, std::ref(lockMachine), std::ref(out1), 50};

	std::thread thread2 {
		OutputOnThreads, std::ref(lockMachine),std::ref(out2), 50};

	thread1.join();
	thread2.join();

	return 0;
}