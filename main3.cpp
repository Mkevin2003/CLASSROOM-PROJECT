#include <iostream>
#include <sstream>
#include <string>
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
        Room* next;
        
        Room(int roomNumber, int capacity, const string& roomName)
            : occupied(false), roomNumber(roomNumber), capacity(capacity), roomName(roomName), next(nullptr) {}
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
        int referenceNumber;
        Reservation* next;
        
        Reservation()
            : startTime(0), endTime(0), roomNumber(0), day(0), month(0), year(0), capacity(0),
              referenceNumber(0), next(nullptr) {}
    };

    Room* rooms;
    Room* roomsHead;
    Reservation* reservations;
    string reservationDataFile;

    int nextReferenceNumber;

public:
    ReservationSystem() {
        nextReferenceNumber = 1;
        rooms = nullptr;
        reservations = nullptr;
        roomsHead = nullptr; 

        createRooms();
        reservationDataFile = "reservations.csv";
        loadReservationsFromFile();
    }

    void createRooms() {
        string roomNames[] = {
            "101", "102", "103", "104", "201",
            "202", "203", "SL1", "SL2", "SL3", "ML", "Cisco",
            "CEL", "401", "402"
        };
        int capacity[] = {40,40,40,40,40,40,30,30,40,50,40,40,40,30,40};
        int numRooms = sizeof(roomNames) / sizeof(roomNames[0]);

        rooms = new Room(1, capacity[0], roomNames[0]);
        Room* currentRoom = rooms;

        for (int i = 1; i < numRooms; i++) {
            currentRoom->next = new Room(i + 1, capacity[i], roomNames[i]);
            currentRoom = currentRoom->next;
        }
    }

    void addReservationNode(const Reservation& reservation)
	{
        Reservation* newNode = new Reservation(reservation);
        newNode->next = nullptr;

        if (reservations == nullptr) {
            reservations = newNode;
        } else {
            Reservation* temp = reservations;
            while (temp->next != nullptr) {
                temp = temp->next;
            }
            temp->next = newNode;
        }
    }

    void updateReservationFile() 
	{
        ofstream file(reservationDataFile);
        if (file) {
            Reservation* temp = reservations;
            while (temp != nullptr) {
                file << temp->roomNumber << ","
                     << temp->roomName << ","
                     << temp->day << "/"
                     << temp->month << "/"
                     << temp->year << ","
                     << temp->startTime << ","
                     << temp->endTime << ","
                     << temp->capacity << ","
                     << temp->section << ","
                     << temp->profName << ","
                     << temp->referenceNumber << "\n";
                temp = temp->next;
            }
            file.close();
        } else {
            cout << "Failed to open reservations database file." << endl;
        }
    }
    
	void loadReservationsFromFile() {
        ifstream file(reservationDataFile);
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
                getline(iss, reservation.profName, ',');
                getline(iss, field, '\n');
                reservation.referenceNumber = stoi(field);

                addReservationNode(reservation);
            }
            file.close();

            // Find the highest reference number and set nextReferenceNumber
            int highestReferenceNumber = 0;
            Reservation* temp = reservations;
            while (temp != nullptr) {
                if (temp->referenceNumber > highestReferenceNumber) {
                    highestReferenceNumber = temp->referenceNumber;
                }
                temp = temp->next;
            }
            nextReferenceNumber = highestReferenceNumber + 1;
        } else {
            cout << "Failed to open reservations database file." << endl;
        }
    }

    time_t getTimestamp(int day, int month, int year, int hour, int minute) 
	{
        struct tm timeStruct;
        timeStruct.tm_sec = 0;
        timeStruct.tm_min = minute;
        timeStruct.tm_hour = hour;
        timeStruct.tm_mday = day;
        timeStruct.tm_mon = month - 1;
        timeStruct.tm_year = year - 1900;
        timeStruct.tm_isdst = -1;
        
        return mktime(&timeStruct);
    }

    int getRoomCapacity(int roomNumber) {
        Room* temp = rooms;
        while (temp != nullptr) {
            if (roomNumber == temp->roomNumber) {
                return temp->capacity;
            }
            temp = temp->next;
        }
        return 0; // Room not found
    }

    string getRoomName(int roomNumber) {
        Room* temp = rooms;
        while (temp != nullptr) {
            if (roomNumber == temp->roomNumber) {
                return temp->roomName;
            }
            temp = temp->next;
        }
        return "N/A"; // Room not found
    }
    
    bool isValidDate(int day, int month, int year) 
	{
        if (year < 1900 || year > 9999)
            return false;

        if (month < 1 || month > 12)
            return false;

        int maxDay = 31; 

        if (month == 4 || month == 6 || month == 9 || month == 11)
            maxDay = 30;
        else if (month == 2) {
            if ((year % 4 == 0 && year % 100 != 0) || year % 400 == 0)
                maxDay = 29;
            else
                maxDay = 28;
        }

        return (day >= 1 && day <= maxDay);
    }
	
    void displayRoomList() {
        cout << "=======================================" << endl;
        cout << "|            CICS BUILDING            |" << endl;
        cout << "=======================================" << endl;
        cout << "+----+-------+-----+----+-------+-----+" << endl;
        cout << "| ID |  Room | CAP | ID |  Room | CAP |" << endl;
        cout << "+----+-------+-----+----+-------+-----+" << endl;

        Room* temp = rooms;
        while (temp != nullptr)
		{
            cout << "|  " << setw(2) << temp->roomNumber << "|" << setw(5) << temp->roomName << "  | " << setw(3) << temp->capacity << " | ";
            temp = temp->next;
            
            if (temp != nullptr) 
			{
                cout << " " << setw(2) << temp->roomNumber << "| " << setw(5) << temp->roomName << " | " << setw(3) << temp->capacity << " | ";
                temp = temp->next;
            }
            cout << endl;
        }

        cout << "+----+-------+-----+----+-------+-----+" << endl;
    }

    void reserveRoom() {
		    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
		    cout << "|           OCCUPYING A ROOM          |" << endl;
		    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
		
		    displayRoomList();
		
		    int roomNumber = getValidRoomNumber();
			
			int day, month, year;
		    string dateString = getValidDateString(day, month, year);
		
		    Reservation reservation;
		    reservation.day = day;
			reservation.month = month;
			reservation.year = year;

		    reservation.roomNumber = roomNumber;
		    reservation.capacity = getRoomCapacity(roomNumber);
		    reservation.roomName = getRoomName(roomNumber);
		
		    int startHour, startMinute;
		    getTimeInput("Enter the start time (HH MM): ", startHour, startMinute);
		
		    int endHour, endMinute;
		    bool validEndTime = false;
		    while (!validEndTime) {
		        getTimeInput("Enter the end time (HH MM): ", endHour, endMinute);
		
		        if (endHour < startHour || (endHour == startHour && endMinute <= startMinute)) {
		            validEndTime = false;
		            cout << "Invalid end time. The end time must be later than the start time." << endl;
		            cout << "Please enter a valid end time: ";
		        } else {
		            validEndTime = true;
		        }
		    }
		
		    reservation.startTime = getTimestamp(day, month, year, startHour, startMinute);
		    reservation.endTime = getTimestamp(day, month, year, endHour, endMinute);
		
		    bool overlap = checkReservationOverlap(reservation);
		    if (overlap) {
		        cout << "The room is already reserved during the specified time. Request rejected." << endl;
		        cout << "Overlapping reservation reference number: " << reservation.referenceNumber << endl;
		        enterToGoBack();
		        return;
		    }
		
		    reservation.section = getReservationSection();
		    reservation.capacity = getValidNumberOfStudents(reservation.capacity);
		
		    reservation.profName = getTeacherName();
		
		    cout << endl;
		    cout << "======================================" << endl;
		    cout << "|  Room: " << reservation.roomName << endl;
		    cout << "|  Section: " << reservation.section << endl;
		    cout << "|  Start time: " << asctime(localtime(&reservation.startTime));
		    cout << "|  End time: " << asctime(localtime(&reservation.endTime));
		    cout << "|  Occupancy date: " << reservation.day << "/" << reservation.month << "/" << reservation.year << endl;
		    cout << "======================================" << endl;
		
		    char confirmation;
		    cout << "Are you sure you want to reserve this room? (y/n): ";
		    cin >> confirmation;
		    if (confirmation != 'y' && confirmation != 'Y') {
		        cout << "Reservation cancelled." << endl;
		        enterToGoBack();
		        return;
		    }
		    reservation.referenceNumber = nextReferenceNumber;
		    nextReferenceNumber++;
		
		    addReservationNode(reservation); // Store the reservation
		
		    cout << endl << endl;
		
		    cout << "Room " << getRoomName(roomNumber) << " reserved. Reference number: " << reservation.referenceNumber << endl;
		
		    saveReservation(reservation);
		
		    enterToGoBack();
		}

	
	void getTimeInput(const string& prompt, int& hour, int& minute) {
	    bool validTime = false;
	    while (!validTime) {
	        cout << prompt;
	        string timeString;
	        getline(cin, timeString);
	
	        istringstream timeStream(timeString);
	        if (timeStream >> hour) {
		        if (hour >= 0 && hour <= 23) {
		            if (timeStream >> minute) {

		                if (minute >= 0 && minute <= 59) {
		                    validTime = true;
		                } else {
		                    cout << "Invalid minute. Please enter a valid minute (0-59): ";
		                }
		            } else {
		                minute = 0;  // Set the minute to 00 by default
		                validTime = true;
		            }
		        } else {
		            cout << "Invalid hour. Please enter a valid hour (0-23): ";
		        }
		    } else {
		        cout << "Invalid time format. Please enter the start time in HH MM format (e.g., 10 00): ";
    }
	    }
	}
	int getValidRoomNumber() {
	    int roomNumber;
	    cout << "Enter the room ID you wish to occupy/reserve (1-15): ";
	    while (!(cin >> roomNumber) || roomNumber < 1 || roomNumber > 15) {
	        cin.clear();
	        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	        cout << "Please enter a valid room ID (1-15): ";
	    }
	    cin.ignore(numeric_limits<streamsize>::max(), '\n');
	    return roomNumber;
	}

	string getValidDateString(int& day, int& month, int& year) {
		    bool validDate = false;
		    string dateString;
		    while (!validDate) {
		        cout << "Enter the date of occupancy (DD MM YYYY): ";
		        getline(cin, dateString);
		        istringstream dateStream(dateString);
		        dateStream >> day >> month >> year;
		        validDate = isValidDate(day, month, year);
		        if (!validDate) {
		            cout << "Invalid date format or invalid date. ";
		        }
		    }
		    return dateString;
		}

	bool checkReservationOverlap(const Reservation& reservation) {
	    Reservation* temp = reservations;
	    while (temp != nullptr) {
	        if (reservation.roomNumber == temp->roomNumber &&
	            reservation.startTime < temp->endTime &&
	            reservation.endTime > temp->startTime) {
	            return true;
	        }
	        temp = temp->next;
	    }
	    return false;
	}
	
	string getReservationSection() {
	    string section;
	    cout << "Enter Section: ";
	    getline(cin, section);
	    return section;
	}
	
	int getValidNumberOfStudents(int roomCapacity) {
	    int numPeople;
	    cout << "Enter Number of Students: ";
	    while (!(cin >> numPeople) || numPeople > roomCapacity) {
	        cin.clear();
	        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	        cout << "Invalid number of students. Please enter a valid number: ";
	    }
	    cin.ignore(numeric_limits<streamsize>::max(), '\n');
	    return numPeople;
	}
	
	string getTeacherName() {
	    string teacherName;
	    cout << "Enter Teacher's Name: ";
	    getline(cin, teacherName);
	    return teacherName;
	}


    void deleteReservation()
		{
		    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
		    cout << "|          DELETING A RESERVATION         |" << endl;
		    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
		
		    int referenceNumber;
		    cout << "Enter the reference number you want to delete: ";
		    cin >> referenceNumber;
		
		    Reservation* prev = nullptr;
		    Reservation* current = reservations;
		    bool reservationDeleted = false; // Flag variable to track if reservation was found and deleted
		
		    while (current != nullptr)
		    {
		        if (current->referenceNumber == referenceNumber)
		        {
		            cout << endl;
		            cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
		            cout << "| Reservation Found: " << endl;
		            cout << "| Reference Number: " << current->referenceNumber << endl;
		            cout << "| Teacher: " << current->profName << endl;
		            cout << "| Room: " << current->roomName << endl;
		            cout << "| Section: " << current->section << endl;
		            cout << "| Start time: " << asctime(localtime(&current->startTime));
		            cout << "| End time: " << asctime(localtime(&current->endTime));
		            cout << "| Occupancy date: " << current->day << "/" << current->month << "/" << current->year << endl;
		            cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
		            cout << endl;
		
		            char confirmation;
		            cout << "Are you sure you want to delete this reservation? (y/n): ";
		            cin >> confirmation;
		            if (tolower(confirmation) == 'y')
		            {
		                if (prev == nullptr)
		                {
		                    reservations = current->next;
		                }
		                else
		                {
		                    prev->next = current->next;
		                }
		                delete current;
		                cout << "Reservation with reference number " << referenceNumber << " deleted." << endl;
		                updateReservationFile();
		                reservationDeleted = true; // Set flag to true
		            }
		            else
		            {
		                cout << "Deletion canceled." << endl;
		            }
		        }
		        prev = current;
		        current = current->next;
		    }
		
		    if (!reservationDeleted)
		    {
		        cout << endl;
		        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;
		        cout << "No reservation found with the given reference number." << endl;
		        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;
		    }
		
		    cout << "Enter to go back to Menu --->";
		    cin.ignore(numeric_limits<streamsize>::max(), '\n');
		    cin.get();
		}



    void saveReservation(const Reservation& reservation) 
	{
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
                 << reservation.profName << ","
                 << reservation.referenceNumber << "\n";
            file.close();
        } else {
            cout << "Failed to open reservations database file." << endl;
        }
    }

   
	void sortsHistory()
	{
	    while (true)
	    {
	        cout << "+---------------------------------------------------------------------------------------------+" << endl;
	        cout << "|                                        FILTER HISTORY                                       |" << endl;
	        cout << "+---------------------------------------------------------------------------------------------+" << endl;
	        cout << "       1. Default (by reference #)             |      4. Search Availability " << endl;
	        cout << "       2. Sort by Closest Time to Current Time |      5. Go Back to Main Menu" << endl;
	        cout << "       3. Show Active Reservations for a Room  |    " << endl;
	        cout << "+---------------------------------------------------------------------------------------------+" << endl;
	        cout << "Enter your choice (1-5): ";	
	        int choice;
	        cin >> choice;
	
	        switch (choice) {
	            case 1:
	                system("CLS");
	                checkReservations();
	                break;
	            case 2:
	            	system("CLS");
	                sortReservationsByClosestTime();
	                break;
	            case 3:
	                system("CLS");
	            {
	            	cout << "+-=-=-=-=-=-=-=-=-=-=--=-=+" << endl;
	            	cout << "| Show Active Reservation |" << endl;
	            	cout << "+-=-=-=-=-=-=-=-=-=-=--=-=+" << endl;
	                string roomName;
	                cout << "Enter the room name: ";
	                cin.ignore();
	                getline(cin, roomName);
	                showActiveReservationsForRoom(roomName);
	                break;
	            }
	            case 4:
	                system("CLS");
	      			searchRoomAvailability();
	      			sortsHistory();
	                break;
	            case 5:
	           		return; 
	            default:
	                cout << "Invalid choice!" << endl;
	            	enterToGoBack();
	                return;
	        }
	
	        cout << "Do you want to Filter History? (Y/N): ";
	        char option;
	        cin >> option;
	        if (option != 'Y' && option != 'y')
	        {
	            break; 
	        }
	    }
	}



	string formatTime(time_t time) 
	{
	    char buffer[9]; 
	    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&time));
	    return buffer;
	}	

	int partition(Reservation** arr, int low, int high) {
	    Reservation* pivot = arr[high];
	    int i = low - 1;
	
	    for (int j = low; j < high; j++) {
	        if (arr[j]->referenceNumber <= pivot->referenceNumber) {
	            i++;
	            swap(arr[i], arr[j]);
	        }
	    }
	
	    swap(arr[i + 1], arr[high]);
	    return i + 1;
	}

	void quickSort(Reservation** arr, int low, int high) {
	    if (low < high) {
	        int pivotIndex = partition(arr, low, high);
	
	        quickSort(arr, low, pivotIndex - 1);
	        quickSort(arr, pivotIndex + 1, high);
	    }
	}

void checkReservations() {
	system("CLS");
    cout << "+---------------------------+" << endl;
    cout << "|      CHECKING HISTORY     |" << endl;
	displayInfo();
    if (reservations == nullptr) {
        cout << endl;
        cout << " |  No reservations found  |" << endl;
    } else {
        int count = 0;
        Reservation* temp = reservations;
        while (temp != nullptr) {
            count++;
            temp = temp->next;
        }
        Reservation** reservationArray = new Reservation*[count];
        temp = reservations;
        int i = 0;
        while (temp != nullptr) {
            reservationArray[i] = temp;
            temp = temp->next;
            i++;
        }

        // Sort the reservations using quicksort
        quickSort(reservationArray, 0, count - 1);

        // Display the sorted reservations
        for (int i = 0; i < count; i++) {
            displayReservation(reservationArray[i]);
        }

        // Clean up memory
        delete[] reservationArray;
    }

    cout << "+---------------------------------------------------------------------------------------------+" << endl;
    cout << endl;
    sortsHistory(); // Call sortsHistory() once after displaying reservations
}



void searchRoomAvailability() {
    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
    cout << "|        SEARCH ROOM AVAILABILITY      |" << endl;
    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
	cin.ignore(numeric_limits<streamsize>::max(), '\n');
    int day, month, year;
    
    string dateString = getValidDateString(day, month, year);

    Reservation reservation;
    reservation.day = day;
    reservation.month = month;
    reservation.year = year;

    int startHour, startMinute;
    getTimeInput("Enter the start time (HH MM): ", startHour, startMinute);
    reservation.startTime = getTimestamp(day, month, year, startHour, startMinute);

    int endHour, endMinute;
    bool validEndTime = false;
    while (!validEndTime) {
        getTimeInput("Enter the end time (HH MM): ", endHour, endMinute);
        reservation.endTime = getTimestamp(day, month, year, endHour, endMinute);

        if (endHour < startHour || (endHour == startHour && endMinute <= startMinute)) {
            validEndTime = false;
            cout << "Invalid end time. The end time must be later than the start time." << endl;
            cout << "Please enter a valid end time: ";
        } else {
            validEndTime = true;
        }
    }

    cout << "Searching for available rooms from " << formatTime(reservation.startTime);
    cout << " to " << formatTime(reservation.endTime) << endl;
    cout << endl;

    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;
    cout << "   Room       |     Status        |     Reference #" << endl;
    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;

    for (int roomNumber = 1; roomNumber <= 15; roomNumber++) {
        reservation.roomNumber = roomNumber;

        Reservation* temp = reservations;
        bool occupied = false;

        while (temp != nullptr) {
            if (temp->roomNumber == roomNumber && temp->startTime < reservation.endTime && temp->endTime > reservation.startTime) {
                cout << " Room  " << left << setw(10) << getRoomName(roomNumber) << setw(14) << "   Occupied               " << temp->referenceNumber << endl;
                occupied = true;
                break;
            }
            temp = temp->next;
        }

        if (!occupied) {
            cout <<" Room  " << left << setw(10) << getRoomName(roomNumber) << setw(14) <<  "   Available             "  << endl;
        }
    }

    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;
}


	void displayReservation(const Reservation* temp) 
	{
	    cout << "| ";
	    cout << setw(7) << left << temp->referenceNumber;
	    cout << "| ";
	    cout << setw(14) << left << temp->profName;
	    cout << "| ";
	    cout << setw(11) << left << temp->roomName;
	    cout << "| ";
	    cout << setw(10) << left << temp->section;
	    cout << "| ";
	    cout << setw(12) << left << formatTime(temp->startTime);
	    cout << "| ";
	    cout << setw(12) << left << formatTime(temp->endTime);
	    cout << "| ";
	    cout << temp->day << "/" << temp->month << "/" << temp->year;
	    cout << "     |" << endl;
	}
	
	void sortReservationsByClosestTime() 
{
    if (reservations == nullptr) {
        cout << "No reservations found." << endl;
        return;
    }

    // Count the number of reservations
    int count = 0;
    Reservation* temp = reservations;
    while (temp != nullptr) {
        count++;
        temp = temp->next;
    }

    // Perform bubble sort
    bool sorted = false;
    for (int i = 0; i < count - 1 && !sorted; i++) {
        sorted = true;
        temp = reservations;
        Reservation* prev = nullptr;
        Reservation* nextNode = temp->next;
        for (int j = 0; j < count - 1 - i; j++) {
            if (temp->startTime > nextNode->startTime) {
                sorted = false;
                // Swap the reservations
                if (prev == nullptr) {
                    reservations = nextNode;
                } else {
                    prev->next = nextNode;
                }
                temp->next = nextNode->next;
                nextNode->next = temp;
                // Update the previous node and next node references
                prev = nextNode;
                nextNode = temp->next;
            } else {
                prev = temp;
                temp = nextNode;
                nextNode = nextNode->next;
            }
        }
    }
    cout << "+--------------------------+" << endl;
    cout << "|    ACTIVE RESERVATIONS   |" << endl;
    cout << "+--------------------------+" << endl;
    bool found = false;
	    temp = reservations;
		displayInfo();
	    while (temp != nullptr) {
	        if (temp->endTime > time(0)) {
	            found = true;
	            displayReservation(temp);
	        }
	
	        temp = temp->next;
	    }
	
	    if (!found) {
	        cout << "   No active reservations found" << endl;
	    }
	    cout << "+---------------------------------------------------------------------------------------------+" << endl;
    cout << "+---------------------------+" << endl;
    cout << "|    EXPIRED RESERVATIONS   |" << endl;
    cout << "+---------------------------+" << endl;
    displayInfo();
    bool foundExpired = false;
    temp = reservations;
    while (temp != nullptr) {
        if (temp->endTime <= time(0)) {
            foundExpired = true;
            displayReservation(temp);
        }
        temp = temp->next;
    }
	
    if (!foundExpired) {
        cout << "No expired reservations found" << endl;
    }
    cout << "+---------------------------------------------------------------------------------------------+" << endl;
}
    void displayInfo()
    {
    	cout << "+---------------------------------------------------------------------------------------------+" << endl;
    	cout << setw(9) << left << "| Ref Num";
        cout << setw(16) << left << "| Teacher";
        cout << setw(13) << left << "| Room";
        cout << setw(12) << left << "| Section";
        cout << setw(14) << left << "| Start time";
        cout << setw(14) << left << "| End time";
        cout << setw(14) << left << "| Occupancy date" << "|" << endl;
        cout << "+---------------------------------------------------------------------------------------------+" << endl;
	}
	void showActiveReservationsForRoom(const string& roomName) 
	{
	    cout << "+---------------------------------------+" << endl;
	    cout << "|  ACTIVE RESERVATIONS FOR ROOM: " << roomName << endl;
	
	    bool found = false;
	    Reservation* temp = reservations;
		displayInfo();
	    while (temp != nullptr) {
	        if (temp->roomName == roomName && temp->endTime > time(0)) {
	            found = true;
	            displayReservation(temp);
	        }
	
	        temp = temp->next;
	    }
	    if (!found) {
	        cout << "   No active reservations found for room: " << roomName << endl;
	    }
		cout << "+---------------------------------------------------------------------------------------------+" << endl;
	    cout << endl;
	}

    void checkRoomStatus() {
	    cout << "+--------------------------+" << endl;
	    cout << "|   CHECKING ROOM STATUS   |" << endl;
	    cout << "+--------------------------+" << endl;
	
	    time_t currentTime = time(0);
	    tm* currentTime_tm = localtime(&currentTime);
	
	    cout << "+============================================================================================================+" << endl;
	    cout << "|    ROOM     |    STATUS   |    PROFESSOR    |   SECTION   |   END TIME    |   # OF RESERVATION(S) TODAY    |" << endl;
	    cout << "+============================================================================================================+" << endl;
	
	    Room* temp = rooms;
	    while (temp != nullptr) {
	        bool roomOccupied = false;
	        Reservation* currentReservation = nullptr;
	        Reservation* tempReservation = reservations;
	        while (tempReservation != nullptr) {
	            if (temp->roomNumber == tempReservation->roomNumber &&
	                currentTime >= tempReservation->startTime &&
	                currentTime < tempReservation->endTime) {
	                roomOccupied = true;
	                currentReservation = tempReservation;
	                break;
	            }
	            tempReservation = tempReservation->next;
	        }
	        int reservationCount = 0;
	        tempReservation = reservations;
	        while (tempReservation != nullptr) {
	            if (temp->roomNumber == tempReservation->roomNumber &&
	                tempReservation->day == currentTime_tm->tm_mday &&
	                tempReservation->month == (currentTime_tm->tm_mon + 1) &&
	                tempReservation->year == (currentTime_tm->tm_year + 1900)) {
	                reservationCount++;
	            }
	            tempReservation = tempReservation->next;
	        }
	        if (roomOccupied)
			{
	            cout << "| " << "Room " << left << setw(7) << temp->roomName << "|     OCCUPIED| " << right << setw(16) << currentReservation->profName << "| " << right << setw(12)
	                << currentReservation->section << "|   " << formatTime(currentReservation->endTime)<< "    |      " << reservationCount << endl;
	        } 
			else {
	            cout << "| " << "Room " << left << setw(7) << temp->roomName << "|       VACANT|                 |             |               |      "  << reservationCount << endl;
	        } 
	
	        temp = temp->next;
	    }
	    cout << "+============================================================================================================+" << endl;
	    cout << endl;
	    enterToGoBack();
	}

    void enterToGoBack() {
        cout << "Press Enter to go back to Menu --->";
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
        cin.get();
    }
};


int main() {
    while (true) {
    	ReservationSystem reservationSystem;
   		 int choice;
        system("CLS");
        time_t currentTime = time(0);
        tm* currentTime_tm = localtime(&currentTime);
        cout << "Time: " << asctime(currentTime_tm);
        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "|           MENU             |" << endl;
        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "| [1] Occupy a Room          |" << endl;
        cout << "| [2] Check Room Status      |" << endl;
        cout << "| [3] Check History          |" << endl;
        cout << "| [4] Delete a Reservation   |" << endl;
        cout << "| [5] Clear Screen           |" << endl;
        cout << "| [6] Exit                   |" << endl;
        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "Choice: ";
        while (!(cin >> choice) || choice < 1 || choice > 7) {
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            cout << "Invalid choice. Please enter a valid choice (1-5): ";
        }
        switch (choice) {
            case 1:
                reservationSystem.reserveRoom();
                break;
            case 2:
                reservationSystem.checkRoomStatus();
                break;
            case 3:
                reservationSystem.checkReservations();
                break;
            case 4:
                reservationSystem.deleteReservation();
                break;
            case 5: 
            	system("CLS");
            	break;
            case 6:
                cout << endl;
					cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
					cout << "|          Good Bye          |" << endl;
					cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
                return 0;
    }
}}
