#include <iostream>
#include <fstream>
#include <string>
#include <unordered_map>
#include <iomanip>

using namespace std;

class Customer {
public:
    Customer(int customer_number, const string& customer_name, double balance_due)
        : customer_number(customer_number), customer_name(customer_name), balance_due(balance_due) {}

    void processOrder(const string& item_ordered, int quantity_ordered, double cost, int discount) {
        double total_cost = quantity_ordered * cost;
        if (discount > 0) {
            total_cost -= (total_cost * discount / 100);
        }
        balance_due += total_cost;
    }

    void processPayment(double payment_amount, int early_payment_discount) {
        if (early_payment_discount > 0) {
            payment_amount -= (balance_due * early_payment_discount / 100);
        }
        balance_due -= payment_amount;
    }

    void generateInvoice() {
        cout << "Customer Name: " << customer_name << endl;
        cout << "Customer Number: " << customer_number << endl << endl;
        cout << "Previous Balance: $" << fixed << setprecision(2) << balance_due << endl << endl;
    }

private:
    int customer_number;
    string customer_name;
    double balance_due;
};

int main() {
    unordered_map<int, Customer> customers;

    // Read and process the master file
    ifstream masterFile("master_file.txt");
    if (!masterFile.is_open()) {
        cerr << "Error: Unable to open master_file.txt" << endl;
        return 1;
    }

    int customer_number;
    string customer_name;
    double balance_due;
    while (masterFile >> customer_number >> customer_name >> balance_due) {
        customers[customer_number] = Customer(customer_number, customer_name, balance_due);
    }
    masterFile.close();

    // Read and process the transaction file
    ifstream transactionFile("transaction_file.txt");
    if (!transactionFile.is_open()) {
        cerr << "Error: Unable to open transaction_file.txt" << endl;
        return 1;
    }

    int current_customer_number = -1;
    while (transactionFile >> ws) {
        char transaction_type;
        transactionFile >> transaction_type;

        int customer_number;
        transactionFile >> customer_number;

        if (customers.find(customer_number) == customers.end()) {
            cerr << "Error: Customer number " << customer_number << " not found in the master file." << endl;
            continue;
        }

        Customer& current_customer = customers[customer_number];

        if (current_customer_number != customer_number) {
            if (current_customer_number != -1) {
                current_customer.generateInvoice();
            }
            current_customer_number = customer_number;
        }

        if (transaction_type == 'O') {
            string item_ordered;
            int quantity_ordered, discount;
            double cost;
            transactionFile >> item_ordered >> quantity_ordered >> cost >> discount;
            current_customer.processOrder(item_ordered, quantity_ordered, cost, discount);
        }
        else if (transaction_type == 'P') {
            double payment_amount;
            int early_payment_discount;
            transactionFile >> payment_amount >> early_payment_discount;
            current_customer.processPayment(payment_amount, early_payment_discount);
        }
    }

    // Print the last invoice
    if (current_customer_number != -1) {
        customers[current_customer_number].generateInvoice();
    }

    // Calculate the total A/R balance of ABC Company
    double total_ar_balance = 0.0;
    for (const auto& pair : customers) {
        total_ar_balance += pair.second.generateInvoice();
    }

    cout << "Total A/R Balance of ABC Company: $" << fixed << setprecision(2) << total_ar_balance << endl;

    transactionFile.close();
    return 0;
}
