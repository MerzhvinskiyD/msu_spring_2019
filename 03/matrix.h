class Matrix
{
public:
	class ProxyMatrix
	{
	public:
		ProxyMatrix(size_t cols, int* data): cols(cols), pr_data(data) {}

		int const operator[](size_t col) const
		{	
			if((col > cols - 1) or (cols == 0))
				throw std::out_of_range("");
			return pr_data[col];
		}

		int& operator[](size_t col)
		{
			if((col > cols - 1) or (cols == 0))
				throw std::out_of_range("");
			return pr_data[col];
		}

	private:
		size_t cols;
		int* pr_data;
	};

	Matrix(size_t rows, size_t cols):  rows(rows), cols(cols), data(new int[rows * cols]) {}

	~Matrix()
	{
		delete[] data;
	}

	size_t getRows() const
	{
		return rows;
	}

	size_t getColumns() const
	{
		return cols;
	}

	ProxyMatrix operator[](size_t row)
	{
		if(row > rows - 1 or rows == 0)
			throw std::out_of_range("");

		return ProxyMatrix(cols, &data[cols * row]);
	}

	ProxyMatrix operator[](size_t row) const
	{
		if(row > rows - 1 or rows == 0)
			throw std::out_of_range("");
		return ProxyMatrix(cols, &data[cols * row]);
	}

	bool operator==(const Matrix& other) const
	{
		if((cols != other.cols) or (rows != other.rows))
			return false;

		if (this == &other)
			return true;

		for(size_t i = 0; i < rows * cols; ++i)
		{
			if(data[i] != other.data[i])
				return false;
		}

		return true;
	}

	bool operator!=(const Matrix& other) const
	{
		return !(*this == other);
	}

	Matrix& operator*=(int m)
	{
		for(size_t i = 0; i < rows * cols; ++i)
			data[i] *= m;
		return *this;
	}
	
private:
	const size_t rows;
	const size_t cols;
	int* data;
};
