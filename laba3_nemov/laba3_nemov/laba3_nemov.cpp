#include <iostream>
#include <thread>
#include <mutex>

int coins = 101; 
int Bob_coins = 0;
int Tom_coins = 0;
std::mutex mtx;

void coin_sharing(const char* name, int& my_coins, int& peer_coins) {
    while (true) {
        mtx.lock();
        if (coins > 0) {
            if (my_coins < peer_coins || (my_coins == peer_coins && coins > 1)) {
                // take if behind or if not last odd coin
                coins--;
                my_coins++;
                std::cout << name << " took 1 coin\n";
            }
            else if (my_coins == peer_coins && coins == 1) {
                // leave the last odd one for the ghost
                coins--;
                std::cout << "Ghost took the last coin\n";
            }
        }

        if (coins == 0) {
            mtx.unlock();
            break;
        }
        mtx.unlock();
    }
}

int main() {
    std::thread t1(coin_sharing, "Bob", std::ref(Bob_coins), std::ref(Tom_coins));
    std::thread t2(coin_sharing, "Tom", std::ref(Tom_coins), std::ref(Bob_coins));

    t1.join();
    t2.join();

    std::cout << "\nBob: " << Bob_coins << "\nTom: " << Tom_coins << "\nBag: " << coins << std::endl;
    return 0;
}