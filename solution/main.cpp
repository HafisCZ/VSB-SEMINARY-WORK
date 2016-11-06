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

        /**
            Constructor (automatic)
        */
        DynamicHandler(void) {
            this->range = 0;
            iter_reset();
        }

        /**
            Constructor (initial size specified at initialization)

            @param range of storage
        */
        DynamicHandler(unsigned int range) {
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
        bool expand(unsigned int range) {
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
            Shrinks storage by

            @param range modifier of new storage
            @return success value
        */
        bool expandBy(unsigned int range) {
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
        bool remove(unsigned int index) {
            if (index >= 0 && index < this->range) {
                T* replacement = new T[this->range - 1];
                for (unsigned int i = 0; i < this->range; i++) {
                    if (i == index) {
                        continue;
                    } else {
                        replacement[(i > index ? i : i - 1)] = this->content[i];
                    }
                }
                dealloc();
                this->content = replacement;
                this->range -= 1;
                replacement = NULL;
                iter_reset();
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
        bool shrink(unsigned int range) {
            if (range < this->range) {
                T* replacement = new T[range];
                for (unsigned int i = 0; i < range; i++) {
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
            Shrinks storage by

            @param range modifier of new storage
            @return success value
        */
        bool shrinkBy(unsigned int range) {
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
        void prefill(const T& value) {
            for (unsigned int i = 0; i < this->range; i++) {
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
            Set value in storage on index

            @param index where to store
            @param value to be stored
            @return success value
        */
        bool set(unsigned int index, const T& value) {
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
        inline unsigned int size(void) {
            return this->range;
        }

        /**
            Push value to back of content, expands storage by 1

            @param value to be stored
            @return success value
        */
        bool push_back(const T& value) {
            if (expandBy(1) && set(this->range - 1, value)) {
                return true;
            } else {
                return false;
            }
        }

        /**
            Push whole DynamicHandler into another DynamicHandler

            @param insertion DynamicHandler to be inserted
            @return success value
        */
        bool insert(const DynamicHandler<T>& insertion) {
            if (insertion.size() > 0) {
                for (unsigned int i = 0; i < insertion.size(); i++) {
                    push_back(insertion.get(i));
                }
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
        bool push_front(const T& value) {
            T *replacement = new T[this->range + 1];
            for (unsigned int i = 0; i < this->range; i++) {
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
        bool extract(T target[], unsigned int begin, unsigned int end) {
            if (begin >= 0 && end <= this->range && begin <= end) {
                for (unsigned int i = begin; i < end; i++) {
                    target[i] = this->content[i];
                }
                return true;
            } else {
                return false;
            }
        }

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

        /**
            Operator + overload, return DynamicHandler

            @param insertion which should be joined
            @return DynamicHandler
        */
        DynamicHandler<T>& operator+(const T& insertion) {
            push_back(insertion);
            return *this;
        }

        /**
            Reset iterator to 0
        */
        inline void iter_reset(void) {
            this->internal_iterator = 0;
        }

        /**
            Set iterator to custom value

            @param iterator_new new index
            @return success value
        */
        bool iter_set(int iterator_new) {
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
        T& iter_current(ITERATOR_W iteratorSetting) {
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
        T& iter_current(void) {
            return iter_current(IGNORE);
        }

        /**
            Return iterator position

            @return index in iterator
        */
        inline int iter_at(void) {
            return this->internal_iterator;
        }
};

/**
    Structure with training's time and distance

    @param Training.distance Distance of training
    @param Training.duration Duration of training
*/
struct Training {
    double distance, duration;

    /**
        Default constructor
    */
    Training(){}

    /**
        Constructor

        @param distance distance of training
        @param duration duration of training
    */
    Training(double distance, double duration) {
        this->distance = distance;
        this->duration = duration;
    }
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

        /**
            Default constructor
        */
        Cyclist(void) {
            preset();
        }

        /**
            Constructor

            @param name Name of cyclist
        */
        Cyclist(const std::string& name){
            preset();
            this->name = name;
        }

        /**
            Total training distance

            @return total distance
        */
        double getTotalDistance(void) {
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
        double getTotalDuration(void) {
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
        double getAverageDistance(void) {
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
        double getAverageDuration(void) {
            if (trains.size() > 0) {
                return (getTotalDuration() / this->lastDurationCheck);
            } else {
                return 0;
            }
        }
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
        std::cout << std::endl << sub.name << std::endl << "\tPocet: " << sub.trains.size() << "\tCelkem: " << sub.getTotalDistance() << "km / " << sub.getTotalDuration() << "h\tPrumer: " << sub.getAverageDistance() << "hm / " << sub.getAverageDuration() << "h" << std::endl;
        for (unsigned int j = 0; j < sub.trains.size(); j++) {
            std::cout << " - Trenink " << j + 1 << "\tUjeto: " << sub.trains[j].distance << "km / " << sub.trains[j].duration << "h" << std::endl;
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
