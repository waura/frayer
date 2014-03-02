#include <cxxtest/TestSuite.h>

#include "UtilOpenCV.h"

class UtilOpenCVBltTestSuite : public CxxTest::TestSuite
{
public:
	UtilOpenCVBltTestSuite()
	{
	}

	~UtilOpenCVBltTestSuite()
	{
	}

	void test_ssememcpy()
	{
		//const int BUF_SIZE = 1000;
		//const int WRITE_SIZE = 4000000;
		//char* _src = new char[BUF_SIZE + 15];
		//char* _dst = new char[BUF_SIZE + 15];

		////set 16byte alignment
		//char* src = (char*)(((unsigned long)_src + 16) & (~0xF));
		//char* dst = (char*)(((unsigned long)_dst + 16) & (~0xF));

		////wite randam test data to src buffer
		//char* idx = src;
		//srand((unsigned)time(NULL));
		//for (int i = 0; i < BUF_SIZE; i++) {
		//	(*idx++) = rand() & 0xff;
		//}

		//{
		//	//
		//	ssememcpy(dst, src, BUF_SIZE);
		//	TS_ASSERT_EQUALS(memcmp(dst, src, BUF_SIZE), 0);

		//	//short buffer write
		//	ssememcpy(dst, src, 10);
		//	TS_ASSERT_EQUALS(memcmp(dst, src, 10), 0);
		//}

		//{
		//	//
		//	ssememcpy(dst + 1, src + 1, BUF_SIZE - 1);
		//	TS_ASSERT_EQUALS(memcmp(dst + 1, src + 1, BUF_SIZE - 1), 0);

		//	//short buffer write
		//	ssememcpy(dst + 1, src + 1, 10 - 1);
		//	TS_ASSERT_EQUALS(memcmp(dst + 1, src + 1, 10 - 1), 0);
		//}

		//{
		//	//
		//	ssememcpy(dst + 1, src, BUF_SIZE - 1);
		//	TS_ASSERT_EQUALS(memcmp(dst + 1, src, BUF_SIZE - 1), 0);

		//	//short buffer write
		//	ssememcpy(dst + 1, src, 10 - 1);
		//	TS_ASSERT_EQUALS(memcmp(dst + 1, src, 10 - 1), 0);
		//}

		//{
		//	//
		//	ssememcpy(dst, src + 1, BUF_SIZE - 1);
		//	TS_ASSERT_EQUALS(memcmp(dst, src + 1, BUF_SIZE - 1), 0);

		//	//short buffer write
		//	ssememcpy(dst, src + 1, 10 - 1);
		//	TS_ASSERT_EQUALS(memcmp(dst, src + 1, 10 - 1), 0);
		//}

		//delete _dst;
		//delete _src;
	}
};