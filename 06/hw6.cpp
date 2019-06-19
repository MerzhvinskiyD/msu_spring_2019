#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <thread>

using namespace std;

mutex m;
condition_variable c;

void sort(const string& input_name, size_t size,const string& output_name) // sorting
{
	ifstream in(input_name, ios::binary | ios::in);
	ofstream out(output_name, ios::binary | ios::out);

	if(!in.is_open() or !out.is_open())
	{
		cout << "Cannot open file." << endl;
		return;
	}

	vector<uint64_t> a(size);

	for(int i=0; i<size; i++)
		in >> a[i];

	in.close();

	std::sort(a.begin(), a.end());


	for(int i=0; i<size; i++)
		out << a[i];

	out.close();
}

void split(ifstream& in, size_t size, ofstream& out1, ofstream& out2) //split of file
{
	uint64_t a;
	int i=0;

	while(1){
		m.lock();
		if(!(in >> a)){
			m.unlock();
			break;
		}
		if(i < size)
			out1 << a;
		else
			out2 << a;   
		++i; 
		m.unlock();
	}
}

void merge_(ifstream& in, ofstream& out, int64_t& bar) //merging of files
{
	int64_t a;
	int it;

	while(1){
		unique_lock<mutex> lock(m);

		if(!(in >> a)){
			if(bar != -1){
				out << bar;
				bar = -1;
			}
			c.notify_one();
			break;
		}
		if(bar == -1){
			out << a;
			continue;
		}
		if(a <= bar)
			out << a;
		else{
			out << bar;
			bar = a;
			c.notify_one();
			if(bar==a)
				c.wait(lock);
		}
	}
}

void merge_sort(const string& filename, int& iter)
{
	constexpr size_t max_size = 8 * 1024 * 1024 / sizeof(uint64_t) / 2;
	size_t size = 0;
	int64_t bar;

	string output_name = filename;

	if(iter == 0)
		output_name = "output.bin";
	++iter;

	uint64_t a;

	ifstream in(filename, ios::binary | ios::in);
	if(!in.is_open())
	{
		cout << "Can not open file." << endl;
		return;
	}

	while(in >> a)
		++size;

	if(size <= max_size)
		sort(filename, size, output_name);
	else{

		string fn1 = to_string(iter) + "1.bin";
		string fn2 = to_string(iter) + "2.bin";

		ifstream in(filename, ios::binary | ios::in);
		ofstream out1(fn1, ios::binary | ios::out);
		ofstream out2(fn2, ios::binary | ios::out);

		if(!out1.is_open() or !out2.is_open())
		{
			cout << "Can not open file." << endl;
			return;
		}

		thread s1(split, ref(in), max_size, ref(out1), ref(out2));
		thread s2(split, ref(in), max_size, ref(out1), ref(out2));

		s1.join();
		s2.join();
		in.close();
		out1.close();
		out2.close();

		merge_sort(fn1, iter);
		merge_sort(fn2, iter);

		ifstream in1(fn1, ios::binary | ios::in);
		ifstream in2(fn2, ios::binary | ios::in);
		ofstream out(output_name, ios::binary | ios::out);

		if(!in1.is_open() or !in2.is_open() or !out.is_open())
		{
			cout << "Can not open file." << endl;
			return;
		}

		in2 >> bar;

		thread m1(merge_, ref(in1), ref(out), ref(bar));
		thread m2(merge_, ref(in2), ref(out), ref(bar));

		m1.join();
		m2.join();
		in1.close();
		in2.close();
		out.close();

		const char* file1 = fn1.c_str();
		const char* file2 = fn2.c_str();
		std::remove(file1);
		std::remove(file2);
	}
}

int main()
{
	int iter=0;
	merge_sort("input.bin", iter);

	return 0;
}