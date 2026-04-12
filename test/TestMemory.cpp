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

// ==================== unique_ptr ====================

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

TEST(SUITE_NAME, UniquePtrDefaultConstructor)
{
	bsc::unique_ptr<int> ptr;
	EXPECT_EQ(ptr.get(), nullptr);
}

TEST(SUITE_NAME, UniquePtrNullptrConstructor)
{
	bsc::unique_ptr<int> ptr = nullptr;
	EXPECT_EQ(ptr.get(), nullptr);
}

TEST(SUITE_NAME, UniquePtrGet)
{
	int* raw = new int(42);
	bsc::unique_ptr<int> ptr(raw);
	EXPECT_EQ(ptr.get(), raw);
	EXPECT_EQ(*ptr.get(), 42);
}

TEST(SUITE_NAME, UniquePtrDereference)
{
	bsc::unique_ptr<int> ptr(new int(99));
	EXPECT_EQ(*ptr, 99);
}

TEST(SUITE_NAME, UniquePtrMoveConstructor)
{
	int counter = 0;
	bsc::unique_ptr<delete_counter> ptr1(new delete_counter(counter));
	ptr1->increase_value();

	bsc::unique_ptr<delete_counter> ptr2(std::move(ptr1));
	EXPECT_EQ(ptr1.get(), nullptr);
	EXPECT_EQ(ptr2->get_value(), 1);
	EXPECT_EQ(counter, 0);
}

TEST(SUITE_NAME, UniquePtrMoveAssignment)
{
	int counter = 0;
	bsc::unique_ptr<delete_counter> ptr1(new delete_counter(counter));
	bsc::unique_ptr<delete_counter> ptr2;

	ptr2 = std::move(ptr1);
	EXPECT_EQ(ptr1.get(), nullptr);
	EXPECT_NE(ptr2.get(), nullptr);
	EXPECT_EQ(counter, 0);
}

TEST(SUITE_NAME, UniquePtrMoveAssignmentDeletesOld)
{
	int counter1 = 0;
	int counter2 = 0;
	bsc::unique_ptr<delete_counter> ptr1(new delete_counter(counter1));
	bsc::unique_ptr<delete_counter> ptr2(new delete_counter(counter2));

	ptr2 = std::move(ptr1);
	EXPECT_EQ(counter2, 1); // old resource was deleted
	EXPECT_EQ(counter1, 0); // moved resource still alive
}

TEST(SUITE_NAME, UniquePtrReset)
{
	int counter = 0;
	bsc::unique_ptr<delete_counter> ptr(new delete_counter(counter));
	EXPECT_EQ(counter, 0);

	ptr.reset();
	EXPECT_EQ(counter, 1);
	EXPECT_EQ(ptr.get(), nullptr);
}

TEST(SUITE_NAME, UniquePtrResetTwice)
{
	int counter = 0;
	bsc::unique_ptr<delete_counter> ptr(new delete_counter(counter));
	ptr.reset();
	ptr.reset(); // should be safe
	EXPECT_EQ(counter, 1);
}

// ==================== shared_ptr ====================

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

TEST(SUITE_NAME, SharedPtrDefaultConstructor)
{
	bsc::shared_ptr<int> ptr;
	EXPECT_EQ(ptr.get(), nullptr);
	EXPECT_EQ(ptr.get_count(), 0);
}

TEST(SUITE_NAME, SharedPtrNullptrConstructor)
{
	bsc::shared_ptr<int> ptr = nullptr;
	EXPECT_EQ(ptr.get(), nullptr);
	EXPECT_EQ(ptr.get_count(), 0);
}

TEST(SUITE_NAME, SharedPtrGet)
{
	int* raw = new int(42);
	bsc::shared_ptr<int> ptr(raw);
	EXPECT_EQ(ptr.get(), raw);
	EXPECT_EQ(*ptr, 42);
}

TEST(SUITE_NAME, SharedPtrRefCount)
{
	bsc::shared_ptr<int> s1(new int(10));
	EXPECT_EQ(s1.get_count(), 1);

	{
		bsc::shared_ptr<int> s2 = s1;
		EXPECT_EQ(s1.get_count(), 2);
		EXPECT_EQ(s2.get_count(), 2);

		{
			bsc::shared_ptr<int> s3 = s2;
			EXPECT_EQ(s1.get_count(), 3);
		}
		EXPECT_EQ(s1.get_count(), 2);
	}
	EXPECT_EQ(s1.get_count(), 1);
}

TEST(SUITE_NAME, SharedPtrCopyAssignment)
{
	int counter1 = 0;
	int counter2 = 0;

	bsc::shared_ptr<delete_counter> s1(new delete_counter(counter1));
	bsc::shared_ptr<delete_counter> s2(new delete_counter(counter2));

	s2 = s1;
	EXPECT_EQ(counter2, 1); // old resource deleted
	EXPECT_EQ(counter1, 0); // shared resource still alive
	EXPECT_EQ(s1.get_count(), 2);
	EXPECT_EQ(s2.get_count(), 2);
}

TEST(SUITE_NAME, SharedPtrSelfAssignment)
{
	int counter = 0;
	bsc::shared_ptr<delete_counter> s1(new delete_counter(counter));
	s1 = s1;
	EXPECT_EQ(counter, 0);
	EXPECT_EQ(s1.get_count(), 1);
}

TEST(SUITE_NAME, SharedPtrMoveConstructor)
{
	int counter = 0;
	bsc::shared_ptr<delete_counter> s1(new delete_counter(counter));
	bsc::shared_ptr<delete_counter> s2(std::move(s1));

	EXPECT_EQ(s1.get(), nullptr);
	EXPECT_NE(s2.get(), nullptr);
	EXPECT_EQ(s2.get_count(), 1);
	EXPECT_EQ(counter, 0);
}

TEST(SUITE_NAME, SharedPtrMoveAssignment)
{
	int counter1 = 0;
	int counter2 = 0;

	bsc::shared_ptr<delete_counter> s1(new delete_counter(counter1));
	bsc::shared_ptr<delete_counter> s2(new delete_counter(counter2));

	s2 = std::move(s1);
	EXPECT_EQ(counter2, 1); // old resource deleted
	EXPECT_EQ(counter1, 0);
	EXPECT_EQ(s1.get(), nullptr);
	EXPECT_EQ(s2.get_count(), 1);
}

TEST(SUITE_NAME, SharedPtrMoveAssignmentFromEmpty)
{
	bsc::shared_ptr<int> s1;
	bsc::shared_ptr<int> s2(new int(5));

	s2 = std::move(s1);
	EXPECT_EQ(s2.get(), nullptr);
}

TEST(SUITE_NAME, SharedPtrReset)
{
	int counter = 0;
	bsc::shared_ptr<delete_counter> s1(new delete_counter(counter));
	bsc::shared_ptr<delete_counter> s2 = s1;
	EXPECT_EQ(s1.get_count(), 2);

	s1.reset();
	EXPECT_EQ(s1.get(), nullptr);
	EXPECT_EQ(s1.get_count(), 0);
	EXPECT_EQ(s2.get_count(), 1);
	EXPECT_EQ(counter, 0); // still alive via s2
}

TEST(SUITE_NAME, SharedPtrResetLastOwner)
{
	int counter = 0;
	bsc::shared_ptr<delete_counter> s1(new delete_counter(counter));
	s1.reset();
	EXPECT_EQ(counter, 1);
}

TEST(SUITE_NAME, SharedPtrDereference)
{
	bsc::shared_ptr<int> ptr(new int(77));
	EXPECT_EQ(*ptr, 77);
	*ptr = 88;
	EXPECT_EQ(*ptr, 88);
}

// ==================== weak_ptr ====================

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

TEST(SUITE_NAME, WeakPtrDefaultConstructor)
{
	bsc::weak_ptr<int> wptr;
	EXPECT_TRUE(wptr.expired());
	EXPECT_EQ(wptr.get_count(), 0);
}

TEST(SUITE_NAME, WeakPtrNullptrConstructor)
{
	bsc::weak_ptr<int> wptr = nullptr;
	EXPECT_TRUE(wptr.expired());
}

TEST(SUITE_NAME, WeakPtrDoesNotAffectRefCount)
{
	bsc::shared_ptr<int> sptr(new int(42));
	EXPECT_EQ(sptr.get_count(), 1);

	bsc::weak_ptr<int> wptr = sptr;
	EXPECT_EQ(sptr.get_count(), 1); // weak_ptr should NOT increment resource ref count
	EXPECT_EQ(wptr.get_count(), 1);
}

TEST(SUITE_NAME, WeakPtrExpiredAfterSharedDestroyed)
{
	bsc::weak_ptr<int> wptr;
	{
		bsc::shared_ptr<int> sptr(new int(10));
		wptr = sptr;
		EXPECT_FALSE(wptr.expired());
	}
	EXPECT_TRUE(wptr.expired());
}

TEST(SUITE_NAME, WeakPtrCopyConstructor)
{
	bsc::shared_ptr<int> sptr(new int(5));
	bsc::weak_ptr<int> w1 = sptr;
	bsc::weak_ptr<int> w2(w1);

	EXPECT_FALSE(w1.expired());
	EXPECT_FALSE(w2.expired());
	EXPECT_EQ(*w1, *w2);
}

TEST(SUITE_NAME, WeakPtrCopyAssignment)
{
	bsc::shared_ptr<int> sptr(new int(5));
	bsc::weak_ptr<int> w1 = sptr;
	bsc::weak_ptr<int> w2;

	w2 = w1;
	EXPECT_FALSE(w2.expired());
	EXPECT_EQ(*w2, 5);
}

TEST(SUITE_NAME, WeakPtrReset)
{
	bsc::shared_ptr<int> sptr(new int(10));
	bsc::weak_ptr<int> wptr = sptr;
	EXPECT_FALSE(wptr.expired());

	wptr.reset();
	EXPECT_TRUE(wptr.expired());
	EXPECT_EQ(wptr.get(), nullptr);
}

TEST(SUITE_NAME, WeakPtrLockValid)
{
	bsc::shared_ptr<int> sptr(new int(42));
	bsc::weak_ptr<int> wptr = sptr;

	bsc::shared_ptr<int> locked = wptr.lock();
	EXPECT_NE(locked.get(), nullptr);
	EXPECT_EQ(*locked, 42);
	EXPECT_EQ(sptr.get_count(), 2);
}

TEST(SUITE_NAME, WeakPtrLockExpired)
{
	bsc::weak_ptr<int> wptr;
	{
		bsc::shared_ptr<int> sptr(new int(42));
		wptr = sptr;
	}

	bsc::shared_ptr<int> locked = wptr.lock();
	EXPECT_EQ(locked.get(), nullptr);
	EXPECT_EQ(locked.get_count(), 0);
}

TEST(SUITE_NAME, WeakPtrLockKeepsResourceAlive)
{
	int counter = 0;
	bsc::weak_ptr<delete_counter> wptr;
	bsc::shared_ptr<delete_counter> locked;
	{
		bsc::shared_ptr<delete_counter> sptr(new delete_counter(counter));
		wptr = sptr;
		locked = wptr.lock();
	}
	// sptr is gone but locked still holds a reference
	EXPECT_EQ(counter, 0);
	EXPECT_EQ(locked.get_count(), 1);

	locked.reset();
	EXPECT_EQ(counter, 1);
}

TEST(SUITE_NAME, WeakPtrSwap)
{
	bsc::shared_ptr<int> s1(new int(10));
	bsc::shared_ptr<int> s2(new int(20));
	bsc::weak_ptr<int> w1 = s1;
	bsc::weak_ptr<int> w2 = s2;

	w1.swap(w2);
	EXPECT_EQ(*w1, 20);
	EXPECT_EQ(*w2, 10);
}

TEST(SUITE_NAME, WeakPtrMultipleWeaksSameShared)
{
	int counter = 0;
	bsc::weak_ptr<delete_counter> w1;
	bsc::weak_ptr<delete_counter> w2;
	bsc::weak_ptr<delete_counter> w3;
	{
		bsc::shared_ptr<delete_counter> sptr(new delete_counter(counter));
		w1 = sptr;
		w2 = sptr;
		w3 = sptr;
		EXPECT_EQ(sptr.get_count(), 1);
	}
	EXPECT_EQ(counter, 1);
	EXPECT_TRUE(w1.expired());
	EXPECT_TRUE(w2.expired());
	EXPECT_TRUE(w3.expired());
}