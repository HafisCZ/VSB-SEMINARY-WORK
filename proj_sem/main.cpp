/**
    @author Martin 'Hafis' @ HIRAISHIN SOFTWARE
    @version V01.B002.EX
*/

#include <iostream>
#include <cstring>
#include <fstream>
#include <stdio.h>
#include <cstdlib>

using namespace std;

/**
    DynamicHandler class for managing heap-stored objects

    @param T type specifier
*/
template <typename T> class DynamicHandler {
    protected:
        T *content;
        uint32_t range;

        /**
            Releases heap memory when size is greater than 0
        */
        virtual void dealloc() {
            if (this->range > 0) {
                delete[] this->content;
            }
        }
    public:
        /**
            Constructor (automatic)
        */
        DynamicHandler(void) {
            this->range = 0;
        }

        /**
            Constructor (manual)

            @param range of storage
        */
        DynamicHandler(uint32_t range) {
            if (range > 0) {
                this->range = range;
                this->content = new T[this->range];
            } else {
                this->range = 0;
            }
        }

        /**
            Expands storage

            @param range of new storage
            @return success value
        */
        bool expand(uint32_t range) {
            if (range > this->range) {
                T *replacement = new T[range];
                for (uint32_t i = 0; i < this->range; i++) {
                    replacement[i] = this->content[i];
                }
                dealloc();
                this->content = replacement;
                this->range = range;
                replacement = NULL;
                return true;
            } else {
                return false;
            }
        }

        /**
            Shrinks storage by

            @param range modifier of new storage
            @return success value
        */
        bool expandBy(uint32_t range) {
            if (range > 0) {
                return expand(range + this->range);
            } else {
                return false;
            }
        }

        /**
            Removes element from storage

            @param index of element
            @return success value
        */
        bool remove(uint32_t index) {
            if (index >= 0 && index < this->range) {
                T* replacement = new T[this->range - 1];
                for (uint32_t i = 0; i < this->range && i != index; i++) {
                    replacement[(i > index ? i : i - 1)] = this->content[i];
                }
                dealloc();
                this->content = replacement;
                this->range -= 1;
                replacement = NULL;
                return true;
            } else {
                return false;
            }
        }

        /**
            Shrinks storage

            @param range of new storage
            @return success value
        */
        bool shrink(uint32_t range) {
            if (range < this->range) {
                T* replacement = new T[range];
                for (uint32_t i = 0; i < range; i++) {
                    replacement[i] = this->content[i];
                }
                dealloc();
                this->content = replacement;
                this->range = range;
                replacement = NULL;
                return true;
            } else {
                return false;
            }
        }

        /**
            Shrinks storage by

            @param range modifier of new storage
            @return success value
        */
        bool shrinkBy(uint32_t range) {
            if (range > 0) {
                return shrink(this->range - range);
            } else {
                return false;
            }
        }

        /**
            Sets each value in content

            @param value being set
        */
        void prefill(T value) {
            for (uint32_t i = 0; i < this->range; i++) {
                this->content[i] = value;
            }
        }

        /**
            Clear heap memory
        */
        void purge(void) {
            dealloc();
            this->range = 0;
            this->content = NULL;
        }

        /**
            Retrieve object at index

            @param index where is stored
            @return object at index
        */
        T& get(uint32_t index) {
            if (index >= 0 && index < this->range) {
                return this->content[index];
            } else {
                T* null = NULL;
                return *null;
            }
        }

        /**
            Set value in storage on index

            @param index where to store
            @param value to be stored
            @return success value
        */
        bool set(uint32_t index, T value) {
            if (index >= 0 && index < this->range) {
                this->content[index] = value;
                return true;
            } else {
                return false;
            }
        }

        /**
            Get content size

            @return content size
        */
        uint32_t size(void) {
            return this->range;
        }

        /**
            Push value to back of content, expands storage by 1

            @param value to be stored
            @return success value
        */
        bool push_back(T value) {
            if (expandBy(1) && set(this->range - 1, value)) {
                return true;
            } else {
                return false;
            }
        }

        /**
            Push value to front of content, expands storage by 1

            @param value to be stored
            @return success value
        */
        bool push_front(T value) {
            T *replacement = new T[this->range + 1];
            for (uint32_t i = 0; i < this->range; i++) {
                replacement[i + 1] = this->content[i];
            }
            dealloc();
            this->content = replacement;
            this->range += 1;
            replacement = NULL;
            if (set(0, value)) {
                return true;
            } else {
                return false;
            }
        }

        /**
            Extracts specified range of data from content

            @param target[] where to copy
            @param begin starting index
            @param end ending index
            @return success value
        */
        bool extract(T target[], uint32_t begin, uint32_t end) {
            if (begin >= 0 && end <= this->range && begin <= end) {
                for (uint32_t i = begin; i < end; i++) {
                    target[i] = this->content[i];
                }
                return true;
            } else {
                return false;
            }
        }
};

// STRUCT DECLARATIONS

typedef struct {
    double distance, time;
} Training;

typedef struct {
    std::string name;
    DynamicHandler<Training> trains;
} Cyclist;

// DECLARATIONS

void sort(DynamicHandler<Cyclist>& cyclists);
bool readFile(DynamicHandler<Cyclist> &database, std::string source);
double getTotalDistance(Cyclist c);
double getTotalTime(Cyclist c);
double getAverageDistance(Cyclist c);
double getAverageTime(Cyclist c);
Training newTraining(double distance, double time);
Cyclist newCyclist(std::string name);


// MAIN

int main()
{
    char simplified;
    DynamicHandler<Cyclist> cyclists;
    std::string s;

    std::cout << "Zjednoduseny vypis? [Y/N] ";
    simplified = std::cin.get();
    std::cin.ignore();

    std::cout << "Zadejte cestu k souboru: ";
    std::getline(cin, s);
    if (!readFile(cyclists, s)) {
        char retry;
        do {
            std::cout << "Neplatna cesta, opakovat zadani? [Y/N] ";
            retry = std::cin.get();
            std::cin.ignore();
            if (retry == 'N') break;
            std::cout << "Zadejte novou cestu k souboru: ";
            std::getline(cin, s);
            if (readFile(cyclists, s)) break;
        } while (retry == 'Y');
    }

    cout << endl << "Seznam cyklistu (" << cyclists.size() << " cyklistu nacteno): " << endl << endl;
    sort(cyclists);
    for (unsigned int i = 0; i < cyclists.size(); i++) {
        Cyclist sub = cyclists.get(i);
        cout << sub.name << "\tPocet: " << sub.trains.size() << "\tCelkem: " << getTotalDistance(sub) << "km / " << getTotalTime(sub) << "h\tPrumer: " << getAverageDistance(sub) << "hm / " << getAverageTime(sub) << "h" << endl;
        for (unsigned int j = 0; j < sub.trains.size() && simplified != 'Y'; j++) {
            cout << " - Trenink " << j + 1 << "\tUjeto: " << sub.trains.get(j).distance << "km / " << sub.trains.get(j).time << "h" << endl;
        }
        if (i == cyclists.size() - 1) {
            for (unsigned int j = 0; j < cyclists.get(i).trains.size(); j++) {
                cyclists.get(i).trains.purge();
            }
            cyclists.purge();
        }
    }
    return 0;
}

// DEFINITIONS

/**
    Creates new instance of Training structure

    @param distance of training
    @param time of training
    @return Training
*/
Training newTraining(double distance, double time) {
    Training f;
    f.distance = distance;
    f.time = time;
    return f;
}

/**
    Creates new instance of Cyclist structure

    @param name of cyclist
    @return Cyclist
*/
Cyclist newCyclist(std::string name) {
    Cyclist c;
    c.name = name;
    return c;
}

/**
        Return average time of cyclist

        @param c cyclist measured
        @return average time
*/
double getAverageTime(Cyclist c) {
    return getTotalTime(c) / c.trains.size();
}

/**
        Return average distance of cyclist

        @param c cyclist measured
        @return average distance
*/
double getAverageDistance(Cyclist c) {
    return getTotalDistance(c) / c.trains.size();
}

/**
        Return total time of cyclist

        @param c cyclist measured
        @return total time
*/
double getTotalTime(Cyclist c) {
    double total = 0;
    for (unsigned int i = 0; i < c.trains.size(); i++) {
        total += c.trains.get(i).time;
    }
    return total;
}

/**
        Return total distance of cyclist

        @param c cyclist measured
        @return total distance
*/
double getTotalDistance(Cyclist c) {
    double total = 0;
    for (unsigned int i = 0; i < c.trains.size(); i++) {
        total += c.trains.get(i).distance;
    }
    return total;
}

/**
        Reads specified file and fills DynamicHandler of Cyclist

        @param database DynamicHandler target
        @param source path to file
        @return success value
*/
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

/**
    Sorts content of DynamicHandler of Cyclists by total time

    @param cyclists DynamicHandler of cyclists
*/
void sort(DynamicHandler<Cyclist>& cyclists) {
    Cyclist temporary;
    bool sortFlag = true;
    for (uint32_t i = 1; i <= cyclists.size() && sortFlag; i++) {
        sortFlag = false;
        for (uint32_t j = 0; j < cyclists.size() - 1; j++) {
            if (getTotalTime(cyclists.get(j + 1)) > getTotalTime(cyclists.get(j))) {
                temporary = cyclists.get(j);
                cyclists.set(j, cyclists.get(j + 1));
                cyclists.set(j + 1, temporary);
                sortFlag = true;
            }
        }
    }
}
