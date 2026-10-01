#include <iostream>
#include <fstream>
#include <vector>
#include <iomanip>
#include <string>
#include <sstream>
#include <limits>
#include <ctime>
#include <conio.h>
#include <cctype>
#include <chrono>

using namespace std;


// ======================================================
//                    ACCOUNT CLASS
// ======================================================

class Account
{
private:
    string accountNumber;
    string name;
    string password;
    double balance;
    
    // Security
    int failedLoginAttempts;
    time_t lockUntil;

public:

    // Default constructor
    Account()
    {
        accountNumber = "";
        name = "";
        password = "";
        balance = 0.0;
        
        // Security variables
        failedLoginAttempts = 0;
        lockUntil = 0;
    }

    // Parameterized constructor
    Account(string accNo, string customerName,
            string pass, double bal = 0.0)
    {
        accountNumber = accNo;
        name = customerName;
        password = pass;
        balance = bal;
        
        // Security 
        failedLoginAttempts = 0;
        lockUntil = 0;
    }

    // Getters
    string getAccountNumber() const
    {
        return accountNumber;
    }

    string getName() const
    {
        return name;
    }

    string getPassword() const
    {
        return password;
    }

    double getBalance() const
    {
        return balance;
    }
    
    // Check whether account is currently locked
	bool isLocked() const
	{
	    return time(nullptr) < lockUntil;
	}
	
	// Get remaining lock time
	int getRemainingLockTime() const
	{
	    if (!isLocked())
	        return 0;
	
	    return static_cast<int>(lockUntil - time(nullptr));
	}
	
	// Record a failed login attempt
	void recordFailedLogin()
	{
	    failedLoginAttempts++;
	
	    if (failedLoginAttempts >= 3)
	    {
	        // Lock account for 60 seconds
	        lockUntil = time(nullptr) + 60;
	
	        // Reset counter after locking
	        failedLoginAttempts = 0;
	    }
	}
	
	// Reset login security after successful login
	void resetLoginAttempts()
	{
	    failedLoginAttempts = 0;
	    lockUntil = 0;
	}
	
    // Set password
    void setPassword(string newPassword)
    {
        password = newPassword;
    }

    // Deposit money
    void deposit(double amount)
    {
        balance += amount;
    }

    // Withdraw money
    bool withdraw(double amount)
    {
        if (amount > 0 && amount <= balance)
        {
            balance -= amount;
            return true;
        }

        return false;
    }

    // Display account details
    void displayDetails() const
    {
        cout << "\n========================================\n";
        cout << "           ACCOUNT DETAILS\n";
        cout << "========================================\n";

        cout << "Account Number : "
             << accountNumber << endl;

        cout << "Account Name   : "
             << name << endl;

        cout << fixed << setprecision(2);

        cout << "Balance        : GHS "
             << balance << endl;

        cout << "========================================\n";
    }
};


// ======================================================
//                  BANKING SYSTEM CLASS
// ======================================================

class BankingSystem
{
private:
// ===============================================
//               PASS WORD RULES
// ===============================================

	void displayPasswordRules()
{
    cout << "\nPassword Requirements:\n";
    cout << " - At least 8 characters\n";
    cout << " - At least one uppercase letter (A-Z)\n";
    cout << " - At least one lowercase letter (a-z)\n";
    cout << " - At least one number (0-9)\n";
    cout << " - At least one special character (!, @, #, $, etc.)\n";
}

// ===========================================
//          PASSWORD VALIDATION
// ===========================================

bool isStrongPassword(const string& password)
{
    bool hasUpper = false;
    bool hasLower = false;
    bool hasDigit = false;
    bool hasSpecial = false;

    if (password.length() < 8)
    {
        return false;
    }

    for (char ch : password)
    {
        if (isupper(ch))
        {
            hasUpper = true;
        }
        else if (islower(ch))
        {
            hasLower = true;
        }
        else if (isdigit(ch))
        {
            hasDigit = true;
        }
        else if (ispunct(ch))
        {
            hasSpecial = true;
        }
    }

    return hasUpper && hasLower && hasDigit && hasSpecial;
}

// ======================================
//         PASSWORD MASKING
// ======================================

string getPassword()
{
    string password;
    char ch;

    cout << "Password: ";

    while (true)
    {
        ch = _getch();

        // Enter key
        if (ch == 13)
        {
            cout << endl;
            break;
        }

        // Backspace key
        else if (ch == 8)
        {
            if (!password.empty())
            {
                password.pop_back();
                cout << "\b \b";
            }
        }

        // Normal character
        else
        {
            password += ch;
            cout << "*";
        }
    }

    return password;
}

    vector<Account> accounts;

    const string accountFile = "accounts.txt";
    const string transactionFile = "transactions.txt";


    // ==================================================
    //              INPUT VALIDATION
    // ==================================================

    int getValidChoice()
    {
        int choice;

        while (true)
        {
            cin >> choice;

            if (cin.fail())
            {
                cin.clear();

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );

                cout << "Invalid input. "
                     << "Please enter a number: ";
            }
            else
            {
                return choice;
            }
        }
    }


    double getValidAmount()
    {
        double amount;

        while (true)
        {
            cin >> amount;

            if (cin.fail())
            {
                cin.clear();

                cin.ignore(
                    numeric_limits<streamsize>::max(),
                    '\n'
                );

                cout << "Invalid amount. "
                     << "Enter a valid number: ";
            }
            else if (amount <= 0)
            {
                cout << "Amount must be greater "
                     << "than zero. Try again: ";
            }
            else
            {
                return amount;
            }
        }
    }


    // ==================================================
    //                FILE MANAGEMENT
    // ==================================================

    void loadAccounts()
    {
        ifstream file(accountFile);

        if (!file)
        {
            return;
        }

        accounts.clear();

        string line;

        while (getline(file, line))
        {
            if (line.empty())
            {
                continue;
            }

            string accountNumber;
            string name;
            string password;
            string balanceText;

            stringstream ss(line);

            getline(ss, accountNumber, '|');
            getline(ss, name, '|');
            getline(ss, password, '|');
            getline(ss, balanceText);

            try
            {
                double balance = stod(balanceText);

                accounts.push_back(
                    Account(
                        accountNumber,
                        name,
                        password,
                        balance
                    )
                );
            }
            catch (...)
            {
                cout << "Warning: Invalid account "
                     << "record skipped.\n";
            }
        }

        file.close();
    }


    void saveAccounts()
    {
        ofstream file(accountFile);

        if (!file)
        {
            cout << "Error: Unable to save "
                 << "account data.\n";

            return;
        }

        for (const Account& account : accounts)
        {
            file << account.getAccountNumber()
                 << "|"
                 << account.getName()
                 << "|"
                 << account.getPassword()
                 << "|"
                 << fixed << setprecision(2)
                 << account.getBalance()
                 << "\n";
        }

        file.close();
    }


    // ==================================================
    //                ACCOUNT SEARCH
    // ==================================================

    int findAccount(string accountNumber)
    {
        for (int i = 0; i < accounts.size(); i++)
        {
            if (accounts[i].getAccountNumber()
                == accountNumber)
            {
                return i;
            }
        }

        return -1;
    }


    // ==================================================
    //             TRANSACTION MANAGEMENT
    // ==================================================

    void recordTransaction(
        string accountNumber,
        string type,
        double amount,
        double balance)
    {
        ofstream file(
            transactionFile,
            ios::app
        );

        if (!file)
        {
            cout << "Error: Unable to record "
                 << "transaction.\n";

            return;
        }

        time_t now = time(0);

        string date = ctime(&now);

        if (!date.empty())
        {
            date.pop_back();
        }

        file << accountNumber
             << " | "
             << type
             << " | GHS "
             << fixed << setprecision(2)
             << amount
             << " | Balance: GHS "
             << balance
             << " | "
             << date
             << endl;

        file.close();
    }


    // ==================================================
    //               CUSTOMER OPERATIONS
    // ==================================================

    void deposit(int index)
    {
        cout << "\n========================================\n";
        cout << "             DEPOSIT MONEY\n";
        cout << "========================================\n";

        cout << "Enter deposit amount: GHS ";

        double amount = getValidAmount();

        accounts[index].deposit(amount);

        saveAccounts();

        recordTransaction(
            accounts[index].getAccountNumber(),
            "Deposit",
            amount,
            accounts[index].getBalance()
        );

        cout << "\nDeposit successful!\n";

        cout << fixed << setprecision(2);

        cout << "Amount Deposited: GHS "
             << amount << endl;

        cout << "New Balance: GHS "
             << accounts[index].getBalance()
             << endl;
    }


    void withdraw(int index)
    {
        cout << "\n========================================\n";
        cout << "            WITHDRAW MONEY\n";
        cout << "========================================\n";

        cout << "Enter withdrawal amount: GHS ";

        double amount = getValidAmount();

        if (amount > accounts[index].getBalance())
        {
            cout << "\nTransaction failed!\n";
            cout << "Reason: Insufficient funds.\n";

            cout << fixed << setprecision(2);

            cout << "Available Balance: GHS "
                 << accounts[index].getBalance()
                 << endl;

            return;
        }

        accounts[index].withdraw(amount);

        saveAccounts();

        recordTransaction(
            accounts[index].getAccountNumber(),
            "Withdrawal",
            amount,
            accounts[index].getBalance()
        );

        cout << "\nWithdrawal successful!\n";

        cout << fixed << setprecision(2);

        cout << "Amount Withdrawn: GHS "
             << amount << endl;

        cout << "New Balance: GHS "
             << accounts[index].getBalance()
             << endl;
    }


    void checkBalance(int index)
    {
        cout << "\n========================================\n";
        cout << "              BALANCE\n";
        cout << "========================================\n";

        cout << fixed << setprecision(2);

        cout << "Available Balance: GHS "
             << accounts[index].getBalance()
             << endl;

        cout << "========================================\n";
    }


    void transferMoney(int senderIndex)
    {
        string receiverAccount;

        cout << "\n========================================\n";
        cout << "             TRANSFER MONEY\n";
        cout << "========================================\n";

        cout << "Enter recipient account number: ";

        cin >> receiverAccount;

        int receiverIndex =
            findAccount(receiverAccount);

        if (receiverIndex == -1)
        {
            cout << "\nRecipient account not found!\n";
            return;
        }

        if (receiverIndex == senderIndex)
        {
            cout << "\nYou cannot transfer money "
                 << "to yourself!\n";

            return;
        }

        cout << "Enter transfer amount: GHS ";

        double amount = getValidAmount();

        if (amount >
            accounts[senderIndex].getBalance())
        {
            cout << "\nInsufficient funds!\n";
            return;
        }

        accounts[senderIndex].withdraw(amount);

        accounts[receiverIndex].deposit(amount);

        saveAccounts();

        recordTransaction(
            accounts[senderIndex].getAccountNumber(),
            "Transfer Sent",
            amount,
            accounts[senderIndex].getBalance()
        );

        recordTransaction(
            accounts[receiverIndex].getAccountNumber(),
            "Transfer Received",
            amount,
            accounts[receiverIndex].getBalance()
        );

        cout << "\n========================================\n";
        cout << "Transfer successful!\n";

        cout << "Recipient: "
             << accounts[receiverIndex].getName()
             << endl;

        cout << fixed << setprecision(2);

        cout << "Amount: GHS "
             << amount << endl;

        cout << "New Balance: GHS "
             << accounts[senderIndex].getBalance()
             << endl;

        cout << "========================================\n";
    }


    void transactionHistory(int index)
    {
        ifstream file(transactionFile);

        cout << "\n========================================\n";
        cout << "         TRANSACTION HISTORY\n";
        cout << "========================================\n";

        if (!file)
        {
            cout << "No transaction records found.\n";
            return;
        }

        string line;

        string accountNumber =
            accounts[index].getAccountNumber();

        bool found = false;

        while (getline(file, line))
        {
            if (line.rfind(
                    accountNumber + " | ", 0
                ) == 0)
            {
                cout << line << endl;

                found = true;
            }
        }

        if (!found)
        {
            cout << "No transactions found.\n";
        }

        cout << "========================================\n";

        file.close();
    }


    // --------------------------------------------------
   //     Secure Change Password
  // --------------------------------------------------

void changePassword(int index) {

    string oldPassword;
    string newPassword;
    string confirmPassword;

    cout << "\n========================================\n";
    cout << "           CHANGE PASSWORD\n";
    cout << "========================================\n";

    // Enter current password
    cout << "Enter current password: ";
    oldPassword = getPassword();

    // Verify current password
    if (oldPassword != accounts[index].getPassword()) {
        cout << "\nIncorrect current password!\n";
        cout << "Password was not changed.\n";
        cout << "========================================\n";
        return;
    }

    // Enter new password
    cout << "\nEnter new password: ";
    newPassword = getPassword();

    // Check minimum password length
    if (newPassword.length() < 4) {
        cout << "\nPassword must contain at least 4 characters.\n";
        cout << "Password was not changed.\n";
        cout << "========================================\n";
        return;
    }

    // Prevent using the same password
    if (newPassword == oldPassword) {
        cout << "\nNew password cannot be the same as your current password.\n";
        cout << "Please choose a different password.\n";
        cout << "========================================\n";
        return;
    }

    // Confirm new password
    cout << "\nConfirm new password: ";
    confirmPassword = getPassword();

    // Check whether passwords match
    if (newPassword != confirmPassword) {
        cout << "\nPasswords do not match!\n";
        cout << "Password was not changed.\n";
        cout << "========================================\n";
        return;
    }

    // Update password
    accounts[index].setPassword(newPassword);

    // Save updated account information
    saveAccounts();

    cout << "\n========================================\n";
    cout << "      PASSWORD CHANGED SUCCESSFULLY!\n";
    cout << "========================================\n";
}


    // ==================================================
    //                CUSTOMER MENU
    // ==================================================

    void customerMenu(int index)
    {
        int choice;

        do
        {
            cout << "\n========================================\n";
            cout << "        CUSTOMER BANKING PORTAL\n";
            cout << "========================================\n";

            cout << "Welcome, "
                 << accounts[index].getName()
                 << "!\n";

            cout << "----------------------------------------\n";

            cout << "1. Deposit Money\n";
            cout << "2. Withdraw Money\n";
            cout << "3. Check Balance\n";
            cout << "4. Transfer Money\n";
            cout << "5. Transaction History\n";
            cout << "6. Account Details\n";
            cout << "7. Change Password\n";
            cout << "8. Logout\n";

            cout << "----------------------------------------\n";

            cout << "Enter choice: ";

            choice = getValidChoice();

            switch (choice)
            {
                case 1:
                    deposit(index);
                    break;

                case 2:
                    withdraw(index);
                    break;

                case 3:
                    checkBalance(index);
                    break;

                case 4:
                    transferMoney(index);
                    break;

                case 5:
                    transactionHistory(index);
                    break;

                case 6:
                    accounts[index].displayDetails();
                    break;

                case 7:
                    changePassword(index);
                    break;

                case 8:
                    cout << "\nLogging out...\n";
                    break;

                default:
                    cout << "\nInvalid choice!\n";
            }

        } while (choice != 8);
    }


    // ==================================================
    //                  ADMIN FUNCTIONS
    // ==================================================

    void viewAllAccounts()
    {
        cout << "\n========================================\n";
        cout << "             ALL ACCOUNTS\n";
        cout << "========================================\n";

        if (accounts.empty())
        {
            cout << "No accounts available.\n";
            return;
        }

        cout << left
             << setw(15) << "Account No."
             << setw(25) << "Name"
             << "Balance"
             << endl;

        cout << "----------------------------------------\n";

        for (const Account& account : accounts)
        {
            cout << left
                 << setw(15)
                 << account.getAccountNumber()

                 << setw(25)
                 << account.getName()

                 << "GHS "
                 << fixed << setprecision(2)
                 << account.getBalance()
                 << endl;
        }

        cout << "========================================\n";
    }


    void searchAccount()
    {
        string accountNumber;

        cout << "\n========================================\n";
        cout << "             SEARCH ACCOUNT\n";
        cout << "========================================\n";

        cout << "Enter Account Number: ";

        cin >> accountNumber;

        int index =
            findAccount(accountNumber);

        if (index == -1)
        {
            cout << "\nAccount not found!\n";
            return;
        }

        accounts[index].displayDetails();
    }


    void viewTotalCustomers()
    {
        cout << "\n========================================\n";
        cout << "          TOTAL CUSTOMERS\n";
        cout << "========================================\n";

        cout << "Total registered customers: "
             << accounts.size()
             << endl;

        cout << "========================================\n";
    }


    void viewTotalBalance()
    {
        double totalBalance = 0.0;

        for (const Account& account : accounts)
        {
            totalBalance += account.getBalance();
        }

        cout << "\n========================================\n";
        cout << "          TOTAL BANK BALANCE\n";
        cout << "========================================\n";

        cout << fixed << setprecision(2);

        cout << "Total money in customer accounts: GHS "
             << totalBalance
             << endl;

        cout << "========================================\n";
    }


    void viewAllTransactions()
    {
        ifstream file(transactionFile);

        cout << "\n========================================\n";
        cout << "          ALL TRANSACTIONS\n";
        cout << "========================================\n";

        if (!file)
        {
            cout << "No transaction records found.\n";
            return;
        }

        string line;

        bool found = false;

        while (getline(file, line))
        {
            cout << line << endl;
            found = true;
        }

        if (!found)
        {
            cout << "No transactions available.\n";
        }

        cout << "========================================\n";

        file.close();
    }


    void deleteAccount()
    {
        string accountNumber;

        cout << "\n========================================\n";
        cout << "             DELETE ACCOUNT\n";
        cout << "========================================\n";

        cout << "Enter Account Number: ";

        cin >> accountNumber;

        int index =
            findAccount(accountNumber);

        if (index == -1)
        {
            cout << "\nAccount not found!\n";
            return;
        }

        cout << "\nAccount Holder: "
             << accounts[index].getName()
             << endl;

        cout << fixed << setprecision(2);

        cout << "Current Balance: GHS "
             << accounts[index].getBalance()
             << endl;

        char confirmation;

        cout << "\nAre you sure you want "
             << "to delete this account? (Y/N): ";

        cin >> confirmation;

        if (confirmation == 'Y' ||
            confirmation == 'y')
        {
            accounts.erase(
                accounts.begin() + index
            );

            saveAccounts();

            cout << "\nAccount deleted successfully!\n";
        }
        else
        {
            cout << "\nAccount deletion cancelled.\n";
        }
    }


    // ==================================================
    //                  ADMIN LOGIN
    // ==================================================

    // --------------------------------------------------
    //              Secure Admin Login
   // --------------------------------------------------

void adminLogin()
{
    string username;
    string password;

    const string adminUsername = "bankadmin";
    const string adminPassword = "Bank@2026Secure";

    const int maxAttempts = 3;
    int attempts = 0;

    cout << "\n========================================\n";
    cout << "              ADMIN LOGIN\n";
    cout << "========================================\n";

    while (attempts < maxAttempts)
    {
        cout << "\nUsername: ";
        cin >> username;

        password = getPassword();

        if (username == adminUsername &&
            password == adminPassword)
        {
            cout << "\n========================================\n";
            cout << "       ADMIN LOGIN SUCCESSFUL!\n";
            cout << "========================================\n";

            adminMenu();

            return;
        }

        attempts++;

        cout << "\nInvalid username or password!\n";

        if (attempts < maxAttempts)
        {
            cout << "Attempts remaining: "
                 << maxAttempts - attempts
                 << endl;
        }
    }

    cout << "\n========================================\n";
    cout << "       ADMIN ACCESS BLOCKED!\n";
    cout << "========================================\n";
    cout << "Maximum login attempts exceeded.\n";
    cout << "Please try again later.\n";
}

    // ==================================================
    //                  ADMIN MENU
    // ==================================================

    void adminMenu()
    {
        int choice;

        do
        {
            cout << "\n========================================\n";
            cout << "            ADMIN DASHBOARD\n";
            cout << "========================================\n";

            cout << "1. View All Accounts\n";
            cout << "2. Search Account\n";
            cout << "3. View Total Customers\n";
            cout << "4. View Total Bank Balance\n";
            cout << "5. View All Transactions\n";
            cout << "6. Delete Account\n";
            cout << "7. Logout\n";

            cout << "----------------------------------------\n";

            cout << "Enter choice: ";

            choice = getValidChoice();

            switch (choice)
            {
                case 1:
                    viewAllAccounts();
                    break;

                case 2:
                    searchAccount();
                    break;

                case 3:
                    viewTotalCustomers();
                    break;

                case 4:
                    viewTotalBalance();
                    break;

                case 5:
                    viewAllTransactions();
                    break;

                case 6:
                    deleteAccount();
                    break;

                case 7:
                    cout << "\nAdmin logging out...\n";
                    break;

                default:
                    cout << "\nInvalid choice!\n";
            }

        } while (choice != 7);
    }


    // ==================================================
    //                 CREATE ACCOUNT
    // ==================================================

    void createAccount()
    {
        string accountNumber;
        string name;
        string password;

        cout << "\n========================================\n";
        cout << "             CREATE ACCOUNT\n";
        cout << "========================================\n";

        cout << "Enter Account Number: ";

        cin >> accountNumber;

        if (findAccount(accountNumber) != -1)
        {
            cout << "\nAccount number already exists!\n";
            return;
        }

        cout << "Enter Full Name: ";

        cin.ignore(
            numeric_limits<streamsize>::max(),
            '\n'
        );

        getline(cin, name);

        if (name.empty())
        {
            cout << "\nName cannot be empty!\n";
            return;
        }
        
        // ===========================================
        //            PASSWORD SECURITY
        // ===========================================
        displayPasswordRules();

		while (true)
		{
		    password = getPassword();
		
		    if (!isStrongPassword(password))
		    {
		        cout << "\nWeak Password!\n";
		        cout << "Your password does not meet the security requirements.\n";
		        cout << "Please try again.\n";
		        continue;
		    }
		
		    break;
		}
		
		// Confirm password
		
		string confirmPassword;

		cout << "\nConfirm Password: ";
		confirmPassword = getPassword();
		
		if (password != confirmPassword)
		{
		    cout << "\nPasswords do not match!\n";
		    cout << "Account creation cancelled.\n";
		    return;
		}
		
		// Create new account
		
        Account newAccount(
            accountNumber,
            name,
            password,
            0.0
        );

        accounts.push_back(newAccount);

        saveAccounts();

        cout << "\n========================================\n";
        cout << "Account created successfully!\n";
        cout << "Account Number: "
             << accountNumber << endl;
        cout << "========================================\n";
    }


    // ==================================================
    //                    CUSTOMER LOGIN
    // ==================================================

    void login()
	{
	    string accountNumber;
	    string password;
	
	    cout << "\n========================================\n";
	    cout << "                LOGIN\n";
	    cout << "========================================\n";
	
	    cout << "\nAccount Number: ";
	    cin >> accountNumber;
	
	    int index = findAccount(accountNumber);
	
	    if (index == -1)
	    {
	        cout << "\nAccount not found!\n";
	        return;
	    }
	
	    // Check if account is locked
	    if (accounts[index].isLocked())
	    {
	        cout << "\n========================================\n";
	        cout << "          ACCOUNT TEMPORARILY LOCKED\n";
	        cout << "========================================\n";
	
	        cout << "Too many failed login attempts.\n";
	        cout << "Please wait "
	             << accounts[index].getRemainingLockTime()
	             << " seconds before trying again.\n";
	
	        return;
	    }
	
	    const int MAX_ATTEMPTS = 3;
	
	    for (int attempt = 1; attempt <= MAX_ATTEMPTS; attempt++)
	    {
	        password = getPassword();
	
	        if (accounts[index].getPassword() == password)
	        {
	            // Successful login
	            accounts[index].resetLoginAttempts();
	
	            cout << "\n========================================\n";
	            cout << "           LOGIN SUCCESSFUL!\n";
	            cout << "========================================\n";
	
	            customerMenu(index);
	
	            return;
	        }
	
	        // Incorrect password
	        accounts[index].recordFailedLogin();
	
	        cout << "\nIncorrect password!\n";
	
	        if (accounts[index].isLocked())
	        {
	            cout << "\n========================================\n";
	            cout << "          ACCOUNT TEMPORARILY LOCKED\n";
	            cout << "========================================\n";
	
	            cout << "Too many failed login attempts.\n";
	            cout << "Please wait 60 seconds before trying again.\n";
	
	            return;
	        }
	
	        if (attempt < MAX_ATTEMPTS)
	        {
	            cout << "Attempts remaining: "
	                 << MAX_ATTEMPTS - attempt
	                 << endl;
	        }
	    }
	}
	
public:

    // ==================================================
    //                CONSTRUCTOR
    // ==================================================

    BankingSystem()
    {
        loadAccounts();
    }


    // ==================================================
    //                 MAIN PROGRAM
    // ==================================================

    void run()
    {
        int choice;

        do
        {
            cout << "\n\n";
            cout << "============================================\n";
            cout << "          BANKING MANAGEMENT SYSTEM\n";
            cout << "============================================\n";
            cout << "          Secure, Simple And Reliable\n";
            cout << "--------------------------------------------\n";

            cout << "1. Create Account\n";
            cout << "2. Customer Login\n";
            cout << "3. Admin Login\n";
            cout << "4. Exit\n";

            cout << "--------------------------------------------\n";

            cout << "Enter choice: ";

            choice = getValidChoice();

            switch (choice)
            {
                case 1:
                    createAccount();
                    break;

                case 2:
                    login();
                    break;

                case 3:
                    adminLogin();
                    break;

                case 4:
                    cout << "\nThank you for using "
                         << "our Banking System!\n";

                    cout << "Goodbye!\n";

                    break;

                default:
                    cout << "\nInvalid choice! "
                         << "Please try again.\n";
            }

        } while (choice != 4);
    }
};


// ======================================================
//                       MAIN
// ======================================================

int main()
{
    BankingSystem bank;

    bank.run();

    return 0;
}