#include "numbers.dat"
#include <iostream>
#include <vector>

std::vector<int> prime (int max)
{
	std::vector<int> prime (max + 1, 1);


	prime[0] = prime[1] = 0;
	for(int i = 2; i * i <= max; ++i)
	{
		if (prime[i])
		{
			for(int j = i * i; j <= max; j += i)
				prime[j] = 0;
		}
	}
	return prime;
}

int counter(int begin, int end)
{
	int counter = 0;
	size_t i = 0;
	std::vector<int> a = prime(end);

	while(Data[i] <= end)
	{
		while((Data[i] >= begin) && (Data[i] <= end))
		{
			if(a[Data[i]])
				++counter;
			++i;
		}
		++i;
	}
	return counter;
}

int main(int argc, char* argv[])
{	
	if(argc % 2 == 0 || argc == 1) return -1;

	for(int i = 1; i < argc; ++i)
	{
		int v = std::atoi(argv[i]);
		int j = 0;

		while(Data[j] <= v) ++j;
		
		if(Data[j - 1] != v)
		{
			std::cout << '0' << std::endl;
		return 0;
		}
	}

	for(int i = 1; i < argc; i += 2)
	{
		std::cout << counter(std::atoi(argv[i]), std::atoi(argv[i + 1])) << std::endl; 
	}

	return 0;
}