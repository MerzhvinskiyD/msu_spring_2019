#include <fstream>
#include <iostream>
#include <vector>
#include <algorithm>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <iterator>

using namespace std;

vector<string> sort_(const string& input_name, size_t size) // sorting and splitting
{
    vector<string> file_names;

    ifstream in(input_name, ios::binary | ios::in);

    if(!in)
        throw runtime_error("Can not open file");

    size_t idx = 0;

    while (in) {

        string output_file_name = "prepare_" + to_string(idx) + ".bin";
        ofstream out(output_file_name, ios::binary | ios::out);

        if (!out)
            throw runtime_error("Cannot open file");

        vector<uint64_t> a;
        uint64_t c;

        for(int i=0; i<size; ++i)
        {
            in.read((char*)(&c), sizeof(uint64_t));
            a.push_back(c);
        }

        sort(a.begin(), a.end());

        for(int i=0; i<size; ++i)
            out.write((char*)(&a[i]), sizeof(uint64_t));

        out.close();

        file_names.push_back(output_file_name);
        ++idx;
    }

    in.close();
    return file_names;
}

void merge_(const string& input_1, const string& input_2, const string& output)// merging
{
    ifstream in1(input_1, ios::binary | ios::in);
    ifstream in2(input_2, ios::binary | ios::in);
    ofstream out(output, ios::binary | ios::out);

    if (!in1 or !in2 or !out)
        throw runtime_error("Can not open file");

    uint64_t a, b;
    in1.read((char*)(&a), sizeof(uint64_t));
    in2.read((char*)(&b), sizeof(uint64_t));
    while(in1 and in2)
    {
        if (a > b)
        {
            out.write((char*)(&b), sizeof(uint64_t));
            in2.read((char*)(&b), sizeof(uint64_t));
        }
        else
        {
            out.write((char*)(&a), sizeof(uint64_t));
            in1.read((char*)(&a), sizeof(uint64_t));
        }
    }

    if (!in1)
        while (in2)
        {
            out.write((char*)(&b), sizeof(uint64_t));
            in2.read((char*)(&b), sizeof(uint64_t));
        }
    else
         while (in1)
        {
            out.write((char*)(&b), sizeof(uint64_t));
            in1.read((char*)(&b), sizeof(uint64_t));
        }

    in1.close();
    in2.close();
    out.close();
    remove(input_1.c_str());
    remove(input_2.c_str());
}


void merge_function(vector<string> names, const string& output, size_t thread_index)
{

    size_t idx = 0;

    while (names.size() != 1)
    {
        vector<string> tmp_names;

        for (int i = 0; i < names.size(); ++i)
        {
            if (i + 1 < names.size()) {
                string output_file_name = "prepared" + to_string(thread_index) + to_string(idx) + to_string(i) + ".bin";
                merge_(names[i], names[i + 1], output_file_name);
                tmp_names.push_back(output_file_name);
                ++i;
            } else {
                tmp_names.push_back(names[i]);
            }
        }

        ++idx;
        swap(names, tmp_names);
    }

    rename(names[0].c_str(), output.c_str());
}


void external_multithread_sort(const string& input, const string& output)
{
    constexpr size_t max_size = 1000;
    auto file_names = sort_(input, max_size);

    if(file_names.size() == 1)
    {
        rename(file_names[0].c_str(), output.c_str());
        return;
    }

    string first_part = "prepared_part_1.bin";
    string second_part = "prepared_part_2.bin";

    vector<string> first_names_part(file_names.begin(), file_names.begin() + file_names.size() / 2);
    vector<string> second_names_part(file_names.begin() + file_names.size() / 2, file_names.end());

    thread thread_1(merge_function, move(first_names_part), first_part, 1);
    thread thread_2(merge_function, move(second_names_part), second_part, 2);
    thread_1.join();
    thread_2.join();

    merge_(first_part, second_part, output);
}

int main()
{
    external_multithread_sort("input.bin", "output.bin");

    return 0;
}