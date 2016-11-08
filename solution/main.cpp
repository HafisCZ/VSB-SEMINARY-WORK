#include <iostream>
#include <fstream>
#include <string>

#include <cstdio>
#include <cstring>
#include <cstdlib>

/**
    Class for managing heap-stored objects.

    @param T type specifier
*/
template <typename T> class DynamicHandler {
    private:
        T *content;
        unsigned int range;
        int internal_iterator;

        /**
            Releases heap memory when size is greater than 0
        */
        void dealloc() {
            if (this->range > 0) {
                delete[] this->content;
            }
        }
    public:
        enum ITERATOR_W {DECREASE = -1, IGNORE = 0, INCREASE = 1};

        DynamicHandler(void);
        DynamicHandler(unsigned int range);
        void purge(void);
        void iter_reset(void);
        bool iter_set(int iterator_new);
        bool expand(unsigned int range);
        bool expandBy(unsigned int range);
        bool set(unsigned int index, const T& value);
        int iter_at(void);
        unsigned int size(void);
        T& iter_current(ITERATOR_W iteratorSetting);
        T& iter_current(void);


        /**
            Operator [] overload, returns object at index

            @param index index
            @return object at index
        */
        T& operator[](unsigned int index) {
            if (index >= 0 && index < this->range) {
                return this->content[index];
            } else {
                T* null = NULL;
                return *null;
            }
        }
};

/**
    Structure with training's time and distance

    @param Training.distance Distance of training
    @param Training.duration Duration of training
*/
struct Training {
    double distance, duration;

    Training(){}
    Training(double distance, double duration);
};

/**
    Structure with cyclist's name and training sessions

    @param Cyclist.name Name of cyclist
    @param Cyclist.trains DynamicHandler with training sessions
*/
struct Cyclist {
    private:
        int lastDistanceCheck, lastDurationCheck;
        double totalDistance, totalDuration;

        /**
            Set all internal counters/values
        */
        void preset(void) {
            this->totalDistance = 0;
            this->totalDuration = 0;
            this->lastDistanceCheck = -1;
            this->lastDurationCheck = -1;
        }
    public:
        std::string name;
        DynamicHandler<Training> trains;

        Cyclist(void);
        Cyclist(const std::string& name);
        double getTotalDistance(void);
        double getTotalDuration(void);
        double getAverageDistance(void);
        double getAverageDuration(void);
};

void sort(DynamicHandler<Cyclist>& cyclists);
void sortSwap(DynamicHandler<Cyclist>& cyclists, int indexA, int indexB);
void sortQuick(DynamicHandler<Cyclist>& cyclists, int zeroIndex, int aboveIndex);
bool readFile(DynamicHandler<Cyclist> &database, const std::string& source);
bool outputHtml(DynamicHandler<Cyclist> &cyclists, const std::string& target);
unsigned int sumOfCyclists(const std::string& source);
unsigned int sumOfTrainings(const Cyclist& c, const std::string& source);

int main()
{
    DynamicHandler<Cyclist> cyclists;
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
    for (unsigned int i = 0; i < cyclists.size(); i++) {
        Cyclist sub = cyclists[i];
        std::cout << std::endl << sub.name << std::endl << "Pocet: " << sub.trains.size() << " Celkem: " << sub.getTotalDistance() << "km / " << sub.getTotalDuration() << "h\tPrumer: " << sub.getAverageDistance() << "hm / " << sub.getAverageDuration() << "h" << std::endl;
        for (unsigned int j = 0; j < sub.trains.size(); j++) {
            std::cout << " - Trenink " << j + 1 << " Ujeto: " << sub.trains[j].distance << "km / " << sub.trains[j].duration << "h" << std::endl;
        }
        sub.trains.purge();
        if (i == cyclists.size() - 1) {
            for (unsigned int j = 0; j < cyclists.size(); j++) {
                cyclists[i].trains.purge();
            }
            cyclists.purge();
        }
    }
    return 0;
}

/**
        Reads specified file and fills DynamicHandler of Cyclist

        @param database DynamicHandler target
        @param source path to file
        @return success value
*/
bool readFile(DynamicHandler<Cyclist> &database, const std::string& source) {
    std::fstream file(source.c_str(), std::ios::in);
    if (file.is_open()) {
        DynamicHandler<std::string> temporary(3);
        database.expand(sumOfCyclists(source));
        std::string temp;
        while (std::getline(file, temp)) {
            temporary.iter_reset();
            double train_distance;
            double train_duration;
            bool exists = false;
            size_t position;
            while ((position = temp.find(';')) != std::string::npos) {
                temporary.iter_current(temporary.INCREASE) = temp.substr(0, position);
                temp.erase(0, position + 1);
            }
            temporary[2] = temp;
            train_distance = strtod(temporary[1].c_str(), NULL);
            train_duration = strtod(temporary[2].c_str(), NULL);
            for (unsigned int i = 0; i < database.size() && !exists; i++) {
                if (database[i].name == temporary[0]) {
                    database[i].trains.iter_current(DynamicHandler<Training>::INCREASE) = Training(train_distance, train_duration);
                    exists = true;
                }
            }
            if (!exists) {
                database.iter_current() = Cyclist(temporary[0]);
                database.iter_current().trains.expand(sumOfTrainings(database[database.iter_at()], source));
                database.iter_current(database.INCREASE).trains.iter_current(DynamicHandler<Training>::INCREASE) = Training(train_distance, train_duration);
            }

        }
        temporary.purge();
        file.close();
        return true;
    } else {
        return false;
    }
}

/**
    Writes all information in DynamicHandler with Cyclists into HTML format

    @param cyclists DynamicHandler of Cyclists
    @param target path where output file should be
    @return success value
*/
bool outputHtml(DynamicHandler<Cyclist> &cyclists, const std::string& target) {
    std::fstream file (target.c_str(), std::ios::out | std::ios::trunc);
    if (file.is_open()) {
        file << "<html>" << std::endl << "<head>" << std::endl << "<meta http-equiv=\"Content-Type\" content=\"test/html;charset=utf-8\">" << std::endl;
        file << "<title>Seznam cyklistu</title>" << std::endl << "</head>" << std::endl << "<body>" << std::endl;
        file << "<table style=\"width:100%; border:1px solid black; border-collapse:collapse\" rules=rows>" << std::endl;
        file << "<caption>Statistika cyklistu</caption>" << std::endl;
        file << "<tr><th>Zavodnik</th><th>Ujeta vzdalenost [km]</th><th>Celkovy cas na kole [h]</th><th>Prumerna ujeta vzdalenost [km]</th><th>Prumerny cas na kole [h]</th></tr>" << std::endl;
        for (unsigned int i = 0; i < cyclists.size(); i++) {
            file << "<tr><th>" << cyclists[i].name << "</th><td>" << cyclists[i].getTotalDistance() << "</td><td>" << cyclists[i].getTotalDuration() << "</td><td>" << cyclists[i].getAverageDistance() << "</td><td>" << cyclists[i].getAverageDuration() << "</td></tr>" << std::endl;
        }
        file << "</table>" << std::endl;
        file << "<table style=\"width:100%; border:1px solid black; border-collapse:collapse\" rules=rows>" << std::endl;
        file << "<caption>Treninky cyklistu</caption>" << std::endl;
        file << "<tr><th>Zavodnik</th><th>Ujeta vzdalenost [km]</th><th>Cas treninku [h]</th></tr>" << std::endl;
        for (unsigned int i = 0; i < cyclists.size(); i++) {
            file << "<tr><td rowspan=" << cyclists[i].trains.size() + 1 << ">" << cyclists[i].name << "</td><td>Prumer: " << cyclists[i].getAverageDistance() << "</td><td>Prumer: " << cyclists[i].getAverageDuration() << "</td></tr>" << std::endl;
            for (unsigned int j = 0; j < cyclists[i].trains.size(); j++) {
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
    Quick sort method for DynamicHandler of Cyclist

    @param cyclists DynamicHandler of Cyclist
    @param zeroIndex lowest index
    @param aboveIndex size of DynamicHandler
*/
void sortQuick(DynamicHandler<Cyclist>& cyclists, int zeroIndex, int aboveIndex) {
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
    Swap two entries in DynamicHandler of Cyclist

    @param cyclists DynamicHandler of Cyclist
    @param indexA point A
    @param indexB point B
*/
void sortSwap(DynamicHandler<Cyclist>& cyclists, int indexA, int indexB) {
    Cyclist temporary = cyclists[indexB];
    cyclists[indexB] = cyclists[indexA];
    cyclists[indexA] = temporary;
}

/**
    Training constructor

    @param distance distance of training
    @param duration duration of training
*/
Training::Training(double distance, double duration) {
    this->distance = distance;
    this->duration = duration;
}

/**
    Default Cyclist constructor
*/
Cyclist::Cyclist(void) {
    preset();
}

/**
    Cyclist constructor

    @param name Name of cyclist
*/
Cyclist::Cyclist(const std::string& name){
    preset();
    this->name = name;
}

/**
    Total training distance

    @return total distance
*/
double Cyclist::getTotalDistance(void) {
    if (this->lastDistanceCheck != (int) trains.size()) {
        this->totalDistance = 0;
        for (unsigned int i = 0; i < trains.size(); i++) {
            this->totalDistance += trains[i].distance;
        }
        this->lastDistanceCheck = trains.size();
    }
    return this->totalDistance;
}

/**
    Total training duration

    @return total duration
*/
double Cyclist::getTotalDuration(void) {
    if (this->lastDurationCheck != (int) trains.size()) {
        this->totalDuration = 0;
        for (unsigned int i = 0; i < trains.size(); i++) {
            this->totalDuration += trains[i].duration;
        }
        this->lastDurationCheck = trains.size();
    }
    return this->totalDuration;
}

/**
    Average training distance

    @return average distance
*/
double Cyclist::getAverageDistance(void) {
    if (trains.size() > 0) {
        return (getTotalDistance() / this->lastDistanceCheck);
    } else {
        return 0;
    }
}

/**
    Average training duration

    @return average duration
*/
double Cyclist::getAverageDuration(void) {
    if (trains.size() > 0) {
        return (getTotalDuration() / this->lastDurationCheck);
    } else {
        return 0;
    }
}

/**
    Get total amount of cyclists

    @param source path to file
    @return amount of cyclists
*/
unsigned int sumOfCyclists(const std::string& source) {
    std::fstream file(source.c_str(), std::ios::in);
    unsigned int count = 0;
    if (file.is_open()) {
        std::string temp, safe = "";
        while (std::getline(file, temp)) {
            temp = temp.substr(0, temp.find(';'));
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
    Get total trainings of cyclist in file

    @param c cyclist
    @param source path to file
    @return amount of trainings
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
            Constructor (automatic)
*/
template <typename T> DynamicHandler<T>::DynamicHandler(void) {
    this->range = 0;
    iter_reset();
}

/**
    Constructor (initial size specified at initialization)

    @param range of storage
*/
template <typename T> DynamicHandler<T>::DynamicHandler(unsigned int range) {
    if (range > 0) {
        this->range = range;
        this->content = new T[this->range];
    } else {
        this->range = 0;
    }
    iter_reset();
}

/**
    Expands storage

    @param range of new storage
    @return success value
*/
template <typename T> bool DynamicHandler<T>::expand(unsigned int range) {
    if (range > this->range) {
        T *replacement = new T[range];
        for (unsigned int i = 0; i < this->range; i++) {
            replacement[i] = this->content[i];
        }
        dealloc();
        this->content = replacement;
        this->range = range;
        replacement = NULL;
        iter_reset();
        return true;
    } else {
        return false;
    }
}

/**
    Expands storage by

    @param range modifier of new storage
    @return success value
*/
template <typename T> bool DynamicHandler<T>::expandBy(unsigned int range) {
    if (range > 0) {
        return expand(range + this->range);
    } else {
        return false;
    }
}

/**
    Clear heap memory
*/
template <typename T> void DynamicHandler<T>::purge(void) {
    dealloc();
    this->range = 0;
    this->content = NULL;
}

/**
    Set value in storage on index

    @param index where to store
    @param value to be stored
    @return success value
*/
template <typename T> bool DynamicHandler<T>::set(unsigned int index, const T& value) {
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
template <typename T> unsigned int DynamicHandler<T>::size(void) {
    return this->range;
}

/**
    Reset iterator to 0
*/
template <typename T> void DynamicHandler<T>::iter_reset(void) {
    this->internal_iterator = 0;
}

/**
    Set iterator to custom value

    @param iterator_new new index
    @return success value
*/
template <typename T> bool DynamicHandler<T>::iter_set(int iterator_new) {
    if (iterator_new > -1 && iterator_new < this->range) {
        this->internal_iterator = iterator_new;
        return true;
    } else {
        return false;
    }
}

/**
    Return object at iterator point

    @param iteratorSetting what to do with internal iterator
    @return object at index
*/
template <typename T> T& DynamicHandler<T>::iter_current(ITERATOR_W iteratorSetting) {
    int iterator_old = this->internal_iterator;
    int iterator_new = this->internal_iterator + (int) iteratorSetting;
    if (iterator_new > -1 && iterator_new < (int) this->range) {
        this->internal_iterator = iterator_new;
    } else if (iterator_new < 0) {
        this->internal_iterator = (int) this->range - 1;
    } else if (iterator_new > (int) this->range - 1){
        this->internal_iterator = 0;
    }
    return this->content[iterator_old];
}

/**
    Return object at iterator point, leaves internal iterator as it was before call

    @return object at index
*/
template <typename T> T& DynamicHandler<T>::iter_current(void) {
    return iter_current(IGNORE);
}

/**
    Return iterator position

    @return index in iterator
*/
template <typename T> int DynamicHandler<T>::iter_at(void) {
    return this->internal_iterator;
}
