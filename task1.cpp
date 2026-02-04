#include <iostream> 
#include <thread> 
#include <chrono> 
#include <vector> 
#include <string> 
#include <cmath> 
#include <iomanip> 
#include <mutex> 
#include <Windows.h> 

using namespace std;

const double SPEED_PX_S = 45.0;
const int    DELAY_MS = 100;
const double CORNER_X = 0.0;
const double CORNER_Y = 0.0;

mutex printMutex;

struct Point {
    double x, y;
};

class Bee {
public:
    string name;
    Point  birthPlace;
    Point  pos;
    bool   goingToCorner;

    Bee(string n, double startX, double startY)
        : name(n), birthPlace{ startX, startY }, pos{ startX, startY }, goingToCorner(true) {
    }

    void moveTo(Point start, Point target) {
        double dx = target.x - start.x;
        double dy = target.y - start.y;
        double distance = hypot(dx, dy);

        if (distance < 1e-6) return;

        double dirX = dx / distance;
        double dirY = dy / distance;
        double totalTimeSec = distance / SPEED_PX_S;
        
        double currentTime = 0.0;
        double deltaT = DELAY_MS / 1000.0;

        while (currentTime < totalTimeSec) {
            pos.x = start.x + dirX * SPEED_PX_S * currentTime;
            pos.y = start.y + dirY * SPEED_PX_S * currentTime;

            {
                lock_guard<mutex> lock(printMutex);
                cout << fixed << setprecision(1)
                    << name << " на (" << pos.x << ", " << pos.y << ") прямує до "
                    << (goingToCorner ? "кута" : "дому") << endl;

            }

            this_thread::sleep_for(chrono::milliseconds(DELAY_MS));
            currentTime += deltaT;
        }
        pos = target;
    }

    void fly() {
        while (true) {
            if (goingToCorner) {
                moveTo(pos, { CORNER_X, CORNER_Y });
            }
            else {
                moveTo(pos, birthPlace);
            }
            goingToCorner = !goingToCorner;
            {
                lock_guard<mutex> lock(printMutex);
                cout << ">>> " << name << " досягла цілі! Повертається..." << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(500));

        }

    }

};

int main() {
    SetConsoleOutputCP(1251); SetConsoleCP(1251);
    vector<Bee> bees = {
        {"Bee-1", 80.0, 25.0},
        {"Bee-2", 30.0, 15.0}
    };

    vector<thread> threads;

    for (auto& bee : bees) {
        threads.emplace_back(&Bee::fly, &bee);
    }

    for (auto& t : threads) {
        t.join();
    }
    return 0;

}
