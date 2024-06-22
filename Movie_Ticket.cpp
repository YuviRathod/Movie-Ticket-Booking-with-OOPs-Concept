#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include <algorithm>
#include <iomanip>
#include <unordered_map>

using namespace std;

class Displayable {
public:
    virtual void display() const = 0; // pure virtual function for displaying
};

class Movie : public Displayable {
protected:
    string name;
    string code;
    double price;
    string genre;
    string description;
    string cast;
    string theater;
    double duration;
    string showTimings;

public:
    Movie(string name, string code, double price, string genre, string description, string cast, string theater, double duration, string showTimings)
        : name(name), code(code), price(price), genre(genre), description(description), cast(cast), theater(theater), duration(duration), showTimings(showTimings) {}

    virtual void display() const override {
        cout << "Movie Name: " << name << ", Code: " << code << ", Price: " << price << ", Genre: " << genre << endl;
        cout << "Description: " << description << endl;
        cout << "Cast: " << cast << endl;
        cout << "Theater: " << theater << endl;
        cout << "Duration: " << duration << " hours" << endl;
        cout << "Show Timings: " << showTimings << endl;
    }

    string getCode() const { return code; }
    string getName() const { return name; }
    double getPrice() const { return price; }
    string getGenre() const { return genre; }
    string getDescription() const { return description; }
    string getCast() const { return cast; }
    string getTheater() const { return theater; }
    double getDuration() const { return duration; }
    string getShowTimings() const { return showTimings; }

    virtual ~Movie() = default;
};

class SeatManager {
    unordered_map<string, unordered_map<string, char[4][15]>> movieShowSeats;

public:
    SeatManager() {}

    void initializeSeats(const string &movieCode, const string &showTime) {
        for (int y = 0; y < 4; y++) {
            for (int x = 0; x < 15; x++) {
                movieShowSeats[movieCode][showTime][y][x] = ' ';
            }
        }
    }

    bool isSeatAvailable(const string &movieCode, const string &showTime, int row, int col) const {
        return movieShowSeats.at(movieCode).at(showTime)[row][col] == ' ';
    }

    void bookSeat(const string &movieCode, const string &showTime, int row, int col) {
        if (!isSeatAvailable(movieCode, showTime, row, col)) {
            throw runtime_error("Seat is already booked");
        }
        movieShowSeats[movieCode][showTime][row][col] = 'B';
    }

    void displaySeats(const string &movieCode, const string &showTime) const {
        cout << "   ";
        for (int screen = 0; screen <= 25; screen++) {
            cout << (char)254;
        }
        cout << " SCREEN ";
        for (int screen = 0; screen <= 24; screen++) {
            cout << (char)254;
        }
        cout << endl;
        for (int y = 0; y < 4; y++) {
            cout << y << " " << (char)179 << " ";
            for (int x = 0; x < 15; x++) {
                cout << movieShowSeats.at(movieCode).at(showTime)[y][x] << " " << (char)179 << " ";
            }
            cout << endl;
        }
        cout << " ";
        for (int z = 0; z < 15; z++) {
            cout << fixed << setw(4) << right << z;
        }
        cout << endl;
        cout << "   ";
        for (int w = 1; w <= 59; w++) {
            cout << (char)205;
        }
        cout << endl << endl;
    }

    void saveSeatAllocation(const string &filename) const {
        ofstream file(filename);
        for (const auto &movie : movieShowSeats) {
            for (const auto &show : movie.second) {
                file << movie.first << " " << show.first << endl;
                for (int y = 0; y < 4; y++) {
                    for (int x = 0; x < 15; x++) {
                        file << show.second[y][x];
                    }
                    file << endl;
                }
            }
        }
        file.close();
    }

    void loadSeatAllocation(const string &filename) {
        ifstream file(filename);
        if (!file) return;

        string movieCode, showTime;
        while (file >> movieCode >> showTime) {
            for (int y = 0; y < 4; y++) {
                for (int x = 0; x < 15; x++) {
                    file >> movieShowSeats[movieCode][showTime][y][x];
                }
            }
        }
        file.close();
    }
};

class MovieManager {
    vector<Movie *> movies;

public:
    void loadMovies() {
        ifstream file("movies.txt");
        if (!file) return;

        string line;
        while (getline(file, line)) {
            stringstream ss(line);
            string name, code, genre, description, cast, theater, showTimings;
            double price, duration;
            ss >> name >> code >> price >> genre >> ws;
            getline(ss, description, ';');
            getline(ss, cast, ';');
            getline(ss, theater, ';');
            ss >> duration >> ws;
            getline(ss, showTimings);

            movies.push_back(new Movie(name, code, price, genre, description, cast, theater, duration, showTimings));
        }
        file.close();
    }

    void saveMovies() const {
        ofstream file("movies.txt");
        for (const auto &movie : movies) {
            file << movie->getName() << " " << movie->getCode() << " " << movie->getPrice() << " " << movie->getGenre() << " "
                 << movie->getDescription() << ";" << movie->getCast() << ";" << movie->getTheater() << ";"
                 << movie->getDuration() << " " << movie->getShowTimings() << endl;
        }
        file.close();
    }

    void addMovie(Movie *movie) {
        movies.push_back(movie);
        saveMovies();
    }

    void deleteMovie(const string &name) {
        auto it = remove_if(movies.begin(), movies.end(), [&name](Movie *movie) { return movie->getName() == name; });
        for (auto i = it; i != movies.end(); ++i) {
            delete *i;
        }
        movies.erase(it, movies.end());
        saveMovies();
    }

    void displayMovies() const {
        cout << "--------------------------------------------------Existing Movies---------------------------------------" << endl;
        for (const auto &movie : movies) {
            movie->display();
            cout << "--------------------------------------------------------------------------------------------------------" << endl;
        }
    }

    Movie *findMovieByCode(const string &code) const {
        for (const auto &movie : movies) {
            if (movie->getCode() == code)
                return movie;
        }
        return nullptr;
    }

    ~MovieManager() {
        for (auto movie : movies) {
            delete movie;
        }
    }
};

class Ticket : public Displayable {
    Movie *movie;
    int row;
    int col;

public:
    Ticket(Movie *movie, int row, int col)
        : movie(movie), row(row), col(col) {}

    virtual void display() const override {
        cout << "Ticket: " << endl;
        cout << "Seat: Row " << row + 1 << ", Column " << col + 1 << endl;
        cout << "Price: ₹" << movie->getPrice() << endl;
    }
};

class Admin {
    string username;
    string password;

public:
    Admin(string username, string password)
        : username(username), password(password) {}

    bool login(const string &user, const string &pass) const {
        return username == user && password == pass;
    }
};

int main() {
    Admin admin("rohit", "123");
    MovieManager movieManager;
    movieManager.loadMovies();
    SeatManager seatManager;
    seatManager.loadSeatAllocation("seat_allocations.txt");
    bool isLoggedIn = false;

    while (true) {
        if (!isLoggedIn) {
            cout << "---------------Welcome to Movie Ticket Booking--------------------" << endl;
            string username, password;
            cout << "Enter username: ";
            cin >> username;
            cout << "Enter password: ";
            cin >> password;

            if (admin.login(username, password)) {
                isLoggedIn = true;
                cout << "Login successful!" << endl;
            } else {
                cout << "Login failed. Try again." << endl;
                continue;
            }
        }

        int choice;
        cout << "1. Add Movie\n2. Delete Movie\n3. Generate Ticket\n4. Logout\n5. Exit\nEnter choice: ";
        cin >> choice;

        try {
            if (choice == 1) {
                string name, code, genre, description, cast, theater, showTimings;
                double price, duration;
                cout << "Enter movie name: ";
                cin >> name;
                cout << "Enter movie code: ";
                cin >> code;
                cout << "Enter movie price: ";
                cin >> price;
                cout << "Enter movie genre: ";
                cin >> genre;
                cin.ignore(); // to ignore the newline character left by previous cin
                cout << "Enter movie description: ";
                getline(cin, description);
                cout << "Enter movie cast: ";
                getline(cin, cast);
                cout << "Enter theater details: ";
                getline(cin, theater);
                cout << "Enter movie duration (in hours): ";
                cin >> duration;
                cin.ignore(); // to ignore the newline character left by previous cin
                cout << "Enter show timings: ";
                getline(cin, showTimings);

                movieManager.addMovie(new Movie(name, code, price, genre, description, cast, theater, duration, showTimings));
                cout << "Movie added successfully." << endl;
            } else if (choice == 2) {
                string name;
                cout << "Enter movie name to delete: ";
                cin >> name;
                movieManager.deleteMovie(name);
                cout << "Movie deleted successfully." << endl;
            } else if (choice == 3) {
                movieManager.displayMovies();
                string code;
                cout << "Enter movie code to generate ticket: ";
                cin >> code;

                Movie *movie = movieManager.findMovieByCode(code);
                if (!movie) {
                    throw runtime_error("Movie not found");
                }

                cout << "Available show timings: " << movie->getShowTimings() << endl;
                string showTime;
                cout << "Enter show timing: ";
                cin >> showTime;

                seatManager.initializeSeats(code, showTime);

                int numTickets, upTickets, downTickets;
                cout << "Enter number of tickets: ";
                cin >> numTickets;

                cout << "Enter number of upper row tickets: ";
                cin >> upTickets;
                if (upTickets > numTickets) {
                    throw runtime_error("Number of upper row tickets cannot exceed total tickets");
                }
                downTickets = numTickets - upTickets;

                seatManager.displaySeats(code, showTime);

                double totalCost = 0;
                for (int i = 0; i < numTickets; ++i) {
                    int row, col;
                    cout << "Enter seat row (0-3) for ticket " << i + 1 << ": ";
                    cin >> row;
                    cout << "Enter seat column (0-14) for ticket " << i + 1 << ": ";
                    cin >> col;

                    if (row < 0 || row >= 4 || col < 0 || col >= 15) {
                        throw runtime_error("Invalid seat");
                    }

                    seatManager.bookSeat(code, showTime, row, col);

                    double ticketPrice = movie->getPrice();
                    if (i < upTickets) {
                        ticketPrice += 50; // Additional cost for upper row tickets
                    }
                    totalCost += ticketPrice;

                    Ticket ticket(movie, row, col);
                    cout<<"-------------------------------------------------------"<<endl;
                    ticket.display();
                    cout<<"-------------------------------------------------------"<<endl;
                }

                cout << "Total cost: " << totalCost << endl;
                double amountPaid;
                cout << "Enter amount to pay: ";
                cin >> amountPaid;

                if (amountPaid < totalCost) {
                    throw runtime_error("Insufficient funds");
                }

                cout << "Ticket(s) generated successfully." << endl;
                cout<<"------------------Booked movie details-------------------"<<endl;
                movie->display();
                seatManager.saveSeatAllocation("seat_allocations.txt");
            } else if (choice == 4) {
                isLoggedIn = false;
                cout << "Logged out successfully." << endl;
            } else if (choice == 5) {
                break;
            } else {
                cout << "Invalid choice. Try again." << endl;
            }
        } catch (const exception &e) {
            cerr << "Error: " << e.what() << endl;
        }
    }
    return 0;
}
