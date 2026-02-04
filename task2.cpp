#include <iostream> 
#include <thread> 
#include <mutex> 
#include <vector> 
#include <random> 
#include <chrono> 
#include <cmath> 
#include <Windows.h> 

const double PI = 3.14159265358979323846;
using namespace std;

const int WIDTH = 100;
const int HEIGHT = 30;
const int SPEED = 1;
const int DELAY_MS = 100;
const int DIRECTION_CHANGE_INTERVAL = 3;

mutex printMutex;

class RandomGenerator {
private:
    mt19937 generator;
    uniform_real_distribution<double> distribution;
public:
    RandomGenerator() : distribution(0.0, 1.0) {
        random_device rd;
        generator.seed(rd());
    }

    double getRandomAngle() {
        return distribution(generator) * 2 * PI;
    }

};

class Drone {
public:
    string name;
    double x, y;
    double directionX;
    double directionY;
    int speed;
    RandomGenerator randomGen;
    int changeIntervalMs;

    Drone(string name, double x, double y, int speed, int changeIntervalSec)
        : name(name), x(x), y(y), speed(speed),
        changeIntervalMs(changeIntervalSec * 1000) {
        changeDirection();
    }

    void changeDirection() {
        double angle = randomGen.getRandomAngle();
        directionX = cos(angle);
        directionY = sin(angle);

        lock_guard<mutex> lock(printMutex);
        cout << "[ СИСТЕМА ] " << name << " змінив кут на: "
            << (angle * 180 / PI) << "°" << endl;
    }

    void move() {
        auto lastChangeTime = chrono::steady_clock::now();

        while (true) {
            auto currentTime = chrono::steady_clock::now();
            auto elapsedMs = chrono::duration_cast<chrono::milliseconds>(
                currentTime - lastChangeTime).count();

            if (elapsedMs >= changeIntervalMs) {
                changeDirection();
                lastChangeTime = currentTime;

            }

            x += directionX * speed;
            y += directionY * speed;


            if (x >= WIDTH || x <= 0) {
                directionX *= -1;
                x = (x >= WIDTH) ? WIDTH - 1 : 1;

            }

            if (y >= HEIGHT || y <= 0) {
                directionY *= -1;
                y = (y >= HEIGHT) ? HEIGHT - 1 : 1;

            }

            {
                lock_guard<mutex> lock(printMutex);
                cout << name << ": X=" << (int)x << " Y=" << (int)y << endl;
            }
            this_thread::sleep_for(chrono::milliseconds(DELAY_MS));

        }
    }
};

int main() {

    SetConsoleOutputCP(1251); SetConsoleCP(1251);
    vector<Drone> drones;
    drones.emplace_back("Drone_1", 10, 5, SPEED, DIRECTION_CHANGE_INTERVAL);
    drones.emplace_back("Drone_2", 50, 15, SPEED, DIRECTION_CHANGE_INTERVAL);
    drones.emplace_back("Drone_3", 80, 20, SPEED, DIRECTION_CHANGE_INTERVAL);

    vector<thread> threads;

    for (auto& drone : drones) {
        threads.emplace_back(&Drone::move, &drone);
    }

    for (auto& t : threads) {
        if (t.joinable()) t.join();
    }
    return 0;

}
