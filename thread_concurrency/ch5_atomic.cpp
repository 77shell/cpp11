#include <atomic>
#include <iostream>
#include <future>
#include <chrono>

int
main(int argc, char *argv[])
{
	std::atomic<bool> b;
	bool x {b.load()};
	b.store(true);

	std::cout << "atmoic b: " << b.load() << std::endl;
	std::cout << "non-atmoic x: " << x << std::endl;
	std::cout << "exchange b & x\n";

	auto f = std::async(std::launch::async, [&]{
			x = b.exchange(false);
			std::cout << "atmoic b: " << b.load() << std::endl;
			std::cout << "non-atmoic x: " << x << std::endl;
		}
		);
	f.get();

	return 0;
}

void
atomic_test_and_set()
{
        std::atomic_flag atm_f;
        auto f = std::async(std::launch::async, [&] {
                        while(atm_f.test_and_set());
                        std::cout << __func__ << " enter atm_f test and set\n";
                        std::this_thread::sleep_for(std::chrono::seconds(2));
                        atm_f.clear();
                        std::cout << __func__ << " leave atm_f test and set\n";
                }
                );

        while(atm_f.test_and_set());
        std::cout << "Lock free type ? : " << ATOMIC_BOOL_LOCK_FREE << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(2));
        std::cout << "Clear flag\n";
        atm_f.clear();
        return 0;
}
