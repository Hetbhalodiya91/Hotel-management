#include<iostream>
#include<string.h>
#include<string>
#include <iomanip>
#include <fstream>
#include<istream>
#include<algorithm>
using namespace std;



class bill
{
public:

    string s[50]; // Array to store item names
    int h[50]={0};    // Array to store item prices
    int total = 0;
    int itemNo = 0; // Tracks the number of items
    int sIteam[50]={0};
    static int orderNo;
   static int dailyTotalOnCash;
   static int dailyTotalOnOnline;

   static string billFilename;
   static string monthlyFile;


   // Function to get the current date as a string in YYYY-MM-DD format
string getCurrentMonth() {
    time_t now = time(0); // Get the current system time
    tm *ltm = localtime(&now); // Convert time_t to a local time structure (tm)

    stringstream ss; // String stream to build the formatted date string
    ss << 1900 + ltm->tm_year << "-"; // Extract year (tm_year stores years since 1900)
    
    // Extract month and format with leading zero if necessary
    ss << (ltm->tm_mon + 1 < 10 ? "0" : "") << ltm->tm_mon + 1;
    
    // // Extract day and format with leading zero if necessary
    // ss << (ltm->tm_mday < 10 ? "0" : "") << ltm->tm_mday;

    return ss.str(); // Return the formatted date string
}

    void addItem(string se, int price,int qu)
    {
        // Check if the item already exists
        for (int i = 0; i < itemNo; i++)
        {
            if (s[i] == se) // If item name matches
            {
                h[i] += price; // Add price to the existing item
                total += price;
                sIteam[i]+=qu;
                return; // Exit after updating existing item
            }
        }

        // If item does not exist, add a new entry
        if (itemNo < 50) // Ensure we don't exceed the array limit
        {
            s[itemNo] = se;
            h[itemNo] = price;
            total += price;
            sIteam[itemNo]+=qu;
            itemNo++; // Increment item counter
        }
        else
        {
            cout << "Cannot add more items. Bill is full!" << endl;
        }
    }


    void display() {
        // Print header line
        cout << "------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << setw(80) << "              JC's Hotel - Bill Summary" << endl;
        cout << setw(50) << "   " << endl;  // Empty line for better spacing
        cout << endl;
    
        // Display a column header for the bill summary
        cout << left << setw(5) << "No." << setw(35) << "Item Description" << setw(20) << "Quantity" << setw(20) << "Price" << endl;
        cout << "------------------------------------------------------------------------------------------------------------------------------" << endl;
    
        // Loop through each item and display details
        for (int i = 0; i < itemNo; i++) {
            // Ensure the array indexes (s[i], sIteam[i], h[i]) are valid
            if (i < itemNo) {
                cout << left << setw(5) << (i + 1)  // Item number (1-based)
                     << setw(35) << s[i]          // Item description
                     << setw(20) << sIteam[i]     // Item quantity
                     << "Rs." << setw(13) << fixed << setprecision(2) << h[i] << endl; // Item price with formatting
            }
        }
    
        // Print total cost with formatting
        cout << "------------------------------------------------------------------------------------------------------------------------------" << endl;
        cout << setw(60) << "Total: " << fixed << setprecision(2) <<"Rs."<< total << endl;
    
        // Footer line
        cout << "------------------------------------------------------------------------------------------------------------------------------" << endl;
    }

    void readLastTotalPayments(const string& billFilename, int& totalOnline, int& totalCash) {
        ifstream file(billFilename, ios::ate); // Open file at the end
        if (!file) {
            cout << "Error opening file!" << endl;
            totalOnline = 0;
            totalCash = 0;
            return;
        }
    
        // Get file size
        streampos fileSize = file.tellg();
        if (fileSize == streampos(0)) { // Empty file
            cout << "File is empty. No previous payments found." << endl;
            totalOnline = 0;
            totalCash = 0;
            file.close();
            return;
        }
    
        const int bufferSize = 1024; // Read in chunks (1 KB at a time)
        char buffer[bufferSize + 1]; 
        buffer[bufferSize] = '\0'; // Null-terminate the buffer
    
        streampos pos = fileSize - static_cast<streampos>(bufferSize);
        string lastChunk;
        
        while (pos >= 0) {
            if (pos < 0) pos = 0; // Ensure valid position
            file.seekg(pos);
            file.read(buffer, bufferSize);
            
            lastChunk = string(buffer) + lastChunk; // Store the read chunk
            
            // Look for the last occurrences of "Total of online :-" and "Total of cash :-"
            size_t onlinePos = lastChunk.rfind("Total of online :-");
            size_t cashPos = lastChunk.rfind("Total of cash :-");
    
            if (onlinePos != string::npos && cashPos != string::npos) {
                stringstream ssOnline(lastChunk.substr(onlinePos + 18)); // Extract number
                ssOnline >> totalOnline;
    
                stringstream ssCash(lastChunk.substr(cashPos + 16)); // Extract number
                ssCash >> totalCash;
    
                break; // Stop reading once we have found both totals
            }
    
            pos -= bufferSize; // Move backwards
        }
    
        file.close();
    
        if (totalOnline == 0 && totalCash == 0) {
            cout << "No valid total payments found. Defaulting to 0." << endl;
        } else {
            cout << "Last recorded totals -> Online: " << totalOnline << ", Cash: " << totalCash << endl;
        }
    }
    
    void saveToMonthlyReport(int tableNumber,int mode)
    {
        monthlyFile = "bills_" + getCurrentMonth() + ".txt";

        readLastTotalPayments(billFilename,dailyTotalOnOnline,dailyTotalOnCash);
        readLastOrderNo();
        // ofstream file("bills.txt", ios::app);
        ofstream file(monthlyFile, ios::app);
        if (!file)
        {
            cout << "Error opening file!" << endl;
            return;
        }
        if (mode==1)
        {
            dailyTotalOnOnline+=total;
        }
        else 
        {
            dailyTotalOnCash+=total;
        }
   
        orderNo++; 
        file << "----------------------------------------------------------------------------------------------------------------\n";
        file << "Table Number: " << tableNumber << "\n";
        file << "                                                    JC's Hotel \n";
        file <<"Order no:"<< orderNo <<setw(40) << "    Bill Summary\n\n";
        for (int i = 0; i < itemNo; i++)
        {
            file << setw(32) << s[i] << "  * " << sIteam[i] << "  - Rs." << h[i] << endl;
        }
        file << "                                                                 Total: Rs." << total << "\n";
        file << "----------------------------------------------------------------------------------------------------------------\n";
        file << "Total of wallet :-"<<dailyTotalOnCash+dailyTotalOnOnline<<endl;
        file << "Total of online :-"<<dailyTotalOnOnline<<endl;
        file << "Total of cash :-"<<dailyTotalOnCash<<endl;
        file <<"Order no:"<< orderNo<<endl;
        file.close();
    }
   
    void saveToFile(int tableNumber,int mode)
    {
   readLastTotalPayments(billFilename,dailyTotalOnOnline,dailyTotalOnCash);
        readLastOrderNo();
        // ofstream file("bills.txt", ios::app);
        ofstream file(billFilename, ios::app);
        if (!file)
        {
            cout << "Error opening file!" << endl;
            return;
        }
        if (mode==1)
        {
            dailyTotalOnOnline+=total;
        }
        else 
        {
            dailyTotalOnCash+=total;
        }
   
        orderNo++; 
        file << "----------------------------------------------------------------------------------------------------------------\n";
        file << "Table Number: " << tableNumber << "\n";
        file << "                                                    JC's Hotel \n";
        file <<"Order no:"<< orderNo <<setw(40) << "    Bill Summary\n\n";
        for (int i = 0; i < itemNo; i++)
        {
            file << setw(32) << s[i] << "  * " << sIteam[i] << "  - Rs." << h[i] << endl;
        }
        file << "                                                                 Total: Rs." << total << "\n";
        file << "----------------------------------------------------------------------------------------------------------------\n";
        file << "Total of wallet :-"<<dailyTotalOnCash+dailyTotalOnOnline<<endl;
        file << "Total of online :-"<<dailyTotalOnOnline<<endl;
        file << "Total of cash :-"<<dailyTotalOnCash<<endl;
        file <<"Order no:"<< orderNo<<endl;
        file.close();
    }
    // <<"   "<< "Total of online :-"<<dailyTotalOnOnline<<"   "<<"Total of cash :-"<<dailyTotalOnCash<<"    Total wallet is :- "<<dailyTotalOnCash+dailyTotalOnOnline
    void readBills()
    {
        // ifstream file("bills.txt");
        ifstream file(billFilename);
        if (!file)
        {
            cout << "No past bills found!" << endl;
            return;
        }

        string line;
        while (getline(file, line))
        {
            cout << line << endl;
        }
        file.close();
    }
    

void readLastOrderNo() {
    ifstream file(billFilename, ios::ate); // Open the file and move the cursor to the end
    if (!file) { // Check if the file was opened successfully
        cout << "No previous bills found. Starting fresh." << endl;
        orderNo = 0; // Reset order number to 0
        return;
    }

    // Get the size of the file
    streampos fileSize = file.tellg();
    if (fileSize == streampos(0)) { // If the file is empty
        cout << "File is empty. Starting fresh." << endl;
        orderNo = 0;
        file.close();
        return;
    }

    // Buffer to store chunks of the file
    const int bufferSize = 1024; // Read 1 KB at a time
    char buffer[bufferSize + 1]; // +1 for null terminator
    buffer[bufferSize] = '\0'; // Null-terminate the buffer

    // Start reading from the end of the file
    // streampos pos = fileSize - bufferSize;
    streampos pos = fileSize - static_cast<streampos>(bufferSize);

    bool orderFound = false; // Flag to check if the order number was found
    int lastOrder = 0; // Variable to store the last order number

    while (pos >= 0) {
        // Ensure `pos` is non-negative
        if (pos < 0) pos = 0;  // Ensure we don't go before the start of the file

        file.seekg(pos); // Move the cursor to the current position
        file.read(buffer, bufferSize); // Read a chunk of the file

        // Search for "Order no:" in the buffer
        string chunk(buffer);
        size_t foundPos = chunk.rfind("Order no:"); // Find the last occurrence of "Order no:"
        if (foundPos != string::npos) {
            // Extract the order number
            stringstream ss(chunk.substr(foundPos + 9)); // Extract the substring after "Order no:"
            if (ss >> lastOrder) { // Convert the substring to an integer
                orderFound = true; // Set the flag to true
                break; // Exit the loop
            } else {
                cerr << "Warning: Invalid order number format in file." << endl;
            }
        }

        // Move backward in the file
        pos -= bufferSize;
    }

    file.close(); // Close the file

    if (orderFound) {
        orderNo = lastOrder; // Update the static order number
        cout << "Resuming from last order number: " << orderNo << endl; // Print the last order number
    } else {
        cout << "No valid order numbers found. Starting fresh." << endl;
        orderNo = 0; // Reset order number to 0
    }
}

};

int bill ::orderNo=0;
int bill ::dailyTotalOnCash=0;
int bill ::dailyTotalOnOnline=0;
string bill ::billFilename="";
string bill :: monthlyFile="";

class Night 
{
    public:
    // Function to display the starter menu and take user input
    void starter(bill &e) 
    {
        while(1) // Infinite loop until user chooses to quit
        {
            // Display menu options with prices
            cout<<"\n1 : manchau soup         ::-80\n";
            cout<<"\n2 : tomato soup          ::-60\n";
            cout<<"\n3 : veg soup             ::-50\n"; 
            cout<<"\n4 : white souse pasta    ::-180\n";
            cout<<"\n5 : red souse pasta      ::-170\n";
            cout<<"\n6 : garlic bread         ::-80\n"; 
            cout<<"\n7 : cheese garlic bread  ::-95\n";
            cout<<"\n8 : french fries         ::-90\n";
            cout<<"\n9 : Spring Rolls (6 pcs) ::-150\n";
            cout<<"\n10 : quit\n";

            int s; // Variable to store user input
            cin>>s; // Take input from user
            // Process user's choice using switch-case
            switch (s)
            {int qu;
                case 1:           
                cout<<"Enter quntuty:- ";
                cin>>qu;
                    e.addItem("manchau soup           80 ",80*qu,qu); 
                    break;
                case 2:          
                cout<<"Enter quntuty:- ";
                cin>>qu;
                    e.addItem("tomato soup            60 ",60*qu,qu); 
                    break;  
                case 3:           
                cout<<"Enter quntuty:- ";
                cin>>qu;
                    e.addItem("veg soup               50 ",50*qu,qu);
                    break;
                case 4:         
                cout<<"Enter quntuty:- ";
                cin>>qu;
                    e.addItem("white souse pasta      180",180*qu,qu);
                    break;
                case 5:              
                cout<<"Enter quntuty:- ";
                cin>>qu;
                    e.addItem("red souse pasta        170",170*qu,qu);
                    break;
                case 6:      
                cout<<"Enter quntuty:- ";
                cin>>qu;
                    e.addItem("garlic bread           80 ",80*qu,qu); 
                    break;
                case 7:           cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("cheese garlic bread    95 ",95*qu,qu);
                    break;
                case 8:    cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("french fries           90 ",90*qu,qu);  
                    break;
                case 9:    cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("Spring Rolls (6 pcs)   150",150*qu,qu);  
                    break;

                case 10: 
                    return; // Exit function when user selects 'quit'
                default:
                    cout<<"invalid choice"; // Handle invalid input
                    break;
            }
        }
        return; // Return to caller
    }

    // Function to display the main course menu
    void maincourse(bill &e) 
    {
        while(1) // Infinite loop until user chooses to quit
        {
            // Display menu options
            cout<<"\n1 : kaju masala          ::-100\n";
            cout<<"\n2 : kaju tava masala     ::-129\n";
            cout<<"\n3 : kaju kadai           ::-119\n";
            cout<<"\n4 : paneer masala        ::-189\n";
            cout<<"\n5 : paneer kofta         ::-199\n";
            cout<<"\n6 : chapati              ::-10\n";
            cout<<"\n7 : naan                 ::-15\n";
            cout<<"\n8 : tanduri roti         ::-20\n";
            cout<<"\n9 : papad                ::-5 \n";
            cout<<"\n10 : quit\n";

            int s; // Variable for user input
            cin>>s; // Take user input
            // Process user input
            switch (s)
            {int qu;
                case 1: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("kaju masala            100",100*qu,qu);
                    break;
                case 2: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("kaju tava masala       129",129*qu,qu);
                    break;  
                case 3: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("kaju kadai             119",119*qu,qu);
                    break;
                case 4: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("paneer masala          189",189*qu,qu);
                    break;
                case 5: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("paneer kofta           199",199*qu,qu);
                    break;
                case 6: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("chapati                10 ",10*qu,qu);
                    break;
                case 7: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("naan                   15 ",15*qu,qu);
                    break;
                case 8: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("tanduri roti           20 ",20*qu,qu);
                    break;
                case 9: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("papad                  5  ",5*qu,qu);
                    break;
                case 10: 
                    return; // Exit function when user selects 'quit'
                default:
                    cout<<"invalid choice";
                    break;
            }
        }
        return;
    }

    // Function to display the dessert menu
    void desert(bill &e) 
    {
        while (1) // Infinite loop until user chooses to quit
        {
            cout<<"\n1 : gulab jamun          ::-30\n";
            cout<<"\n2 : rasgulla             ::-20\n";
            cout<<"\n3 : ice cream            ::-50\n";
            cout<<"\n4 : gajar ka halwa       ::-40\n";
            cout<<"\n5 : wholenul shreekhand  ::-90\n";
            cout<<"\n6 : chees cake           ::-110\n";
            cout<<"\n7 : quit\n";

            int s;
            cin>>s;
            switch (s)
            {int qu;
                case 1: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("gulab jamun            30 ",30*qu,qu);
                    break;
                case 2: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("rasgulla               20 ",20*qu,qu);
                    break;  
                case 3: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("ice cream              50 ",50*qu,qu);
                    break;
                case 4: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("gajar ka halwa         40 ",40*qu,qu);
                    break;
                case 5: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("wholenul shreekhand    90 ",90*qu,qu);
                    break;
                case 6: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("chees cake             110",110*qu,qu);
                    break;
                case 7:
                    return; // Exit function
                default:
                    cout<<"invalid choice";
                    break;
            }
        }
        return;
    }

    // Function to display the ice cream menu
    void icecream(bill &e) 
    {
        while(1)
        {
            cout<<"\n1 : vanilla              ::-30\n";
            cout<<"\n2 : chocolate            ::-40\n";
            cout<<"\n3 : strawberry           ::-50\n";
            cout<<"\n4 : butter scotch        ::-35\n";
            cout<<"\n5 : vanila brownie       ::-60\n";
            cout<<"\n6 : rajbhog              ::-55\n";
            cout<<"\n7 : quit\n";

            int s;
            cin>>s;

            switch (s)
            {int qu;
                case 1: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("vanilla                30 ",30*qu,qu);
                    break;
                case 2: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("Chocolate              40 ",40*qu,qu);
                    break;  
                case 3: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("Strawberry             50 ",50*qu,qu);
                    break;
                case 4: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("butter scotch          35 ",35*qu,qu);
                    break;
                case 5: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("vanila brownie         60 ",60*qu,qu);
                    break;
                case 6: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("rajbhog                55 ",55*qu,qu);
                    break;
                case 7:
                    return; // Exit function
                default:
                    cout<<"invalid choice";
                    break;
            }
        }
        return;
    }

    // Function to display the cold drink menu
    void coldDrink(bill &e) 
    {
        while(1)
        {
            cout<<"\n1 : pepsi                :-> 30\n";
            cout<<"\n2 : coke                 :-> 30\n";
            cout<<"\n3 : sprite               :-> 30\n";
            cout<<"\n4 : fanta                :-> 30\n";
            cout<<"\n5 : quit\n";

            int s;
            cin>>s;

            switch (s)
            {int qu;
                case 1: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("Pepsi                  30 ",30*qu,qu);
                    break;
                case 2: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("Coke                   30 ",30*qu,qu);
                    break;  
                case 3: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("Sprite                 30 ",30*qu,qu);
                    break;
                case 4: cout<<"Enter quntuty:- ";cin>>qu;
                    e.addItem("Fanta                  30 ",30*qu,qu);
                    break;
                case 5: 
                    return; // Exit function
                default:
                    cout<<"invalid choice";
                    break;
            }
        }
        return;
    }
};

class morning
{
    public:
    void mornig(bill &e)  // Method to display morning menu and take orders
{
    int w = 0; // Flag to control while loop
    while (true) // Infinite loop until user chooses to quit
    {
        // Displaying the morning menu with item names and prices
        cout << "\n1 : Poha                 ::-80\n";
        cout << "\n2 : Jalebi               ::-60\n";
        cout << "\n3 : Gathiya              ::-30\n"; 
        cout << "\n4 : Maskaban             ::-280\n";
        cout << "\n5 : Upma                 ::-300\n";
        cout << "\n6 : Idli                 ::-80\n"; 
        cout << "\n7 : Podi idli            ::-100\n";
        cout << "\n8 : Khaman               ::-120\n";
        cout << "\n9 : Chai                 ::-120\n";
        cout << "\n10 : Sp. Chai            ::-120\n";
        cout << "\n11 : Coffee              ::-120\n";
        cout << "\n12 : Turkish Coffee      ::-120\n";
        cout << "\n13 : Filter Coffee       ::-120\n";
        cout << "\n14 : Milk                ::-120\n";
        cout << "\n15 : Quit\n";  // Option to exit

        int s; // Variable to store user selection
        cin >> s; // Take user input for menu selection
        // Switch case to handle user selection
        switch (s)
        {int qu;
            case 1:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Poha                   80 ", 80*qu,qu); break;
            case 2:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Jalebi                 60 ", 60*qu,qu); break;
            case 3:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Gathiya                30 ", 30*qu,qu); break;
            case 4:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Maskaban               280", 280*qu,qu); break;
            case 5:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Upma                   300", 300*qu,qu); break;
            case 6:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Idli                   50 ", 50*qu,qu); break; // Mismatch in price in original code
            case 7:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Podi Idli              70 ", 70*qu,qu); break;
            case 8:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Khaman                 40 ", 40*qu,qu);break;
            case 9:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Tea                    40 ", 40*qu,qu); break;
            case 10:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Sp. Tea                40 ", 40*qu,qu); break;
            case 11:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Coffee                 40 ", 40*qu,qu); break;
            case 12:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Turkish Coffee         40 ", 40*qu,qu); break;
            case 13:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Filter Coffee          40 ", 40*qu,qu); break;
            case 14:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Milk                   40 ", 40*qu,qu); break;
            case 15: w = 1; break; // Set flag to exit
            default: cout << "Invalid choice\n"; break;
        }

        if (w == 1) break; // Break out of loop if user selects Quit (15)
    }
    return;
}
}

;


class Noon  // Class to handle noon/lunch menu
{
public:
void gujarati(bill &e) // Method for Gujarati food menu
{
    int w = 0;
    while (true)
    {
        // Displaying Gujarati menu
        cout << "\n1 : Bharela-Ringan Bateka  ::-80\n";
        cout << "\n2 : Lasaniya Bateta        ::-60\n";
        cout << "\n3 : Suki Bhaji             ::-30\n"; 
        cout << "\n4 : Sev-Tameta             ::-280\n";
        cout << "\n5 : Akhi Dungali           ::-300\n";
        cout << "\n6 : Undhiyu                ::-80\n"; 
        cout << "\n7 : Umbadiyu               ::-100\n";
        cout << "\n8 : Dal-Dhokli             ::-120\n";
        cout << "\n9 : Vagharello-Rotlo       ::-130\n";
        cout << "\n10 : Quit\n";  // Option to exit

        int forSwitch;
        cin >> forSwitch;  // Take user input
        system("CLS");
        switch (forSwitch) // Handle user selection
        { int qu;
            case 1:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Bharela-Ringan Bateka  80 ", 80*qu,qu); break;
            case 2:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Lasaniya Bateta        60 ", 60*qu,qu); break;
            case 3:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Suki Bhaji             30 ", 30*qu,qu); break;
            case 4:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Sev-Tameta             280", 280*qu,qu); break;
            case 5:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Akhi Dungali           300", 300*qu,qu); break;
            case 6:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Undhiyu                50 ", 50*qu,qu); break;
            case 7:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Umbadiyu               70 ", 70*qu,qu); break;
            case 8:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Dal-Dhokli             40 ", 40*qu,qu); break;
            case 9:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Vagharello-Rotlo       40 ", 40*qu,qu); break;
            case 10: return; // Exit function
            default: cout << "Invalid choice\n"; break;
        }
    }
}

void punjabi(bill &e) // Method for Punjabi food menu
{
    int w = 0;
    while (true)
    {
        // Displaying Punjabi menu
        cout << "\n1 : Kaju Masala          ::-100\n";
        cout << "\n2 : Kaju Tava Masala     ::-129\n";
        cout << "\n3 : Kaju Kadai           ::-119\n";
        cout << "\n4 : Paneer Masala        ::-189\n";
        cout << "\n5 : Paneer Kofta         ::-199\n";
        cout << "\n6 : Chapati              ::-10\n";
        cout << "\n7 : Naan                 ::-15\n";
        cout << "\n8 : Tandoori Roti        ::-20\n";
        cout << "\n9 : Papad                ::-5 \n";
        cout << "\n10 : Quit\n";  // Option to exit

        int forSwitch;
        cin >> forSwitch; // Take user input
        system("CLS");
        switch (forSwitch) // Handle user selection
        {int qu;
            case 1:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Kaju Masala            100", 100*qu,qu); break;
            case 2:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Kaju Tava Masala       129", 129*qu,qu); break;
            case 3:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Kaju Kadai             119", 119*qu,qu); break;
            case 4:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Paneer Masala          189", 189*qu,qu); break;
            case 5:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Paneer Kofta           199", 199*qu,qu); break;
            case 6:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Chapati                10 ", 10*qu,qu); break;
            case 7:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Naan                   15 ", 15*qu,qu); break;
            case 8:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Tandoori Roti          20 ", 20*qu,qu); break;
            case 9:cout<<"Enter quntuty:- ";cin>>qu; e.addItem("Papad                  5  ", 5*qu,qu); break;
            case 10: return; // Exit function
            default: cout << "Invalid choice\n"; break;
        }
    }
}

    void roti(bill &e)
        {
                int w=0;
                while(1)
                {
                  cout<<"\n1 : Fulka Roti       ::-80\n";
                 cout<<"\n2 : Chapati          ::-60\n";
                 cout<<"\n3 : BUtter Chapati            ::-30\n"; 
                 cout<<"\n4 : Naan      ::-280\n";
                 cout<<"\n5 : Butter Naan      ::-300\n";
                 cout<<"\n6 : Tandoori         ::-80\n"; 
                 cout<<"\n7 : Butter Tandoori  ::-100\n";
                 cout<<"\n8 : Roomali        ::-120\n";
                 cout<<"\n9 : quit\n";
                 int forSwitch;
                 cin>>forSwitch;
                 system("CLS");
                 switch (forSwitch)
                 {  int qu;
                     case 1:  cout<<"Enter quntuty:- ";cin>>qu;
                     e.addItem("Fulka Roti             80 ",80,qu);
                         break;
                     case 2:  cout<<"Enter quntuty:- ";cin>>qu;
                     e.addItem("Chapati                60 ",60,qu);
                         break;  
                     case 3:  cout<<"Enter quntuty:- ";cin>>qu;
                     e.addItem("BUtter Chapati         30 ",30,qu);
                         break;
                     case 4:  cout<<"Enter quntuty:- ";cin>>qu;
                     e.addItem("Naan                   280",280,qu);
                         break;
                     case 5:    cout<<"Enter quntuty:- ";cin>>qu;
                     e.addItem("Butter Naan            300",300,qu);
                         break;
                     case 6:  cout<<"Enter quntuty:- ";cin>>qu;
                     e.addItem("Tandoori               50 ",50,qu);
                         break;
                     case 7:  cout<<"Enter quntuty:- ";cin>>qu;
                     e.addItem("Butter Tandoori        70 ",70,qu);
                         break;
                     case 8:     cout<<"Enter quntuty:- ";cin>>qu;
                     e.addItem("Roomali                40 ",40,qu);
                         break;
                     case 9: w=1;
                         break;
                         default:cout<<"invalid choice";
                         break;
                 }
                    if(w==1)
                    {
                        return;
                    }
                }
         }
};


void noon(){
   Noon k[11];
    bill b[11];

    int table[11]={0};
 int choicInputManu;
 int choic;
 int bi=0;

    while (true)
    {
        int EXIt=0;
        cout<<"welcome to jc's hotel"<<endl;

        cout<<"1. Take order"<<endl;
        cout<<"2. Bill"<<endl;
        cout<<"3. Back to timing"<<endl;
        cout<<"4. EXIT"<<endl;
        cin>>choicInputManu;
        system("CLS");
        switch (choicInputManu)
        {
        case 1:
  
        while (true)
            {
               
            if (EXIt==0)
                {
                    cout<<"Free tables are :";
            
                for (int i = 1; i <= 10; i++)
                {
                    if (table[i]==0)
                    {
                        cout<<i<<" ";
                    }
                }
                cout<<endl;
                cout<<endl;
                cout<<"start tables are :";
                for (int i = 1; i <= 10; i++)
                {
                    if (table[i]==1)
                    {
                        cout<<i<<" ";
                    }
                }
                cout<<endl;
                cout<<"Enter seat no. :";
              
                cin>>choic;
                table[choic]=1;
                system("CLS");
                }

                int forManuSelection;    int qu;
                cout << "\n1 : Gujarati\n";
                cout << "\n2 : Punjabi\n";
                cout << "\n3 : Roti\n";
                cout << "\n4 : Gujarati Thali       :: 200rs \n";
                cout << "\n5 : Punjabi Thali        :: 250rs\n";
                cout << "\n6 : Quit\n";
                cin>>forManuSelection;
                switch (forManuSelection)
                {
                case 1:
                    k[choic].gujarati(b[choic]);
                    EXIt=2;
                    break;
                case 2:
                k[choic].punjabi(b[choic]);
                EXIt=2;
                    break;
                case 3:
                k[choic].roti(b[choic]);
                EXIt=2;
                    break;
                case 4:
                cout<<"Enter quntuty:- ";
                cin>>qu;
                b[choic].addItem("Gujarati Thali         200", 200*qu,qu);
                EXIt=2;
                    break;
    
                case 5:
                cout<<"Enter quntuty:- ";
                cin>>qu;
                b[choic].addItem("Punjabi Thali          250", 250*qu,qu);
                EXIt=2;
                    break;
    
                case 6:  
                    EXIt=1;
                break;
                default:
    
                    break;
                }

                if(EXIt==1)
                {
                    break;
                }
    
            }
            

            break;
        case 2:

       for (int  i = 1; i <= 10; i++)
       {
        if (table[i]==1)
        {bi=1;}
       }
       
      
        if(bi==0)
        {
            cout<<"All table are empty!!"<<endl;
        }
        else{
            cout<<"start tables are :";
            for (int i = 1; i <= 10; i++)
            {
                if (table[i]==1)
                {
                    cout<<i<<" ";
                }
            }
            cout<<endl;
            cout<<"Enter seat no. :";
            int choic;
            cin>>choic;
            table[choic]=0;
            system("CLS");
            cout<<"\n1. online \n2. cash\n"<<endl;
            int ch;
            cin>>ch;
            system("CLS");
            b[choic].display();
            b[choic].saveToFile(choic,ch);
            b[choic].saveToMonthlyReport(choic,ch);


            for (int  i = 1; i <= 10; i++)
            {
                int s=0;
             if (table[i]==0) 
             { bi=0;}
             
            }
               
        }
       
    
            break;
        case 3:
        return;
        break;
        case 4:
        cout<<"Are you sure?          1.Yes  2.No"<<endl;
        int c;
        cin>>c;
        system("CLS");
        switch(c)
        {
            case 1: exit(0);

            case 2: cout<<"Okayyy!!"<<endl;
            continue;
        }

        default:
            break;
        }


    }
}
// Function to verify user PIN before accessing the system
void homepass()
{
    int pass = 9126, pin, count = 0;

    while (count < 3) // Allow 3 attempts
    {
        cout << "Enter 4-digit PIN: ";
        cin >> pin;
        system("CLS");
        if (pin == pass) return; // If correct, grant access
        else cout << "Incorrect PIN!\n";
        
        count++;
    }

    cout << "Too many incorrect attempts. Exiting...\n";
    exit(0); // Exit the program after 3 failed attempts
}


// Function to handle morning orders
void mornin()
{
    morning g[11]; // Array to store morning menu items for each table
    bill b[11];    // Array to store the bill for each table

    int table[11]={0}; // Array to track table availability (0 = free, 1 = occupied)
    int choicInputManu;    // User's menu choice
    int choic;     // User input for table number
    int bi = 0;    // Flag to check if any table is occupied

    while (true) // Infinite loop to keep the menu running
    {
        int EXIt = 0; // Control flag to exit inner loops
        cout << "welcome to jc's hotel" << endl;

        // Display menu options
        cout << "1. Take order" << endl;
        cout << "2. Bill" << endl;
        cout << "3. Back to timing" << endl;
        cout << "4. EXIT" << endl;
        cout << endl;
        cout << "Enter your choice:-  ";
        cin >> choicInputManu; // User input for menu option
        system("CLS");
        switch (choicInputManu)
        {
        case 1:
            if (EXIt == 0) // Ensure order taking is available
            {
                cout << "Free tables are :";
                for (int i = 1; i <= 10; i++) // Loop through tables
                {
                    if (table[i] == 0) // Check for unoccupied tables
                    {
                        cout << i << " ";
                    }
                }
                cout << endl;

                cout << "Occupied tables are :";
                for (int i = 1; i <= 10; i++) // Loop through tables
                {
                    if (table[i] == 1) // Check for occupied tables
                    {
                        cout << i << " ";
                    }
                }
                cout << endl;

                cout << "Enter table no. :";
                cin >> choic; // User input for table selection

                table[choic] = 1; // Mark table as occupied
                g[choic].mornig(b[choic]); // Call function to take the order
                system("CLS");
            }
            break;

        case 2: // Case to generate the bill
            for (int i = 1; i <= 10; i++) // Check if any table is occupied
            {
                if (table[i] == 1)
                {
                    bi = 1; // Set flag if at least one table is occupied
                }
            }

            if (bi == 0) // No occupied tables
            {
                cout << "All tables are empty!!" << endl;
            }
            else
            {
                cout << "Occupied tables are :";
                for (int i = 1; i <= 10; i++)
                {
                    if (table[i] == 1)
                    {
                        cout << i << " ";
                    }
                }
                cout << endl;
                cout << "Enter table no. :";
                cin >> choic; // User inputs table number
                table[choic] = 0; // Free up the table
                system("CLS");
                cout << "\n1. Online \n2. Cash\n" << endl;
                int ch;
                cin >> ch; // Payment method

                b[choic].display(); // Display the bill
                b[choic].saveToFile(choic, ch); // Save the bill details
                b[choic].saveToMonthlyReport(choic,ch);

                for (int i = 1; i <= 10; i++)
                {
                    if (table[i] == 0)
                    {
                        bi = 0; // Reset flag if all tables are free
                    }
                }
            }
            break;

        case 3:
            return; // Return to the previous menu
            break;

        case 4: // Exit confirmation
            cout << "Are you sure? 1.Yes  2.No" << endl;
            int c;
            cin >> c;
            switch (c)
            {
            case 1:
                exit(0); // Exit the program
            case 2:
                cout << "Okayyy!!" << endl;
                continue; // Return to menu
            }
            break;

        default:
            break;
        }
    }
}

// Function to handle night orders
void night()
{
    Night i[11]; // Array to store night menu items
    bill b[11];  // Array to store bill for each table

    int table[11]={0}; // Table tracking array
    int choicInputManu;    // User's menu choice
    int choic;     // User's table number choice
    int bi = 0;    // Flag for occupied tables

    while (true) // Infinite loop to keep the menu running
    {
        int EXIt = 0;
        cout << "Welcome to JC's Hotel" << endl;

        cout << "1. Take order" << endl;
        cout << "2. Bill" << endl;
        cout << "3. Back to timing" << endl;
        cout << "4. EXIT" << endl;
        cin >> choicInputManu;
        system("CLS");
        switch (choicInputManu)
        {
        case 1:
            while (true)
            {
                if (EXIt == 0)
                {
                    cout << "Free tables are :";
                    for (int i = 1; i <= 10; i++)
                    {
                        if (table[i] == 0)
                        {
                            cout << i << " ";
                        }
                    }
                    cout << endl;

                    cout << "Occupied tables are :";
                    for (int i = 1; i <= 10; i++)
                    {
                        if (table[i] == 1)
                        {
                            cout << i << " ";
                        }
                    }
                    cout << endl;

                    cout << "Enter seat no. :";
                    cin >> choic;
                    table[choic] = 1;
                }

                int forManuSelection;
                cout << "\n1 : Starter\n";
                cout << "\n2 : Main course\n";
                cout << "\n3 : Desert\n";
                cout << "\n4 : Cold drink\n";
                cout << "\n5 : Ice cream\n";
                cout << "\n6 : Quit\n";
                cin >> forManuSelection;
                system("CLS");

                switch (forManuSelection)
                {
                case 1:
                    i[choic].starter(b[choic]);
                    EXIt = 2;
                    break;
                case 2:
                    i[choic].maincourse(b[choic]);
                    EXIt = 2;
                    break;
                case 3:
                    i[choic].desert(b[choic]);
                    EXIt = 2;
                    break;
                case 4:
                    i[choic].coldDrink(b[choic]);
                    EXIt = 2;
                    break;
                case 5:
                    i[choic].icecream(b[choic]);
                    EXIt = 2;
                    break;
                case 6:
                    EXIt = 1;
                    break;
                default:
                    break;
                }

                if (EXIt == 1)
                {
                    break;
                }
            }
            break;

        case 2:
            for (int i = 1; i <= 10; i++)
            {
                if (table[i] == 1)
                {
                    bi = 1;
                }
            }

            if (bi == 0)
            {
                cout << "All tables are empty!!" << endl;
            }
            else
            {
                cout << "Occupied tables are :";
                for (int i = 1; i <= 10; i++)
                {
                    if (table[i] == 1)
                    {
                        cout << i << " ";
                    }
                }
                cout << endl;
                cout << "Enter seat no. :";
                cin >> choic;
                table[choic] = 0;
                system("CLS");

                cout << "\n1. Online \n2. Cash\n" << endl;
                int ch;
                cin >> ch;

                system("CLS");
                b[choic].display();
                b[choic].saveToFile(choic, ch);
                b[choic].saveToMonthlyReport(choic,ch);
                
            }
            break;

        case 3:
            return;
            break;

        case 4:
            cout << "Are you sure? 1.Yes  2.No" << endl;
            int c;
            cin >> c;
            system("CLS");
            switch (c)
            {
            case 1:
                exit(0);
            case 2:
                cout << "Okay!!" << endl;
                continue;
            }
            break;

        default:
            break;
        }
    }
}



// Function to get the current date as a string in YYYY-MM-DD format
string getCurrentDate() {
    time_t now = time(0); // Get the current system time
    tm *ltm = localtime(&now); // Convert time_t to a local time structure (tm)

    stringstream ss; // String stream to build the formatted date string
    ss << 1900 + ltm->tm_year << "-"; // Extract year (tm_year stores years since 1900)
    
    // Extract month and format with leading zero if necessary
    ss << (ltm->tm_mon + 1 < 10 ? "0" : "") << ltm->tm_mon + 1 << "-";
    
    // Extract day and format with leading zero if necessary
    ss << (ltm->tm_mday < 10 ? "0" : "") << ltm->tm_mday;

    return ss.str(); // Return the formatted date string
}


// Function to initialize the billing system and set up the billing file for the current date
void startUp() {
    int choicInputManu; // Variable to store user choice
    string currentDate = getCurrentDate(); // Get the current date as a string
    bill::billFilename = "bills_" + currentDate + ".txt"; // Set the billing filename using the current date

    // Display the welcome message and options for billing data management
    cout << "Welcome to JC's Hotel Billing System\n";
    cout << "1. Continue with Old Data (" << bill::billFilename << ")\n";
    cout << "2. Start Fresh (New File)\n";
    cout << "Enter your choice: ";
    cin >> choicInputManu; // Take user input for their choice
    system("CLS");
    if (choicInputManu == 2) { // If the user chooses to start fresh
        ofstream file(bill::billFilename, ios::app); // Open the file in append mode (creates if not exists)
        file.close(); // Close the file after creation
        
        cout << "New billing file created for " << currentDate << ".\n"; // Notify user about new file creation

        // Reset billing data for a fresh start
        bill::orderNo = 0; // Reset order number
        bill::dailyTotalOnCash = 0; // Reset daily total cash payments
        bill::dailyTotalOnOnline = 0; // Reset daily total online payments
    } else {
        // If the user chooses to continue with existing billing data
        cout << "Loading existing billing data for " << currentDate << "...\n"; // Notify user about loading old data
    }
}


int main()
{


homepass();

startUp();

 while (true)
 {int time;
    cout<<endl;
    cout<<endl;

    cout<<endl;

    cout<<"Enter Timing";

    cout<<"\n1:    Morning\n2:    afternoon\n3:    night\n";
    cin>>time;
    system("CLS");
    switch (time)
    {
    case 1:
        mornin();
        break;
    case 2:
        noon();
    break;
    case 3:
        night();
    break;
    default:
    cout<<"Incurrect....";
        break;
    }

 }

    return 0;
}



