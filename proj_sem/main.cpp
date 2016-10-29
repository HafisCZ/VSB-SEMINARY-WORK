#include <iostream>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <stdlib.h>

using namespace std;

template <typename T> class DynamicHandler {
    private:
        T *__content;
        unsigned int __size;
    public:
        DynamicHandler(void) : __size(0) {}
        DynamicHandler(const unsigned int _size) {
            if (_size > 0) {
                __size = _size;
                __content = new T[__size];
            } else {
                __size = 0;
            }
        }
        bool expand(unsigned int _size) {
            if (_size > __size) {
                T *__copyContent = new T[_size];
                for (unsigned int i = 0; i < __size; i++) {
                    __copyContent[i] = __content[i];
                }
                dealloc();
                __content = __copyContent;
                __size = _size;
                __copyContent = NULL;
                return true;
            } else {
                return false;
            }
        }
        virtual void dealloc() {
            if (__size > 0) {
                delete[] __content;
            }
        }
        bool expandBy(unsigned int _size) {
            if (_size > 0) {
                return expand(_size + __size);
            } else {
                return false;
            }
        }
        bool shrink(unsigned int _size) {
            if (_size < __size) {
                T *__copyContent = new T[_size];
                for (unsigned int i = 0; i < _size; i++) {
                    __copyContent[i] = __content[i];
                }
                dealloc();
                __content = __copyContent;
                __size = _size;
                __copyContent = NULL;
                return true;
            } else {
                return false;
            }
        }
        void prefill(T _value) {
            for (int i = 0; i < __size; i++) {
                __content[i] = _value;
            }
        }
        void purge(void) {
            __size = 0;
            delete[] __content;
            __content = NULL;
        }
        T get(unsigned int _index) {
            if (_index >= 0 && _index < __size) {
                return __content[_index];
            } else {
                return __content[0];
            }
        }
        bool set (unsigned int _index, T _value) {
            if (_index >= 0 && _index < __size) {
                __content[_index] = _value;
                return true;
            } else {
                return false;
            }
        }
        unsigned int size(void) {
            return __size;
        }
        bool push_back(T _value) {
            expandBy(1);
            if (set(__size - 1, _value)) {
                return true;
            } else {
                return false;
            }
        }
        bool push_front(T _value) {
            T *__copyContent = new T[__size + 1];
            for (unsigned int i = 0; i < __size; i++) {
                __copyContent[i + 1] = __content[i];
            }
            dealloc();
            __content = __copyContent;
            __size++;
            __copyContent = NULL;
            if (set(0, _value)) {
                return true;
            } else {
                return false;
            }
        }
        bool extract(T _copy[], unsigned int _size) {
            if (_size >= 0 && _size <= __size) {
                for (unsigned int i = 0; i < _size; i++) {
                    _copy[i] = __content[i];
                }
                return true;
            } else {
                return false;
            }
        }
};

typedef struct {
    double distance, time;
} Training;

Training newTraining(double distance, double time) {
    Training f;
    f.distance = distance;
    f.time = time;
    return f;
}

typedef struct {
    std::string name;
    DynamicHandler<Training> trains;
} Cyclist;

Cyclist newCyclist(std::string name) {
    Cyclist c;
    c.name = name;
    return c;
}

double getTotalDistance(Cyclist c) {
    double total = 0;
    for (unsigned int i = 0; i < c.trains.size(); i++) {
        total += c.trains.get(i).distance;
    }
    return total;
}

double getTotalTime(Cyclist c) {
    double total = 0;
    for (unsigned int i = 0; i < c.trains.size(); i++) {
        total += c.trains.get(i).time;
    }
    return total;
}

double getAverageDistance(Cyclist c) {
    return getTotalDistance(c) / c.trains.size();
}

double getAverageTime(Cyclist c) {
    return getTotalTime(c) / c.trains.size();
}

bool readFile(DynamicHandler<Cyclist> &database, std::string source) {
    std::ifstream file(source.c_str());
    if (file.is_open()) {
        std::string temp, temp2 = "";
        DynamicHandler<std::string> s;
        while (std::getline(file, temp)) {
            bool database_flag = false;
            for (unsigned int i = 0; i < temp.length(); i++) {
                if (temp[i] == ';') {
                    s.push_back(temp2);
                    temp2 = "";
                } else {
                    temp2 += temp[i];
                }
            }
            for (unsigned int i = 0; i <= database.size() && !database_flag; i++) {
                if (i < database.size()) {
                    if (strcmp(database.get(i).name.c_str(), s.get(0).c_str()) == 0) {
                      database.get(i).trains.push_back(newTraining(strtod(s.get(1).c_str(), NULL), strtod(s.get(2).c_str(), NULL)));
                      break;
                    }
                } else {
                    database_flag = true;
                    Cyclist c = newCyclist(s.get(0));
                    c.trains.push_back(newTraining(strtod(s.get(1).c_str(), NULL), strtod(s.get(2).c_str(), NULL)));
                    database.push_back(c);
                }
            }
            s.purge();
        }
        file.close();
        return true;
    } else {
        return false;
    }
}

int main()
{
    DynamicHandler<Cyclist> cyclists;
    std::string s;

    std::cout << "Zadejte cestu k souboru: ";
    getline(cin, s);
    readFile(cyclists, s);

    for (unsigned int i = 0; i < cyclists.size(); i++) {
        cout << "There is!" << endl;
    }

    return 0;
}
