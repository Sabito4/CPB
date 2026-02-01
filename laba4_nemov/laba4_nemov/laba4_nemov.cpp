
#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

using namespace std;

int dish[3] = { 3000, 3000, 3000 };
int eaten[3] = { 0, 0, 0 };

mutex mootex;
bool cook_turn = true;

bool fired = false;
bool no_salary = false;
bool quit = false;

void cook(int efficiency) {
    auto start = chrono::steady_clock::now();

    while (true) {
        // check 5 days
        auto now = chrono::steady_clock::now();
        auto time_passed = chrono::duration_cast<chrono::milliseconds>(now - start);
        if (time_passed.count() >= 5000) {
            quit = true;
            cout << "cook quit after 5 days" << endl;
            return;
        }

        mootex.lock();

        // check if fired
        for (int i = 0; i < 3; i++) {
            if (dish[i] <= 0) {
                fired = true;
                cout << "cook fired! dish " << i << " empty" << endl;
                mootex.unlock();
                return;
            }
        }

        if (!cook_turn) {
            mootex.unlock();
            this_thread::yield();
            continue;
        }

        // add nuggets
        for (int i = 0; i < 3; i++) {
            dish[i] += efficiency;
        }
        cout << "cook added " << efficiency << " to each dish" << endl;

        cook_turn = false;
        mootex.unlock();

        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void fatty(int id, int gluttony) {
    while (true) {
        if (fired || quit) return;

        mootex.lock();

        if (eaten[id] >= 10000) {
            cout << "fatty " << id << " self-destructed" << endl;

            // check if all dead
            bool all_dead = true;
            for (int i = 0; i < 3; i++) {
                if (eaten[i] < 10000) {
                    all_dead = false;
                    break;
                }
            }
            if (all_dead) {
                no_salary = true;
                cout << "all dead, no salary" << endl;
            }

            mootex.unlock();
            return;
        }

        if (cook_turn || dish[id] <= 0) {
            mootex.unlock();
            this_thread::yield();
            continue;
        }

        // eat
        int eat_amount = gluttony;
        if (dish[id] < gluttony) {
            eat_amount = dish[id];
        }

        dish[id] -= eat_amount;
        eaten[id] += eat_amount;
        cout << "fatty " << id << " ate " << eat_amount << endl;

        //if its last fatty in this round
        bool all_ate = true;
        for (int i = 0; i < 3; i++) {
            if (dish[i] > 0 && eaten[i] < 10000) {
                all_ate = false;
                break;
            }
        }

        if (all_ate) {
            cook_turn = true;
        }

        mootex.unlock();

        this_thread::sleep_for(chrono::milliseconds(10));
    }
}

void run_test(int gluttony, int efficiency, string name) {
    cout << endl << "--- " << name << " ---" << endl;
    cout << "gluttony = " << gluttony << ", efficiency = " << efficiency << endl;

    // reset
    for (int i = 0; i < 3; i++) {
        dish[i] = 3000;
        eaten[i] = 0;
    }
    fired = false;
    no_salary = false;
    quit = false;
    cook_turn = true;

    thread cook_thread(cook, efficiency);
    thread fatty1(fatty, 0, gluttony);
    thread fatty2(fatty, 1, gluttony);
    thread fatty3(fatty, 2, gluttony);

    cook_thread.join();
    fatty1.join();
    fatty2.join();
    fatty3.join();

    if (fired) cout << "result: cook fired" << endl;
    else if (no_salary) cout << "result: no salary" << endl;
    else if (quit) cout << "result: cook quit" << endl;
    else cout << "result: unknown" << endl;
}

int main() {
    cout << "nugget simulation" << endl;
    cout << "each dish starts with 3000 nuggets" << endl;
    cout << "self-destruct at 10000 nuggets" << endl;
    cout << "5 days = 5 seconds" << endl;

    run_test(500, 1, "test 1: cook fired");
    run_test(1000, 10000, "test 2: no salary");
    run_test(5, 100, "test 3: cook quits");

    cout << endl << "simulation done" << endl;
    return 0;
}
