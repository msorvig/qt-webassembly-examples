#include <iostream>
#include <coroutine>
#include <future>
#include <thread>

using namespace std;

struct task
{
	struct promise_type
	{
		task get_return_object() { return {}; }
		std::suspend_never initial_suspend() { return {}; }
		std::suspend_never final_suspend() noexcept { return {}; } 
		void return_void() {}
		void unhandled_exception() {}
	};
};

struct awaitable_t
{
    bool await_ready() const { 
        std::cout << "await_ready" << endl;
        return false; 
    }
    bool await_suspend(std::coroutine_handle<> h) {
       std::cout << "await_suspend" << endl;
       return true;
    }
    void await_resume() {
        std::cout << "await_resume" << endl;
    };
};

task helper()
{
    cout << "helper 1" << endl;
    awaitable_t avaitable;
    cout << "helper 2" << endl;
    co_await avaitable;
    cout << "helper 3" << endl;
    co_return;
}

void outer_helper()
{
    cout << "before helper" << endl;

    helper();

    cout << "after helper" << endl;
}

int main(int argc, char **argv)
{
    // 'co_await' cannot be used in the 'main' function
    outer_helper();
}