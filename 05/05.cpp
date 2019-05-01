#include <iostream>
#include <atomic>
#include <thread>

const size_t N = 500000;
std::atomic<bool> Ping_Pong(true);

void ping()
{
	for (size_t i = 0; i < N;)
	{
		while(Ping_Pong)
		{
			std::cout << "ping" << std::endl;
			i++;
			Ping_Pong = false;
		}
	}
}

void pong()
{
	for (size_t i = 0; i < N;)
	{
		while(!Ping_Pong)
		{
			std::cout << "pong" << std::endl;
			i++;
			Ping_Pong = true;
		}
	}
}

int main()
{
	std::thread t1(ping);
	std::thread t2(pong);

	t1.join();
	t2.join();

	return 0;
}
