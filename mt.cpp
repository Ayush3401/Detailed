#include <iostream>
#include <thread>
#include <vector>
using namespace std;

// function to be used in callable
void func_dummy(int N)
{
    vector<int> a(1e6);
    for (int i = 0; i < N; i++)
    {
        a[i] = i;
        cout << "Thread 1 :: callable => function pointer\n";
    }
}

// A callable object
class thread_obj
{
public:
    void operator()(int n)
    {
        vector<int> a(1e6);
        for (int i = 0; i < n; i++)
        {
            a[i] = i;
            cout << "Thread 2 :: callable => function object\n";
        }
    }
};

int main()
{
    // Define a Lambda Expression
    auto f = [](int n)
    {
        for (int i = 0; i < n; i++)
            cout << "Thread 3 :: callable => lambda expression\n";
    };
    // launch thread using function pointer as callable
    thread th1(func_dummy, 2);

    // launch thread using function object as callable
    thread th2(thread_obj(), 2);

    // launch thread using lambda expression as callable
    thread th3(f, 2);

    // Wait for thread t1 to finish
    th1.join();
    // Wait for thread t2 to finish
    th2.join();

    // Wait for thread t3 to finish
    th3.join();

    return 0;
}