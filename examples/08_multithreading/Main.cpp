#include "MyWindow.hpp"
#include <mutex>
#include <thread>

int APIENTRY WinMain(
    _In_     HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_     PSTR      pCmdLine,
    _In_     INT       nCmdShow)
{
    /**
     * 该示例演示了如何在多个线程中创建窗口，并通过按钮点击事件在新线程中创建新窗口。
     * SimpleWindow支持多线程创建窗口，每个线程的窗口计数、Dpi缩放信息等相互独立，
     * 因此每个线程都可以有自己的消息循环，窗口之间不会互相影响，也不会阻塞彼此。
     * 需要注意的是，UI元素对象在创建时就与其所在的线程绑定，不能跨线程添加子元素。
     * 若需要跨线程更新UI，可以调用元素的Invoke/InvokeAsync方法在其所属线程执行代码。
     */


    std::mutex mut;
    std::vector<std::unique_ptr<std::thread>> pool;


    // 回调函数，在新线程创建一个窗口并添加到线程池
    // 窗口的RequestNewWindow事件绑定此回调
    sw::Action<> onRequestNewWindow = [&]()
    {
        // 加锁，防止多个线程同时修改线程池
        std::lock_guard<std::mutex> lock(mut);

        // 创建一个新线程，并显示一个新窗口
        pool.emplace_back(std::make_unique<std::thread>([&]() {
            MyWindow w;
            w.RequestNewWindow += onRequestNewWindow;
            w.ShowDialog();
        }));
    };
    

    // 创建第一个窗口
    onRequestNewWindow();
    
    
    while (!pool.empty()) {
        // 依次取出线程
        mut.lock();
        auto t = std::move(pool.back());
        pool.pop_back();
        mut.unlock();
        // 等待该线程结束
        if (t->joinable()) {
            t->join();
        }
    }
    return 0;
}
