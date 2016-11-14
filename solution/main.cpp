#include <iostream>
#include <fstream>
#include <string>

#include <cstdio>
#include <cstring>
#include <cstdlib>

/**
    Class for managing heap-stored objects.

    @param T Object type.
*/
template <typename T> class HeapArrayInterface {
    private:
        /**
            T type dynamic array (internal storage of HeapArrayInterface)
        */
        T *content_container;
        /**
            Amount of T type objects in T dynamic array
        */
        int container_size;
        /**
            Internal pseudo-iterator
        */
        int internal_iterator;

        void deallocate();
        bool in_bounds(int index);
    public:
        enum ITER_OPERATION {DECREASE = -1, IGNORE = 0, INCREASE = 1};

        HeapArrayInterface(void);
        HeapArrayInterface(int new_size);
        void purge(void);
        void iter_reset(void);
        bool iter_set(int iterator_postion);
        bool set(int index, const T& value);
        bool resize(int new_size, bool keep_data);
        int iter_at(void);
        int size(void);
        T& iter_current(ITER_OPERATION iterator_operation);
        T& iter_current(void);

        /**
            Operator [] overload, returns object at index.

            @param index Index of object.
            @return Object at index.
        */
        T& operator[](int index) {
            if (in_bounds(index)) {
                return this->content_container[index];
            } else if (this->size() == 0) {
                resize(1, false);
            }

            return this->content_container[0];
        }
};

/**
    Structure with training's time and distance.

    @param Training.distance Distance of training.
    @param Training.duration Duration of training.
*/
struct Training {
    double distance, duration;

    Training() : distance(0), duration(0) {}
    Training(double distance, double duration);
};

/**
    Structure with cyclist's name and training sessions.

    @param Cyclist.name Name of cyclist.
    @param Cyclist.trains HeapArrayInterface with training sessions.
*/
struct Cyclist {
    private:
        int lastDistanceCheck, lastDurationCheck;
        double totalDistance, totalDuration;

        /**
            Set all internal counters/values.
        */
        void preset(void) {
            this->totalDistance = 0;
            this->totalDuration = 0;
            this->lastDistanceCheck = -1;
            this->lastDurationCheck = -1;
        }
    public:
        std::string name;
        HeapArrayInterface<Training> trains;

        Cyclist(void);
        Cyclist(const std::string& name);
        double getTotalDistance(void);
        double getTotalDuration(void);
        double getAverageDistance(void);
        double getAverageDuration(void);
};

void sortSwap(HeapArrayInterface<Cyclist>& cyclists, int indexA, int indexB);
void sortQuick(HeapArrayInterface<Cyclist>& cyclists, int zeroIndex, int aboveIndex);
bool readFile(HeapArrayInterface<Cyclist> &database, const std::string& source);
bool outputHtml(HeapArrayInterface<Cyclist> &cyclists, const std::string& target);
unsigned int sumOfCyclists(const std::string& source);
unsigned int sumOfTrainings(const Cyclist& c, const std::string& source);

int main()
{
    HeapArrayInterface<Cyclist> cyclists;
    std::string s;

    std::cout << "[IMPORT] Zadejte cestu: ";
    std::getline(std::cin, s);
    if (!readFile(cyclists, s)) {
        std::cout << "Soubor nenalezen!" << std::endl;
        return 2;
    } else {
        sortQuick(cyclists, 0, cyclists.size());
    }

    std::cout << "[EXPORT] Zadejte cestu: ";
    std::getline(std::cin, s);
    if (s.length() > 1 && !outputHtml(cyclists, s)) {
        std::cout << "Export nebyl proveden!" << std::endl;
    }

    std::cout << std::endl << "Seznam cyklistu (" << cyclists.size() << " cyklistu nacteno): " << std::endl << std::endl;
    for (int i = 0; i < cyclists.size(); i++) {
        std::cout << std::endl << static_cast<char>(218) << "(" << cyclists[i].trains.size() << ") " << cyclists[i].name << " [TOT]: " << cyclists[i].getTotalDistance() << "km / " << cyclists[i].getTotalDuration() << "h [AVG]: " << cyclists[i].getAverageDistance() << "hm / " << cyclists[i].getAverageDuration() << "h" << std::endl;
        for (int j = 0, j_max = cyclists[i].trains.size(); j < j_max; j++) {
            std::cout << static_cast<char>(j == j_max - 1 ? 192 : 195) << "[" << ((j_max - 1 >= 10 && j + 1 < 10) || (j_max - 1 >= 100 && j + 1 < 100) ? "0" : "") << j + 1 << "] Ujeto: " << cyclists[i].trains[j].distance << "km / " << cyclists[i].trains[j].duration << "h" << std::endl;
        }
    }

    for (int i = 0; i <= cyclists.size(); i++) {
        if (i < cyclists.size()) {
            cyclists[i].trains.purge();
        } else {
            cyclists.purge();
        }
    }

    std::cin.get();
    std::cin.ignore();

    return 0;
}

/**
        Opens specified file and fills HeapArrayInterface<Cyclist> with its content.

        @param database HeapArrayInterface<Cyclist> where to write to.
        @param source Path to file.
        @return Success value.
*/
bool readFile(HeapArrayInterface<Cyclist>& database, const std::string& source) {
    std::fstream file(source.c_str(), std::ios::in);
    if (file.is_open()) {
        HeapArrayInterface<std::string> temporary(3);
        database.resize(sumOfCyclists(source), false);
        std::string temp;
        double train_distance, train_duration;
        size_t position;
        bool exists;
        while (std::getline(file, temp)) {
            temporary.iter_reset();
            exists = false;
            while ((position = temp.find(';')) != std::string::npos) {
                temporary.iter_current(temporary.INCREASE) = temp.substr(0, position);
                temp.erase(0, position + 1);
            }
            temporary[2] = temp;
            train_distance = strtod(temporary[1].c_str(), NULL);
            train_duration = strtod(temporary[2].c_str(), NULL);
            if (train_distance <= 0 || train_duration <= 0 || temporary[2].size() < 1) {
                std::cout << "[ERR] Zaznam neplatny!" << std::endl;
                continue;
            }
            for (int i = 0; i < database.size() && !exists; i++) {
                if (database[i].name == temporary[0]) {
                    database[i].trains.iter_current(HeapArrayInterface<Training>::INCREASE) = Training(train_distance, train_duration);
                    exists = true;
                }
            }
            if (!exists) {
                database.iter_current() = Cyclist(temporary[0]);
                database.iter_current().trains.resize(sumOfTrainings(database[database.iter_at()], source), false);
                database.iter_current(database.INCREASE).trains.iter_current(HeapArrayInterface<Training>::INCREASE) = Training(train_distance, train_duration);
            }

        }
        temporary.purge();
        file.close();

        for (int i = 0; i < database.size(); i++) {
            if (database[i].trains.size() == 0) {
                database.resize(i, true);
                break;
            } else {
                for (int j = 0; j < database[i].trains.size(); j++) {
                    if (database[i].trains[j].distance <= 0 || database[i].trains[j].duration <= 0) {
                        database[i].trains.resize(j, true);
                        break;
                    }
                }
            }
        }
        return true;
    } else {
        return false;
    }
}

/**
    Opens specific file and writes HeapArrayInterface<Cyclist> content into structured HTML.

    @param cyclists HeapArrayInterface<Cyclist> what to write from.
    @param target Path to file.
    @return Success value.
*/
bool outputHtml(HeapArrayInterface<Cyclist> &cyclists, const std::string& target) {
    std::fstream file (target.c_str(), std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        file << "<html>" << std::endl << "<head>" << std::endl << "<meta http-equiv=\"Content-Type\" content=\"test/html;charset=utf-8\">" << std::endl;
        file << "<title>Seznam cyklistu</title>" << std::endl << "</head>" << std::endl << "<body>" << std::endl;
        file << "<table style=\"width:100%; border:1px solid black; border-collapse:collapse\" rules=rows>" << std::endl;
        file << "<caption>Statistika cyklistu</caption>" << std::endl;
        file << "<tr><th>Zavodnik</th><th>Ujeta vzdalenost [km]</th><th>Celkovy cas na kole [h]</th><th>Prumerna ujeta vzdalenost [km]</th><th>Prumerny cas na kole [h]</th></tr>" << std::endl;
        for (int i = 0; i < cyclists.size(); i++) {
            file << "<tr><td>" << cyclists[i].name << "</td><td>" << cyclists[i].getTotalDistance() << "</td><td>" << cyclists[i].getTotalDuration() << "</td><td>" << cyclists[i].getAverageDistance() << "</td><td>" << cyclists[i].getAverageDuration() << "</td></tr>" << std::endl;
        }
        file << "</table>" << std::endl << "<br>" << std::endl;
        file << "<table style=\"width:100%; border:1px solid black; border-collapse:collapse\" rules=rows>" << std::endl;
        file << "<caption>Treninky cyklistu</caption>" << std::endl;
        file << "<tr><th>Zavodnik</th><th>Ujeta vzdalenost [km]</th><th>Cas treninku [h]</th></tr>" << std::endl;
        for (int i = 0; i < cyclists.size(); i++) {
            file << "<tr><td rowspan=" << cyclists[i].trains.size() + 1 << ">" << cyclists[i].name << "</td><td>Prumer: " << cyclists[i].getAverageDistance() << "</td><td>Prumer: " << cyclists[i].getAverageDuration() << "</td></tr>" << std::endl;
            for (int j = 0; j < cyclists[i].trains.size(); j++) {
                file << "<tr><td>" << cyclists[i].trains[j].distance << "</td><td>" << cyclists[i].trains[j].duration << "</td></tr>" << std::endl;
            }
        }
        file << "</table>" << std::endl;
        file << "</body>" << std::endl << "</html>" << std::endl;
        file.close();
        std::cout << "HTML soubor vygenerovan!" << std::endl;
        return true;
    } else {
        return false;
    }
}

/**
    Quick sort algorithm, sort by total duration on trainings.

    @param cyclists HeapArrayInterface<Cyclist> what to sort.
    @param zeroIndex Lowest index in HeapArrayInterface<Cyclist>, default 0.
    @param aboveIndex Maximal + 1 index in HeapArrayInterface<Cyclist>, can be HeapArrayInterface<Cyclist>::size() return value.
*/
void sortQuick(HeapArrayInterface<Cyclist>& cyclists, int zeroIndex, int aboveIndex) {
    if (zeroIndex < aboveIndex) {
        int betweenIndex = zeroIndex;
        for (int i = zeroIndex + 1; i < aboveIndex; i++) {
            if (cyclists[i].getTotalDuration() > cyclists[zeroIndex].getTotalDuration()) {
                sortSwap(cyclists, i, ++betweenIndex);
            }
        }
        sortSwap(cyclists, zeroIndex, betweenIndex);
        sortQuick(cyclists, zeroIndex, betweenIndex);
        sortQuick(cyclists, betweenIndex + 1, aboveIndex);
    }
}

/**
    Swap two objects in HeapArrayInterface<Cyclist>.

    @param cyclists HeapArrayInterface<Cyclist> container.
    @param indexA Index A.
    @param indexB Index B.
*/
void sortSwap(HeapArrayInterface<Cyclist>& cyclists, int indexA, int indexB) {
    Cyclist temporary = cyclists[indexB];
    cyclists[indexB] = cyclists[indexA];
    cyclists[indexA] = temporary;
}

/**
    Training constructor.

    @param distance Distance of training.
    @param duration Duration of training.
*/
Training::Training(double distance, double duration) {
    this->distance = distance;
    this->duration = duration;
}

/**
    Default Cyclist constructor.
*/
Cyclist::Cyclist(void) {
    preset();
}

/**
    Cyclist constructor.

    @param name Name of new cyclist.
*/
Cyclist::Cyclist(const std::string& name){
    preset();
    this->name = name;
}

/**
    Total training distance.

    @return Total distance of Cyclist.
*/
double Cyclist::getTotalDistance(void) {
    if (this->lastDistanceCheck != (int) trains.size()) {
        this->totalDistance = 0;
        for (int i = 0; i < trains.size(); i++) {
            this->totalDistance += trains[i].distance;
        }
        this->lastDistanceCheck = trains.size();
    }
    return this->totalDistance;
}

/**
    Total training duration.

    @return Total duration of Cyclist.
*/
double Cyclist::getTotalDuration(void) {
    if (this->lastDurationCheck != (int) trains.size()) {
        this->totalDuration = 0;
        for (int i = 0; i < trains.size(); i++) {
            this->totalDuration += trains[i].duration;
        }
        this->lastDurationCheck = trains.size();
    }
    return this->totalDuration;
}

/**
    Average training distance.

    @return Average distance of Cyclist.
*/
double Cyclist::getAverageDistance(void) {
    if (trains.size() > 0) {
        return (getTotalDistance() / this->lastDistanceCheck);
    } else {
        return 0;
    }
}

/**
    Average training duration.

    @return Average duration of Cyclist.
*/
double Cyclist::getAverageDuration(void) {
    if (trains.size() > 0) {
        return (getTotalDuration() / this->lastDurationCheck);
    } else {
        return 0;
    }
}

/**
    Get amount of Cyclist in certain file.

    @param source Path to file.
    @return Amount of Cyclist.
*/
unsigned int sumOfCyclists(const std::string& source) {
    std::fstream file(source.c_str(), std::ios::in);
    unsigned int count = 0;
    if (file.is_open()) {
        std::string temp, safe = "";
        while (std::getline(file, temp)) {
            temp = temp.substr(0, temp.find(';')) + ";";
            if(safe.find(temp) == std::string::npos) {
                safe += temp;
                count++;
            }
        }
        file.close();
    }
    return count;
}

/**
    Get amount of Training for certain Cyclist in certain file.

    @param c Cyclist.
    @param source Path to file.
    @return Amount of Cyclist's Training.
*/
unsigned int sumOfTrainings(const Cyclist& c, const std::string& source) {
    unsigned int count = 0;
    std::fstream file(source.c_str(), std::ios::in);
    if (file.is_open()) {
        std::string temp;
        while (std::getline(file, temp)) {
            if (c.name == temp.substr(0, temp.find(';'))) {
                count++;
            }
        }
        file.close();
    }
    return count;
}

/**
    Constructor (equivalent to HeapArrayInterface<T> handler(0)).
*/
template <typename T> HeapArrayInterface<T>::HeapArrayInterface(void) {
    this->container_size = 0;
    iter_reset();
}

/**
    Constructor.

    @param range Size of HeapArrayInterface internal storage array.
*/
template <typename T> HeapArrayInterface<T>::HeapArrayInterface(int new_size) {
    if (new_size > 0) {
        this->container_size = new_size;
        this->content_container = new T[this->container_size];
    } else {
        this->container_size = 0;
    }
    iter_reset();
}

/**
    Releases heap memory when size is greater than 0.
*/
template <typename T> void HeapArrayInterface<T>::deallocate() {
    if (this->container_size > 0) {
        delete[] this->content_container;
    }
}

/**
    Resize HeapArrayInterface internal storage array to certain size.

    @param range New size of HeapArrayInterface internal storage array.
    @param keep_data Copy data from old storage array to new one.
    @return Success value;
*/
template <typename T> bool HeapArrayInterface<T>::resize(int new_size, bool keep_data) {
    iter_reset();
    if (new_size == this->container_size || new_size < 0) {
        return false;
    } else if (new_size == 0) {
        purge();
        return true;
    } else {
        T* replacement = new T[new_size];
        if (keep_data) {
            for (int i = 0; i < (new_size < this->container_size ? new_size : this->container_size); i++) {
                replacement[i] = this->content_container[i];
            }
        }
        deallocate();
        this->content_container = replacement;
        this->container_size = new_size;
        replacement = NULL;
        return true;
    }
}

/**
    Clear heap memory and set size of HeapArrayInterface internal storage to 0.
*/
template <typename T> void HeapArrayInterface<T>::purge(void) {
    deallocate();
    this->container_size = 0;
    this->content_container = NULL;
}

/**
    Set value in HeapArrayInterface on index to.

    @param index Index where to store object.
    @param value Object to be stored.
    @return Success value.
*/
template <typename T> bool HeapArrayInterface<T>::set(int index, const T& value) {
    if (in_bounds(index)) {
        this->content_container[index] = value;
        return true;
    } else {
        return false;
    }
}

/**
    Get size of HeapArrayInterface internal storage.

    @return Size of HeapArrayInterface internal storage.
*/
template <typename T> int HeapArrayInterface<T>::size(void) {
    return this->container_size;
}

/**
    Reset pseudo-iterator to 0.
*/
template <typename T> void HeapArrayInterface<T>::iter_reset(void) {
    this->internal_iterator = 0;
}

/**
    Set pseudo-iterator to user value.

    @param iterator_new New value of pseudo-iterator.
    @return Success value.
*/
template <typename T> bool HeapArrayInterface<T>::iter_set(int iterator_position) {
    if (in_bounds(iterator_position)) {
        this->internal_iterator = iterator_position;
        return true;
    } else {
        return false;
    }
}

/**
    Return object at pseudo-iterator index.

    @param iteratorSetting Operation to be done with pseudo-iterator.
    @return Object at index.
*/
template <typename T> T& HeapArrayInterface<T>::iter_current(ITER_OPERATION iterator_operation) {
    int iterator_old = this->internal_iterator;
    int iterator_new = this->internal_iterator + iterator_operation;
    if (in_bounds(iterator_new)) {
        this->internal_iterator = iterator_new;
    } else if (iterator_new < 0) {
        this->internal_iterator = this->container_size - 1;
    } else if (iterator_new > this->container_size - 1){
        this->internal_iterator = 0;
    }
    return this->content_container[iterator_old];
}

/**
    Return object at pseudo-iterator index (equivalent to HeapArrayInterface::iter_current(HeapArrayInterface::IGNORE)).

    @return Object at index.
*/
template <typename T> T& HeapArrayInterface<T>::iter_current(void) {
    return iter_current(IGNORE);
}

/**
    Return value of pseudo-iterator.

    @return Value of pseudo-iterator.
*/
template <typename T> int HeapArrayInterface<T>::iter_at(void) {
    return this->internal_iterator;
}

/**
    Return true if index in in bounds (0 <= index < container_size)
*/
template <typename T> bool HeapArrayInterface<T>::in_bounds(int index) {
    return (index >= 0 && index < this->container_size);
}
