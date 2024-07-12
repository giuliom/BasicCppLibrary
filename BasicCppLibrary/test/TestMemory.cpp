#include <gtest/gtest.h>

#include <BasicMemory.h>

#define SUITE_NAME TestMemory

class delete_counter
{
	int m_value;
	int& m_counter;
public:
	delete_counter(int& counter) : m_value(0), m_counter(counter) {}
	~delete_counter() { m_counter++; }

	const int& get_value() { return m_value; }
	void increase_value() { ++m_value; }
};

TEST(SUITE_NAME, UniquePtr)
{
	int counter = 0;
	{
		delete_counter* dc = new delete_counter(counter);
		bsc::unique_ptr<delete_counter> ptr = dc;
		EXPECT_EQ(ptr->get_value(), 0);
		EXPECT_EQ(dc->get_value(), ptr->get_value());
	}

	EXPECT_EQ(counter, 1);
}

TEST(SUITE_NAME, SharedPtr)
{
	int counter = 0;
	delete_counter* dc = new delete_counter(counter);
	{
		bsc::shared_ptr<delete_counter> s1 = dc;
		EXPECT_EQ(s1->get_value(), 0);
		EXPECT_EQ(dc->get_value(), s1->get_value());
		
		{
			bsc::shared_ptr<delete_counter> s2 = s1;
			s2->increase_value();
			EXPECT_EQ(s1->get_value(), s2->get_value());
		}
		
		EXPECT_EQ(counter, 0);
	}

	EXPECT_EQ(counter, 1);
}

TEST(SUITE_NAME, WeakPtr)
{
	int counter = 0;
	delete_counter* dc = new delete_counter(counter);
	bsc::weak_ptr<delete_counter> wptr1 = nullptr;
	{
		bsc::shared_ptr<delete_counter> sptr = dc;
		wptr1 = sptr;
		{
			bsc::weak_ptr<delete_counter> wptr2 = sptr;
			wptr2->increase_value();
			EXPECT_EQ(wptr2->get_value(), sptr->get_value());
		}

		EXPECT_EQ(counter, 0);
	}
	EXPECT_EQ(counter, 1);

	EXPECT_TRUE(wptr1.expired());
}