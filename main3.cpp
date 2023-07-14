#include <iostream>
#include <ctime>
#include <sstream>
#include <string>
#include <vector>
#include <limits>
#include <iomanip>
#include <fstream>


using namespace std;

class ReservationSystem {
private:
    struct Room {
        bool occupied;
        int roomNumber;
        int capacity;
        string roomName;
    };

    struct Reservation {
        time_t startTime;
        time_t endTime;
        int roomNumber;
        int day;
        int month;
        int year;
        string profName;
        string section;
        int capacity;
        string roomName;
    };

    vector<Room> rooms;
    vector<Reservation> reservations;
    string reservationDataFile;

public:
    ReservationSystem() {
        // Initialize rooms
        rooms.resize(15);
        rooms[0] = {false, 1, 40, "Room 101"};
        rooms[1] = {false, 2, 40, "Room 102"};
        rooms[2] = {false, 3, 40, "Room 103"};
        rooms[3] = {false, 4, 40, "Room 104"};
        rooms[4] = {false, 5, 40, "Room 201"};
        rooms[5] = {false, 6, 40, "Room 202"};
        rooms[6] = {false, 7, 40, "Room 203"};
        rooms[7] = {false, 8, 40, "SL1"};
        rooms[8] = {false, 9, 40, "SL2"};
        rooms[9] = {false, 10, 40, "SL3"};
        rooms[10] = {false, 11, 40, "ML"};
        rooms[11] = {false, 12, 40, "Cisco Lab"};
        rooms[12] = {false, 13, 40, "CEL"};
        rooms[13] = {false, 14, 40, "Room 401"};
        rooms[14] = {false, 15, 40, "Room 402"};
        
        reservationDataFile = "reservations.csv";

        ofstream file(reservationDataFile, ios::app);
        file.close();
    }

    void displayRoomList() {
        cout << "=======================================" << endl;
        cout << "|            CICS BUILDING            |" << endl;
        cout << "=======================================" << endl;
        cout << "+----+-------+-----+----+-------+-----+" << endl;
        cout << "| ID |  Room | CAP | ID |  Room | CAP |" << endl;
        cout << "+----+-------+-----+----+-------+-----+" << endl;
        cout << "|  1 |  101  |  40 |  9 |  SL2  | 40  |" << endl;
        cout << "|  2 |  102  |  40 | 1O |  SL3  | 40  |" << endl;
        cout << "|  3 |  103  |  40 | 11 |  ML   | 40  |" << endl;
        cout << "|  4 |  104  |  40 | 12 | Cisco | 40  |" << endl;
        cout << "|  5 |  201  |  40 | 13 |  CEL  | 40  |" << endl;
        cout << "|  6 |  202  |  40 | 14 |  401  | 40  |" << endl;
        cout << "|  7 |  203  |  40 | 15 |  402  | 40  |" << endl;
        cout << "|  8 |  SL1  |  40 |    |       |     |" << endl;
        cout << "+----+-------+-----+----+-------+-----+" << endl;
    }

    void reserveRoom() {
        cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "|           RESERVING A ROOM          |" << endl;
        cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        displayRoomList();

        int roomNumber;
        cout << "Enter the room ID you wish to occupy/reserve(1-15): ";
        cin >> roomNumber;

        if (roomNumber >= 1 && roomNumber <= 15 && !rooms[roomNumber - 1].roomName.empty()) {
            string dateString;
            cout << "Enter the date of occupancy(DD MM YYYY): ";
            cin.ignore();
            getline(cin, dateString);
            istringstream dateStream(dateString);

            Reservation reservation;
            reservation.roomNumber = roomNumber;
            reservation.capacity = rooms[roomNumber - 1].capacity;
            reservation.roomName = rooms[roomNumber - 1].roomName;

            dateStream >> reservation.day;
            dateStream.ignore();
            dateStream >> reservation.month;
            dateStream.ignore();
            dateStream >> reservation.year;

            cout << "Enter the start time (HH:MM): ";
            string timeString;
            getline(cin, timeString);
            istringstream timeStreamStart(timeString);
            tm inputTimeStart{};
            timeStreamStart >> inputTimeStart.tm_hour;
            timeStreamStart.ignore();
            timeStreamStart >> inputTimeStart.tm_min;

            cout << "Enter the end time (HH:MM): ";
            getline(cin, timeString);
            istringstream timeStreamEnd(timeString);
            tm inputTimeEnd{};
            timeStreamEnd >> inputTimeEnd.tm_hour;
            timeStreamEnd.ignore();
            timeStreamEnd >> inputTimeEnd.tm_min;

            inputTimeStart.tm_sec = 0;
            inputTimeStart.tm_year = reservation.year - 1900;
            inputTimeStart.tm_mon = reservation.month - 1;
            inputTimeStart.tm_mday = reservation.day;
            inputTimeStart.tm_min = inputTimeStart.tm_min;
            inputTimeStart.tm_hour = inputTimeStart.tm_hour;

            inputTimeEnd.tm_sec = 0;
            inputTimeEnd.tm_year = reservation.year - 1900;
            inputTimeEnd.tm_mon = reservation.month - 1;
            inputTimeEnd.tm_mday = reservation.day;
            inputTimeEnd.tm_min = inputTimeEnd.tm_min;
            inputTimeEnd.tm_hour = inputTimeEnd.tm_hour;

            reservation.startTime = mktime(&inputTimeStart);
            reservation.endTime = mktime(&inputTimeEnd);

            // Check for overlapping reservations
            bool overlap = false;
            for (const auto& existingReservation : reservations) {
                if (reservation.roomNumber == existingReservation.roomNumber &&
                    reservation.startTime < existingReservation.endTime &&
                    reservation.endTime > existingReservation.startTime) {
                    overlap = true;
                    break;
                }
            }

            if (overlap) {
                cout << "The room is already reserved during the specified time. Request rejected." << endl;
                return;
            }

            string section;
            cout << "Enter Section: ";
            getline(cin, section);

            reservation.section = section;

            // Check capacity
            int numPeople;
            cout << "Enter Number of Students: ";
            cin >> numPeople;
            if (numPeople > reservation.capacity) {
                cout << "The number of people exceeds the room's capacity. Request rejected." << endl;
                return;
            }

            string profName;
            cout << "Enter Teacher's Name: ";
            cin.ignore();
            getline(cin, profName);

            reservation.profName = profName;

            reservations.push_back(reservation); // Store the reservation

            cout << "Room " << rooms[roomNumber - 1].roomName << " reserved." << endl;
            
            saveReservation(reservation);
            
        } else {
            cout << "Invalid room number." << endl;
        }
    }
	
	void saveReservation(const Reservation& reservation) {
        ofstream file("reservations.csv", ios::app);
        if (file) {
            file << reservation.roomNumber << ","
                 << reservation.roomName << ","
                 << reservation.day << "/"
                 << reservation.month << "/"
                 << reservation.year << ","
                 << reservation.startTime << ","
                 << reservation.endTime << ","
                 << reservation.capacity << ","
                 << reservation.section << ","
                 << reservation.profName << "\n";
            file.close();
        } else {
            cout << "Failed to open reservations database file." << endl;
        }
    }

    void loadReservations() {
        ifstream file("reservations.csv");
        if (file) {
            string line;
            while (getline(file, line)) {
                istringstream iss(line);
                Reservation reservation;
                string field;

                getline(iss, field, ',');
                reservation.roomNumber = stoi(field);

                getline(iss, reservation.roomName, ',');

                getline(iss, field, '/');
                reservation.day = stoi(field);
                getline(iss, field, '/');
                reservation.month = stoi(field);
                getline(iss, field, ',');
                reservation.year = stoi(field);

                getline(iss, field, ',');
                reservation.startTime = stoi(field);
                getline(iss, field, ',');
                reservation.endTime = stoi(field);

                getline(iss, field, ',');
                reservation.capacity = stoi(field);

                getline(iss, reservation.section, ',');

                getline(iss, reservation.profName, '\n');

                reservations.push_back(reservation);
            }
            file.close();
        } else {
            cout << "Failed to open reservations database file." << endl;
        }
    }
    
    void checkReservations() {
        cout << "+---------------------------+" << endl;
        cout << "¦   CHECKING RESERVATIONS   ¦" << endl;
        cout << "+---------------------------+" << endl;

        for (const auto& reservation : reservations) {
            cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
            cout << "| Teacher: " << reservation.profName << endl;
            cout << "| Room: " << reservation.roomNumber << endl;
            cout << "| Section: " << reservation.section << endl;
            cout << "| Start time: " << asctime(localtime(&reservation.startTime));
            cout << "| End time: " << asctime(localtime(&reservation.endTime));
            cout << "| Occupancy date: " << reservation.day << "/" << reservation.month << "/" << reservation.year << endl;
            cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-" << endl;
            cout << endl;
        }
    }

    void checkRoomStatus() {
        cout << "+--------------------------+" << endl;
        cout << "¦   CHECKING ROOM STATUS   ¦" << endl;
        cout << "+--------------------------+" << endl;
        
        time_t currentTime = time(0);
        tm* currentTime_tm = localtime(&currentTime);

        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "|    ROOM     |    STATUS   |    PROFESSOR    |   SECTION    |       END TIME          " << endl;
        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;

        for (const auto& room : rooms) {
            bool roomOccupied = false;

            for (const auto& reservation : reservations){
                if (room.roomNumber == reservation.roomNumber) {
                    if (currentTime >= reservation.startTime && currentTime < reservation.endTime) {
                        roomOccupied = true;
                        cout << "| " << left << setw(12) << room.roomName << "|     OCCUPIED| " << right << setw(16) << reservation.profName << "| " << right << setw(13) << reservation.section << "| " << asctime(localtime(&reservation.endTime));
                        break;
                    }
                }
            }

            if (!roomOccupied) {
                cout << "| " << left << setw(12) << room.roomName << "|       VACANT|" << right << setw(18)<< "|" << right << setw(15) << "|" << endl;
            }
        }

        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "Enter to go back to Menu --->";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }

    void run() {
    	
    	loadReservations();
        int choice = 0;
        while (choice != 4) {
            time_t currentTime = time(0);
            tm* currentTime_tm = localtime(&currentTime);
            cout << "Time: " << asctime(currentTime_tm);
            cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
            cout << "|           MENU             |" << endl;
            cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
            cout << "| [1] Occupy a room          |" << endl;
            cout << "| [2] Check history          |" << endl;
            cout << "| [3] Check room status      |" << endl;
            cout << "| [4] Exit                   |" << endl;
            cout << "| [5] Clear Screen           |" << endl;
            cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
            cout << "Choice: ";
            cin >> choice;

            switch (choice) {
                case 1:
                    reserveRoom();
                    break;

                case 2:
                    checkReservations();
                    break;

                case 3:
                    checkRoomStatus();
                    break;

                case 4:
                    cout << "Exiting the program." << endl;
                    break;

                case 5:
                    system("CLS");
                    break;

                default:
                    cout << "Invalid choice. Please try again." << endl;
                    break;
            }
        }
    }
};

int main() {
    ReservationSystem reservationSystem;
    reservationSystem.run();

    return 0;
}
