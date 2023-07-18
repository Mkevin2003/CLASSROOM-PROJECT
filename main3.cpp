#include <iostream> // ---- DEFAULT
#include <sstream> // ----- MANIPULATE STRINGS 
#include <string> // ----- FOR STRINGS
#include <limits> // ----- FOR ERROR HANDLING. 
#include <iomanip> // ----- AESTHETIC
#include <fstream> // ----- DATA BASE RELATED 
#include <stack> // ---- TO USE STACK FUNCTIONS SUCH AS POP 
#include <windows.h> // ----- AESTHETIC 


HANDLE color = GetStdHandle(STD_OUTPUT_HANDLE); // ------ AESTHETIC ---- ADDING COLORS 
using namespace std;

class ReservationSystem {
private:
	// ----------------------------------------------------------------------------------------------------------------------- DECLARING VARIABLES-------------
    struct Room {
        int roomNumber;
        int capacity;
        string roomName;
        Room* next;
        
        Room(int roomNumber, int capacity, const string& roomName)
            :  roomNumber(roomNumber), capacity(capacity), roomName(roomName), next(nullptr) {}
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
	
    ReservationSystem() //------------------------------------------------------------------------------------------------ INITIALIZER
	{
	    nextReferenceNumber = 1;
	    rooms = nullptr;
	    reservations = nullptr;
	    roomsHead = nullptr; 
	
	    createRooms();
	    reservationDataFile = "reservations.csv";
	    loadReservationsFromFile();
    }

    void createRooms() { // ------------------------------------------------------------------------------------------- CREATES A LINKED LIST WITH DEFAULT ARRAYS FOR ROOMS ---
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

    void addReservationNode(const Reservation& reservation) // --------------------------------------------------------- LINKING RESERVATION TO THE LINKED LIST ---- TAIL ---
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

    void updateReservationFile() // -------------------------------------------------------------------------------------- UPDATING CSV DATABASE ----
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
    
	void loadReservationsFromFile() { // -------------------------------------------------------------------------------- LOADING THE CSV DATABASE -- TRAVERSES THEN GETS THE TOP REFERENCE #----
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

    time_t getTimestamp(int day, int month, int year, int hour, int minute) // ------------------------------------------------- TIME RELATED FUNCTION -- CONVERTS INPUT TO LEGIT TIME I THINK --
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
    
    void displayInfo()  // --------------------------------------------------------------------------------------------------- DISPLAY COLUMNS (AESTHETIC)
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
    
    void displayRoomList() { //------------------------------------------------------------------------------------- DISPLAY THE ROOM INDEX, NAME AND CAP
    	SetConsoleTextAttribute(color, 11);
        cout << "==============================================================" << endl;
        cout << "|                       CICS BUILDING                        |" << endl;
        cout << "==============================================================" << endl;
        SetConsoleTextAttribute(color, 14);
        cout << "\t   +----+-------+-----+----+-------+-----+" << endl;
        cout << "\t   | ID |  Room | CAP | ID |  Room | CAP |" << endl;
        cout << "\t   +----+-------+-----+----+-------+-----+" << endl;

        Room* temp = rooms;
        while (temp != nullptr)
		{
            cout << "\t   " << "|  " << setw(2) << temp->roomNumber << "|" << setw(5) << temp->roomName << "  | " << setw(3) << temp->capacity << " | ";
            temp = temp->next;
            
            if (temp != nullptr) 
			{
                cout << " " << setw(2) << temp->roomNumber << "| " << setw(5) << temp->roomName << " | " << setw(3) << temp->capacity << " | ";
                temp = temp->next;
            }
            
            cout << endl;
        }
        SetConsoleTextAttribute(color, 11);
        cout << "==============================================================" << endl;
        SetConsoleTextAttribute(color, 14);
    }
    
    bool isValidDate(int day, int month, int year) // ------------------------------------------------------------------ ERROR HANDLING FOR DATE 
	{
        if (year < 2023|| year > 9999)
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
    
	string getValidDateString(int& day, int& month, int& year) { // ------------------------------------------------------- SEPERATING INPUT DATE ---- 
		    bool validDate = false;
		    string dateString;
		    while (!validDate) {
		        cout << "Enter the date of occupancy (DD MM YYYY): ";
		        getline(cin, dateString);
		        istringstream dateStream(dateString);
		        dateStream >> day >> month >> year;
		        validDate = isValidDate(day, month, year);
		        if (!validDate) {
		        	SetConsoleTextAttribute(color, 12);
		            cout << "Invalid date.";
		            SetConsoleTextAttribute(color, 11);
		        }
		    }
		    return dateString;
	}
	
	void getTimeInput(const string& prompt, int& hour, int& minute) // ------------------------------------------------- SEPERATING STRING AND ERROR HANDLING FOR INPUTTING TIME --------
	{
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
		                	SetConsoleTextAttribute(color, 12);
		                    cout << "Invalid minute. Please enter a valid minute (0-59): ";
		                    SetConsoleTextAttribute(color, 11);
		                }
		            } else {
		                minute = 0;  // Set the minute to 00 by default
		                validTime = true;
		            }
		        } else {
		        	SetConsoleTextAttribute(color, 12);
		            cout << "Invalid hour. Please enter a valid hour (0-23): ";
		            SetConsoleTextAttribute(color, 11);
		        }
		    } else {
		    	SetConsoleTextAttribute(color, 12);
		        cout << "Invalid time format. Input (HOUR MINUTE) | ";
		        SetConsoleTextAttribute(color, 11);
    		}
	    }
	}
	
	int getValidRoomNumber() { // ---------------------------------------------------------------------------------------- ERROR HANDLING FOR ROOM INDEX -----
	    int roomNumber;
	    cout << "Enter the room ID you wish to occupy/reserve (1-15): ";
	    while (!(cin >> roomNumber) || roomNumber < 1 || roomNumber > 15) {
	        cin.clear();
	        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	        SetConsoleTextAttribute(color, 12);
	        cout << "Please enter a valid room ID (1-15): ";
	        SetConsoleTextAttribute(color, 11);
	    }
	    cin.ignore(numeric_limits<streamsize>::max(), '\n');
	    return roomNumber;
	}
	
	
	int getRoomCapacity(int roomNumber) {  // --------------------------------------------------------------------- GET ROOMCAPACITY USING ROOM NUMBER ----
        Room* temp = rooms;
        while (temp != nullptr) {
            if (roomNumber == temp->roomNumber) {
                return temp->capacity;
            }
            temp = temp->next;
        }
        return 0;
    }

    string getRoomName(int roomNumber) {  // ---------------------------------------------------------------------- GET ROOMNAME USING ROOM NUMBER ----------
        Room* temp = rooms;
        while (temp != nullptr) {
            if (roomNumber == temp->roomNumber) {
                return temp->roomName;
            }
            temp = temp->next;
        }
        return "N/A"; 
    }
    
    bool checkReservationOverlap(const Reservation& reservation) {  // ------------------------------------------------ CHECKING FOR OVERLAP RESERVATIONS ---------
	    Reservation* temp = reservations;
	    while (temp != nullptr) {
	        if (reservation.roomNumber == temp->roomNumber &&
	            reservation.startTime < temp->endTime &&
	            reservation.endTime > temp->startTime) {
	            cout << endl;
	            SetConsoleTextAttribute(color, 12);
	            cout << "Overlapping reservation reference number: " << temp->referenceNumber << endl;
	            SetConsoleTextAttribute(color, 11);
	            return true;
	        }
	        temp = temp->next;
	    }
	    return false;
	}

	int getValidNumberOfStudents(int roomCapacity) { // -------------------------------------------------------------- ERROR HANDLING FOR CAPACITY -----
	    int numPeople;
	    cout << "Enter Number of Students: ";
	    while (!(cin >> numPeople) || numPeople > roomCapacity) {
	        cin.clear();
	        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	        SetConsoleTextAttribute(color, 12);
	        cout << "Check the CAP." ;
	        SetConsoleTextAttribute(color, 11);
			cout << "Please enter a valid number: ";
	    }
	    cin.ignore(numeric_limits<streamsize>::max(), '\n');
	    return numPeople;
	}
	
    void reserveRoom() { // ------------------------------------------------------------------------------------------------------------- OCCUPYING THE ROOM --- HEART ----
    		cout << "  ____   _____ _____ _    _ _______     _______ _   _  _____ " << endl;
    		cout << " / __ \\ / ____/ ____| |  | |  __ \\ \\   / /_   _| \\ | |/ ____|" << endl;
    		cout << "| |  | | |   | |    | |  | | |__) \\ \\_/ /  | | |  \\| | |  __ " << endl;
    		cout << "| |  | | |   | |    | |  | |  ___/ \\   /   | | | . ` | | |_ |" << endl;
    		cout << "| |__| | |___| |____| |__| | |      | |   _| |_| |\\  | |__| |" << endl;
    		cout << " \\____/ \\_____\\_____|\\____/|_|      |_|  |_____|_| \\_|\\_____|" << endl;
		    cout << "==============================================================" << endl;

		
		    displayRoomList();
		    
		    cout << endl;
			SetConsoleTextAttribute(color, 11);
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
		            SetConsoleTextAttribute(color, 12);
		            cout << "Invalid end time. The end time must be later than the start time." << endl;
		            SetConsoleTextAttribute(color, 11);
		            cout << "Please enter a valid end time: ";
		        } else {
		            validEndTime = true;
		        }
		    }
		
		    reservation.startTime = getTimestamp(day, month, year, startHour, startMinute);
		    reservation.endTime = getTimestamp(day, month, year, endHour, endMinute);
		
		    bool overlap = checkReservationOverlap(reservation);
		    if (overlap) {
		    	SetConsoleTextAttribute(color, 12);
		        cout << "The room is already reserved during the specified time. Request rejected." << endl;
		        cout << endl;
		        SetConsoleTextAttribute(color, 14);
		        enterToGoBack();
		        return;
		    }
			SetConsoleTextAttribute(color, 11);
		    string teacherName;
		    cout << "Enter Professor's Name: ";
		    getline(cin, teacherName);
		    reservation.profName = teacherName;
			string section;
			
		    cout << "Enter Section: ";
		    getline(cin, section);
		    reservation.section = section;
		    
		    reservation.capacity = getValidNumberOfStudents(reservation.capacity);

		    cout << endl;
		    SetConsoleTextAttribute(color, 10);
		    cout << "======================================" << endl;
		    cout << "|  Room: " << reservation.roomName << endl;
		    cout << "|  Professor: " << reservation.profName << endl;
		    cout << "|  Section: " << reservation.section << endl;
		    cout << "|  Start time: " << asctime(localtime(&reservation.startTime));
		    cout << "|  End time: " << asctime(localtime(&reservation.endTime));
		    cout << "|  Occupancy date: " << reservation.day << "/" << reservation.month << "/" << reservation.year << endl;
		    cout << "======================================" << endl;
			cout << endl;
		    char confirmation;
		    SetConsoleTextAttribute(color, 14);
		    cout << "Are you sure you want to reserve this room? (y/n): ";
		    cin >> confirmation;
		    if (confirmation != 'y' && confirmation != 'Y') {
		    	SetConsoleTextAttribute(color, 12);
		        cout << "   _____          _   _  _____ ______ _      _      ______ _____  " << endl;
			    cout << "  / ____|   /\\   | \\ | |/ ____|  ____| |    | |    |  ____|  __ \\ " << endl;
			    cout << " | |       /  \\  |  \\| | |    | |__  | |    | |    | |__  | |  | |" << endl;
			    cout << " | |      / /\\ \\ | . ` | |    |  __| | |    | |    |  __| | |  | | " << endl;
			    cout << " | |____ / ____ \\| |\\  | |____| |____| |____| |____| |____| |__| |" << endl;
			    cout << "  \\_____/_/    \\_\\_| \\_|\\_____|______|______|______|______|_____/ " << endl << endl;
		        SetConsoleTextAttribute(color, 14);
		        enterToGoBack();
		        cin.ignore(numeric_limits<streamsize>::max(), '\n');
		        return;
		    }
		    
		    reservation.referenceNumber = nextReferenceNumber;
		    nextReferenceNumber++;
		
		    addReservationNode(reservation); // Store the reservation
		
		    cout << endl;
			SetConsoleTextAttribute(color, 10);
			cout << " _____  ______  _____ ______ _______      ________ _____  " << endl;
		    cout << "|  __ \\|  ____|/ ____|  ____|  __ \\ \\    / /  ____|  __ \\  " << endl;
		    cout << "| |__) | |__  | (___ | |__  | |__) \\ \\  / /| |__  | |  | |" << endl;
		    cout << "|  _  /|  __|  \\___ \\|  __| |  _  / \\ \\/ / |  __| | |  | |" << endl;
		    cout << "| | \\ \\| |____ ____) | |____| | \\ \\  \\  /  | |____| |__| |" << endl;
		    cout << "|_|  \\_\\______|_____/|______|_|  \\_\\  \\/   |______|_____/  " << endl << endl;
		    cout << "Room " << getRoomName(roomNumber) << " reserved. Reference number: " << reservation.referenceNumber;
		    cout << endl;
			SetConsoleTextAttribute(color, 14);
		    saveReservation(reservation);
		    cin.ignore(numeric_limits<streamsize>::max(), '\n');
		    cout << endl;
		    enterToGoBack();

		}

    

    void saveReservation(const Reservation& reservation)  // -------------------------------------------------------------- SAVE RESERVATION ON CSV DATABASE ---------
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

	void sortsHistory()  // ------------------------------------------------------------------------------------------------------------- MENU SORT HISTORY -------------------
	{
		cin.ignore();
	    while (true)
	    {
	    	SetConsoleTextAttribute(color, 14);
	        cout << "+---------------------------------------------------------------------------------------------+" << endl;
	        cout << "|                                        FILTER HISTORY                                       |" << endl;
	        cout << "+---------------------------------------------------------------------------------------------+" << endl;
	        cout << "       1. Default (by reference #)             |      4. Go Back to Main Menu " << endl;
	        cout << "       2. Sort by Closest Time to Current Time |   " << endl;
	        cout << "       3. Show Active Reservations for a Room  |    " << endl;
	        cout << "+---------------------------------------------------------------------------------------------+" << endl;
	        cout << "Enter your choice (1-4): ";
	        int choice;
	        if (!(cin >> choice))
	        {
	            cout << "Invalid input! Please enter a valid choice (1-4)." << endl;
	            cin.clear();  // Clear error state flags
	            cin.ignore(numeric_limits<streamsize>::max(), '\n');
	            continue; // Continue to the next iteration of the loop
	        }
	        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	
	        switch (choice)
	        {
	            case 1:
	                historyArt();
	                checkReservations();
	                break;
	            case 2:
	                historyArt();
	                sortReservationsByClosestTime();
	                break;
	            case 3:
	                {
	                	historyArt();
	                    cout << "+-=-=-=-=-=-=-=-=-=-=--=-=+" << endl;
	                    cout << "| Show Active Reservation |" << endl;
	                    cout << "+-=-=-=-=-=-=-=-=-=-=--=-=+" << endl;
	                    string roomName;
	                    cout << "Enter the room name: ";
	                    cin.clear();  // Clear error state flags
	                    getline(cin, roomName);
	                    showActiveReservationsForRoom(roomName);
	                }
	                break;
	            case 4:
	               return;
	            default:
	                cout << "Invalid choice!" << endl;
	                break;
	        }
	    }
	}

	string formatTime(time_t time) // -------------------------------------------------------------------------------------------------- FORMAT TIME (AESTHETIC) -----
	{
	    char buffer[9]; 
	    strftime(buffer, sizeof(buffer), "%H:%M:%S", localtime(&time));
	    return buffer;
	}	

	void checkReservations() // -------------------------------------------------------------------------------------------- CHECK RESERVATIONS (REFERENCE NUMBER)  ---------
	{
	    
	    displayInfo();
	    if (reservations == nullptr) {
	        cout << endl;
	        cout << " |  No reservations found  |" << endl;
	    } else {
	        Reservation* temp = reservations;
	        while (temp != nullptr) {
	            displayReservation(temp);
	            temp = temp->next;
	        }
    }
	    cout << "+---------------------------------------------------------------------------------------------+" << endl;
	    cout << endl;
	}
	
	void searchRoomAvailability()
{
    system("CLS");
    searchArt();
    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
    cout << "|       SEARCH ROOM AVAILABILITY      |" << endl;
    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;

    // Prompt for date and time
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

    system("CLS");
    SetConsoleTextAttribute(color, 14);
    searchArt();
    cout << "+----------------------------------------------------+" << endl;
    cout << "|   SEARCH AVAILABLE ROOMS FROM: " << formatTime(reservation.startTime) << " TO " << formatTime(reservation.endTime) << endl;
    cout << "+----------------------------------------------------+" << endl;
    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;
    cout << "   Room       |     Status        |     Reference #" << endl;
    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;

    for (int roomNumber = 1; roomNumber <= 15; roomNumber++) {
        reservation.roomNumber = roomNumber;
        Reservation* temp = reservations;
        bool occupied = false;

        while (temp != nullptr) {
            if (temp->roomNumber == roomNumber && temp->startTime < reservation.endTime && temp->endTime > reservation.startTime) {
                SetConsoleTextAttribute(color, 6);
                cout << " Room  " << left << setw(10) << getRoomName(roomNumber) << setw(14) << "   Occupied               " << temp->referenceNumber << endl;
	                occupied = true;
            }
            temp = temp->next;
        }

        if (!occupied) {
            SetConsoleTextAttribute(color, 14);
            cout <<" Room  " << left << setw(10) << getRoomName(roomNumber) << setw(14) <<  "   Available             "  << endl;
        }
    }

    char occupy;
    cout << "Occupy a room with the same date and Time? (Y/N): ";
    cin >> occupy;

    if (tolower(occupy) == 'y') {
    	displayRoomList();
        string teacherName, section, occupancy;
		Reservation newReservation;
		newReservation.day = day;
        newReservation.month = month;
        newReservation.year = year;
        newReservation.startTime = reservation.startTime;
        newReservation.endTime = reservation.endTime;
        int roomNumber = getValidRoomNumber();
		newReservation.roomNumber = roomNumber;
		bool overlap = checkReservationOverlap(newReservation);
		    if (overlap) {
		    	SetConsoleTextAttribute(color, 12);
		        cout << "The room is already reserved during the specified time. Request rejected." << endl;
		        cout << endl;
		        SetConsoleTextAttribute(color, 14);
		        enterToGoBack();
		        return;
		    }
        cout << "Enter the teacher's name: ";
        getline(cin, teacherName);

        cout << "Enter the section: ";
        getline(cin, section);

        // Create a new reservation
        
        newReservation.roomNumber = roomNumber;
        newReservation.roomName = getRoomName(roomNumber);
        newReservation.capacity = getRoomCapacity(roomNumber);
        newReservation.capacity = getValidNumberOfStudents(newReservation.capacity);
        newReservation.section = section;
        newReservation.profName = teacherName;
        newReservation.referenceNumber = getNextReferenceNumber();

        // Add the reservation to the linked list
        addReservationNode(newReservation);

        // Save the reservation to the file
        saveReservation(newReservation);

        cout << "Room " << getRoomName(roomNumber) << " has been reserved with reference number " << newReservation.referenceNumber << endl;
    }

    SetConsoleTextAttribute(color, 14);
    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;
    enterToGoBack();
}

	int getNextReferenceNumber()
{
    return nextReferenceNumber++;
}
	void displayReservation(const Reservation* temp) //--------------------------------------------------------------------------------- DISPLAYING INFO TEMP ----------------------
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
	    cout << temp->day << "/" << temp->month << "/" << setw(12) << left << temp->year ;
	    cout << endl;

	}
	
	void sortReservationsByClosestTime() // -------------------------------------------------------------------------------------- SORT BY CLOSEST TIME (BUBBLE SORT) --------
		{
		    if (reservations == nullptr) 
			{
		        cout << "No reservations found." << endl;
		        return;
	    	}
	    Reservation* tempReservations = nullptr;
	    Reservation* temp = reservations;
	    Reservation* prev = nullptr;
	
	    while (temp != nullptr) {
	        Reservation* newReservation = new Reservation(*temp);
	
	        if (prev == nullptr) {
	            tempReservations = newReservation;
	        } else {
	            prev->next = newReservation;
	        }
	
	        prev = newReservation;
	        temp = temp->next;
	    }
	
	    bool sorted = false;
	    while (!sorted) {
	        sorted = true;
	        temp = tempReservations;
	        prev = nullptr;
	
	        while (temp != nullptr && temp->next != nullptr) {
	            if (temp->startTime > temp->next->startTime) {
	                sorted = false;
	                Reservation* nextNode = temp->next;
	                Reservation* nextNextNode = nextNode->next;
	
	                if (prev == nullptr) {
	                    tempReservations = nextNode;
	                } else {
	                    prev->next = nextNode;
	                }
	
	                nextNode->next = temp;
	                temp->next = nextNextNode;
	                prev = nextNode;
	            } else {
	                prev = temp;
	                temp = temp->next;
	            }
	        }
	    }
	    
		SetConsoleTextAttribute(color, 10);
	    cout << "+--------------------------+" << endl;
	    cout << "|    ACTIVE RESERVATIONS   |" << endl;
	    cout << "+--------------------------+" << endl;
	    bool foundActive = false;
	    temp = tempReservations;
	    displayInfo();
	
	    while (temp != nullptr) {
	        if (temp->endTime > time(0)) {
	            foundActive = true;
	            displayReservation(temp);
	        }
	        temp = temp->next;
	    }
	
	    if (!foundActive) {
	        cout << "   No active reservations found" << endl;
	    }
	
	    cout << "+---------------------------------------------------------------------------------------------+" << endl;
	    SetConsoleTextAttribute(color, 12);
	    cout << "+---------------------------+" << endl;
	    cout << "|    EXPIRED RESERVATIONS   |" << endl;
	    cout << "+---------------------------+" << endl;
	    bool foundExpired = false;
	    temp = tempReservations;
	    displayInfo();
	
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
	
	    // Clean up the temporary linked list
	    temp = tempReservations;
	    while (temp != nullptr) {
	        Reservation* nextNode = temp->next;
	        delete temp;
	        temp = nextNode;
	    }
	}
	
	void showActiveReservationsForRoom(const string& roomName) // ------------------------------------------------------------------------ SHOW ACTIVE RESERVATION FOR ROOM -----------
	{
		system("CLS");
		historyArt();
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

    void checkRoomStatus() { //---------------------------------------------------------------------------------------------------- CHECK ROOM STATUS ---best part-------------
	    cout << "  _____   ____   ____  __  __    _____ _______    _______ _    _  _____ " << endl;
	    cout << " |  __ \\ / __ \\ / __ \\|  \\/  |  / ____|__   __|/\\|__   __| |  | |/ ____|" << endl;
	    cout << " | |__) | |  | | |  | | \\  / | | (___    | |  /  \\  | |  | |  | | (___  " << endl;
	    cout << " |  _  /| |  | | |  | | |\\/| |  \\___ \\   | | / /\\ \\ | |  | |  | |\\___ \\ " << endl;
	    cout << " | | \\ \\| |__| | |__| | |  | |  ____) |  | |/ ____ \\| |  | |__| |____) |" << endl;
	    cout << " |_|  \\_\\\\____/ \\____/|_|  |_| |_____/   |_/_/    \\_\\_|   \\____/|_____/ " << endl;
	
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
	            cout << "| " << "Room " << left << setw(7) << temp->roomName << "| ";
	            SetConsoleTextAttribute(color, 12);
				cout << "    OCCUPIED";
				SetConsoleTextAttribute(color, 14);
				cout << "|  "<< right << setw(15);
				cout << currentReservation->profName << "|" << right << setw(13)
	                << currentReservation->section << "|   " << formatTime(currentReservation->endTime)<< "    |      " << reservationCount << endl;
	        } 
			else {
	            cout << "| " << "Room " << left << setw(7) << temp->roomName << "|       VACANT|                 |             |               |      "  << reservationCount << endl;
	        } 
	
	        temp = temp->next;
	    }
	    cout << "+============================================================================================================+" << endl;
	    cout << endl;
	    cin.clear();
        cin.ignore(numeric_limits<streamsize>::max(), '\n');
	    enterToGoBack();
	}

    void enterToGoBack() {  //------------------------------------------------------------------------------------------------------------- ENTER TO GO BACK -------------------
    	cin.clear();
        cout << "Press Enter to go back to Menu --->";
        cin.get();
    }
 	void deleteReservation() // ----------------------------------------------------------------------------------------------- DELETE A SPECIFIC RESERVATION --------------
		{
			deleteArt();
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
		            SetConsoleTextAttribute(color, 12);
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
		                cout << endl;
		                SetConsoleTextAttribute(color, 10);
		                cout << "Reservation with reference number " << referenceNumber << " deleted." << endl;
		                updateReservationFile();
		                reservationDeleted = true; 
		            }
		            else
		            {
		            	cout << endl;
		            	SetConsoleTextAttribute(color, 14);
		                cout << "Deletion canceled." << endl << endl;
		                SetConsoleTextAttribute(color, 12);
		                reservationDeleted = true;
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
		
	void deleteLastReservation() //------------------------------------------------------------------------------------------------------ DELETE LAST RESERVATION -------------
		{
			deleteArt();
		    if (reservations == nullptr)
		    {
		        cout << "No reservations found." << endl;
		        return;
		    }
		
		    stack<Reservation*> reservationStack;
		    Reservation* current = reservations;
		
		    while (current != nullptr)
		    {
		        reservationStack.push(current);
		        current = current->next;
		    }
		
		    if (reservationStack.empty())
		    {
		        cout << "No reservations found." << endl;
		        return;
		    }
		
		    Reservation* lastReservation = reservationStack.top();
		    reservationStack.pop();
		
		    while (!reservationStack.empty())
		    {
		        current = reservationStack.top();
		        reservationStack.pop();
		    }
		
		    cout << endl;
		    SetConsoleTextAttribute(color, 12);
		    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
		    cout << "| MOST RECENT RESERVATION: " << endl;
		    cout << "| Reference Number: " << lastReservation->referenceNumber << endl;
		    cout << "| Teacher: " << lastReservation->profName << endl;
		    cout << "| Room: " << lastReservation->roomName << endl;
		    cout << "| Section: " << lastReservation->section << endl;
		    cout << "| Start time: " << asctime(localtime(&lastReservation->startTime));
		    cout << "| End time: " << asctime(localtime(&lastReservation->endTime));
		    cout << "| Occupancy date: " << lastReservation->day << "/" << lastReservation->month << "/" << lastReservation->year << endl;
		    cout << "+-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
		    cout << endl;
		    SetConsoleTextAttribute(color, 14);
		
		    char confirmation;
		    bool validConfirmation = false;
		    while (!validConfirmation)
		    {
		        cout << "Are you sure you want to delete this reservation? (y/n): ";
		        cin >> confirmation;
		        cin.ignore(numeric_limits<streamsize>::max(), '\n');
		
		        if (tolower(confirmation) == 'y' || tolower(confirmation) == 'n')
		        {
		            validConfirmation = true;
		        }
		        else
		        {
		        	SetConsoleTextAttribute(color, 12);
		            cout << "Invalid choice! Please enter 'y' for yes or 'n' for no." << endl;
		            SetConsoleTextAttribute(color, 14);
		        }
		    }
		
		    if (tolower(confirmation) == 'y')
		    {
		        delete lastReservation;
		        SetConsoleTextAttribute(color, 12);
		        cout << endl << "Last reservation deleted." << endl << endl;
		        SetConsoleTextAttribute(color, 14);
		        enterToGoBack();
		
		        Reservation* prev = nullptr;
		        current = reservations;
		
		        if (current == lastReservation)
		        {
		            reservations = current->next;
		        }
		        else
		        {
		            while (current != lastReservation)
		            {
		                prev = current;
		                current = current->next;
		            }
		            prev->next = current->next;
		        }
		
		        updateReservationFile(); // Update the reservation file
		    }
		    else
		    {	SetConsoleTextAttribute(color, 10);
		    	cout << endl;
		        cout << "Deletion canceled." << endl << endl;
		        SetConsoleTextAttribute(color, 14);
		        enterToGoBack();
		    }
		}
		
	void searchArt()
	{
		cout << "  _____ ______          _____   _____ _    _    _____   ____   ____  __  __ " << endl;
 		cout << " / ____|  ____|   /\\   |  __ \\ / ____| |  | |  |  __ \\ / __ \\ / __ \\|  \\/  | " << endl;
 		cout << "| (___ | |__     /  \\  | |__) | |    | |__| |  | |__) | |  | | |  | | \\  / |" << endl;
 		cout << " \\___ \\|  __|   / /\\ \\ |  _  /| |    |  __  |  |  _  /| |  | | |  | | |\\/| |" << endl;
 		cout << " ____) | |____ / ____ \\| | \\ \\| |____| |  | |  | | \\ \\| |__| | |__| | |  | |" << endl;
 		cout << "|_____/|______/_/    \\_\\_|  \\_\\\\_____|_|  |_|  |_|  \\_\\\\____/ \\____/|_|  |_| " << endl;
	}
	void historyArt()
	{
			system("CLS");
			cout << " _    _ _____  _____ _______ ____  _______     __ " << endl;
    		cout << "| |  | |_   _|/ ____|__   __/ __ \\|  __ \\ \\   / /" << endl;
    		cout << "| |__| | | | | (___    | | | |  | | |__) \\ \\_/ /  " << endl;
    		cout << "|  __  | | |  \\___ \\   | | | |  | |  _  / \\   /  " << endl;
    		cout << "| |  | |_| |_ ____) |  | | | |__| | | \\ \\  | |   " << endl;
    		cout << "|_|  |_|_____|_____/   |_|  \\____/|_|  \\_\\ |_|" << endl;
	}
	void deleteArt()
	{
		system("CLS");
			SetConsoleTextAttribute(color, 12);
			cout << " _____  ______ _      ______ _______ ______ " << endl;
    		cout << "|  __ \\|  ____| |    |  ____|__   __|  ____" << endl;
    		cout << "| |  | | |__  | |    | |__     | |  | |__   " << endl;
    		cout << "| |  | |  __| | |    |  __|    | |  |  __|   " << endl;
    		cout << "| |__| | |____| |____| |____   | |  | |____ " << endl;
    		cout << "|_____/|______|______|______|  |_|  |______|" << endl;
	}
};

welcome(){
		SetConsoleTextAttribute(color, 11);
		cout << "  _____ _____ _____  _____ " << endl;
 		cout << " / ____|_   _/ ____|/ ____|" << endl;
 		cout << "| |      | || |    | (___  " << endl;
 		cout << "| |      | || |     \\___ \\ " << endl;
 		cout << "| |____ _| || |____ ____) |" << endl;
 		cout << " \\_____|_____\\_____|_____/  " << endl;
 		
 		cout << endl;

		cout << "  _____ _                _____ _____ _____   ____   ____  __  __ " << endl;
 		cout << " / ____| |        /\\    / ____/ ____|  __ \\ / __ \\ / __ \\|  \\/  |" << endl;
 		cout << "| |    | |       /  \\  | (___| (___ | |__) | |  | | |  | | \\  / |" << endl;
 		cout << "| |    | |      / /\\ \\  \\___ \\\\___ \\|  _  /| |  | | |  | | |\\/| |" << endl;
 		cout << "| |____| |____ / ____ \\ ____) |___) | | \\ \\| |__| | |__| | |  | |" << endl;
 		cout << " \\_____|______/_/    \\_\\_____/_____/|_|  \\_\\\\____/ \\____/|_|  |_| " << endl;
 		
 		cout << endl;

 		cout << " _____  ______  _____ ______ _______      __  _______ _____ ____  _   _  " << endl;
 		cout << "|  __ \\|  ____|/ ____|  ____|  __ \\ \\    / /\\|__   __|_   _/ __ \\| \\ | |" << endl;
 		cout << "| |__) | |__  | (___ | |__  | |__) \\ \\  / /  \\  | |    | || |  | |  \\| |" << endl;
 		cout << "|  _  /|  __|  \\___ \\|  __| |  _  / \\ \\/ / /\\ \\ | |    | || |  | | . ` |" << endl;
 		cout << "| | \\ \\| |____ ____) | |____| | \\ \\  \\  / ____ \\| |   _| || |__| | |\\  |" << endl;
 		cout << "|_|  \\_\\______|_____/|______|_|  \\_\\  \\/_/    \\_\\_|  |_____\\____/|_| \\_|" << endl;
 		
 		cout << endl;

 		cout << "  _______     _______ _______ ______ __  __  " << endl;
 		cout << " / ____\\ \\   / / ____|__   __|  ____|  \\/  |" << endl;
 		cout << "| (___  \\ \\_/ / (___    | |  | |__  | \\  / |" << endl;
 		cout << " \\___ \\  \\   / \\___ \\   | |  |  __| | |\\/| |" << endl;
 		cout << " ____) |  | |  ____) |  | |  | |____| |  | |" << endl;
 		cout << "|_____/   |_| |_____/   |_|  |______|_|  |_|" << endl;
	}
int main() {
	ReservationSystem reservationSystem;
	welcome();
        cout << endl;
        reservationSystem.enterToGoBack();
    while (true) {
    	
   		int choice;
        system("CLS");
        system("CLS");
        time_t currentTime = time(0);	
        tm* currentTime_tm = localtime(&currentTime);
        SetConsoleTextAttribute(color, 14);
        cout << "Time: " << asctime(currentTime_tm);
        SetConsoleTextAttribute(color, 14);
		cout << "  __  __ ______ _    _ _    _   " << endl;
 		cout << " |  \\/  |  ____|  \\ | | |  | | " << endl;
 		cout << " | \\  / | |__  |   \\| | |  | |" << endl;
 		cout << " | |\\/| |  __| | .    | |  | |" << endl;
 		cout << " | |  | | |____| | \\  | |__| |" << endl;
 		cout << " |_|  |_|______|_|  \\_|\\____/ " << endl;
                            
        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "|                            |" << endl;
        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "| [1] Occupy a Room          |" << endl;
        cout << "| [2] Check Room Status      |" << endl;
        cout << "| [3] Check History          |" << endl;
        cout << "| [4] Delete a Reservation   |" << endl;
        cout << "| [5] Delete Most Recent     |" << endl;
        cout << "| [6] Search Availability    |" << endl;
        cout << "| [7] Exit                   |" << endl;
        cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-+" << endl;
        cout << "Choice: ";
        
        while (!(cin >> choice) || choice < 1 || choice > 7) 
		{
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            SetConsoleTextAttribute(color, 12);
            cout << "Please enter a valid choice (1-6): ";
            SetConsoleTextAttribute(color, 14);
        }
        
        switch (choice) {
            case 1:
            	system("CLS");
                reservationSystem.reserveRoom();
                break;
            case 2:
            	system("CLS");
                reservationSystem.checkRoomStatus();
                break;
            case 3:
            	system("CLS");
            	reservationSystem.historyArt();
                reservationSystem.sortsHistory();
                break;
            case 4:
            	system("CLS");
                reservationSystem.deleteReservation();
                break;
            case 5: 
            	system("CLS");
            	cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;
				cout << "|  DELETE MOST RECENT RESERVATION |" << endl;
				cout << "+=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=+" << endl;
				reservationSystem.deleteLastReservation();
            	break;
            case 7:
            	system("CLS");
                cout << endl;
				cout << "\t\t   _____                 _ _                  _ " << endl;
		 		cout << "\t\t  / ____|               | | |                | |" << endl;
		 		cout << "\t\t | |  __  ___   ___   __| | |__  _   _  ___  | |" << endl;
		 		cout << "\t\t | | |_ |/ _ \\ / _ \\ / _` | '_ \\| | | |/ _ \\ | |" << endl;
		 		cout << "\t\t | |__| | (_) | (_) | (_| | |_) | |_| |  __/ |_|" << endl;
		 		cout << "\t\t  \\_____|\\___/ \\___/ \\__,_|_.__/ \\__, |\\___| (_)" << endl;
		 		cout << "\t\t                                  __/ |         " << endl;
		 		cout << "\t\t                                 |___/          " << endl;
		 		
                return 0; 	
            case 6:
            	cin.clear();
          		cin.ignore(numeric_limits<streamsize>::max(), '\n');
            	reservationSystem.searchRoomAvailability();
    	}
	}
}
