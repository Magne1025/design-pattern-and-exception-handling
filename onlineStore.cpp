#include <iostream>
#include <fstream>
#include <cstring>
#include <iomanip>
#include <limits>
#include <string>
#include <cctype>
#include <sstream>
#include <climits>
using namespace std;

// ==================== CONSTANTS ====================
const int MAX_PRODUCTS = 50;
const int MAX_CART_ITEMS = 100;
const int MAX_ORDERS = 100;
const int MAX_PRODUCT_NAME_LENGTH = 50;
const int MAX_PAYMENT_METHOD_LENGTH = 50;

// ==================== EXCEPTIONS ====================
class InvalidInputException : public exception {
public:
    const char* what() const throw() {
        return "Invalid input! Please enter a valid product code.";
    }
};

class ProductNotFoundException : public exception {
public:
    const char* what() const throw() {
        return "Product code not found.";
    }
};

class EmptyCartException : public exception {
public:
    const char* what() const throw() {
        return "Shopping cart is empty.";
    }
};

// ==================== INPUT VALIDATION ====================
char getYesNoInput(const string& prompt) {
    string input;
    bool valid = false;
    char result = '\0';
    
    while (!valid) {
        cout << prompt;
        getline(cin, input);
        
        // Trim whitespace
        size_t start = input.find_first_not_of(" \t\n\r\f\v");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t\n\r\f\v");
            input = input.substr(start, end - start + 1);
        } else {
            input = "";
        }
        
        if (input.length() == 1 && (toupper(input[0]) == 'Y' || toupper(input[0]) == 'N')) {
            result = toupper(input[0]);
            valid = true;
        } else {
            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║          ❗ INVALID INPUT                  ║\n";
            cout << "║ Please enter Y or N                       ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
        }
    }
    return result;
}

string getProductCodeInput(const string& prompt, const string productCodes[], int productCount) {
    string input;
    bool valid = false;
    
    while (!valid) {
        cout << prompt;
        getline(cin, input);
        
        // Convert to uppercase and trim
        for (char &c : input) {
            c = toupper(c);
        }
        
        size_t start = input.find_first_not_of(" \t\n\r\f\v");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t\n\r\f\v");
            input = input.substr(start, end - start + 1);
        } else {
            input = "";
        }
        
        if (input == "0") {
            return input;
        } else if (input.length() == 3) {
            for (int i = 0; i < productCount; i++) {
                if (input == productCodes[i]) {
                    valid = true;
                    break;
                }
            }
            if (!valid) {
                cout << "╔════════════════════════════════════════════╗\n";
                cout << "║          ❗ PRODUCT NOT FOUND             ║\n";
                cout << "║ Please enter a valid 3-letter product code║\n";
                cout << "╚════════════════════════════════════════════╝\n";
            }
        } else {
            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║          ❗ INVALID INPUT                  ║\n";
            cout << "║ Product codes must be exactly 3 letters   ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
        }
    }
    return input;
}

template<typename T>
T getValidInput(const string& prompt, T min = numeric_limits<T>::min(), T max = numeric_limits<T>::max()) {
    string input;
    T value;
    bool valid = false;
    
    while (!valid) {
        cout << prompt;
        getline(cin, input);
        
        // Trim whitespace
        size_t start = input.find_first_not_of(" \t\n\r\f\v");
        if (start != string::npos) {
            size_t end = input.find_last_not_of(" \t\n\r\f\v");
            input = input.substr(start, end - start + 1);
        } else {
            input = "";
        }
        
        if (input.find(' ') != string::npos) {
            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║          ❗ INVALID INPUT                  ║\n";
            cout << "║ No spaces allowed. Enter a single number.  ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            continue;
        }
        
        istringstream iss(input);
        if (iss >> value && value >= min && value <= max) {
            string remaining;
            if (!(iss >> remaining)) {
                valid = true;
            } else {
                cout << "╔════════════════════════════════════════════╗\n";
                cout << "║          ❗ INVALID INPUT                  ║\n";
                cout << "║ No extra characters allowed.              ║\n";
                cout << "╚════════════════════════════════════════════╝\n";
            }
        } else {
            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║          ❗ INVALID INPUT                  ║\n";
            cout << "║ Please enter a positive number            ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
        }
    }
    return value;
}

// ==================== PRODUCT CLASS ====================
class Product {
private:
    string id;
    char name[MAX_PRODUCT_NAME_LENGTH];
    float price;

public:
    Product() : price(0.0f) {
        id = "";
        name[0] = '\0';
    }

    Product(const string& id, const char* name, float price) : id(id), price(price) {
        strncpy(this->name, name, MAX_PRODUCT_NAME_LENGTH - 1);
        this->name[MAX_PRODUCT_NAME_LENGTH - 1] = '\0';
    }

    const string& getId() const { return id; }
    const char* getName() const { return name; }
    float getPrice() const { return price; }

    void display() const {
        cout << "║ " << left << setw(8) << id << " ║ "
             << setw(20) << name << " ║ "
             << right << setw(10) << fixed << setprecision(2) << price << " ║\n";
    }
};

// ==================== CART ITEM CLASS ====================
class CartItem {
private:
    Product product;
    int quantity;

public:
    CartItem() : quantity(0) {}
    CartItem(const Product& p, int qty) : product(p), quantity(qty) {}

    const Product& getProduct() const { return product; }
    int getQuantity() const { return quantity; }
    void setQuantity(int qty) { quantity = qty; }

    void display() const {
        cout << "║ " << left << setw(8) << product.getId() << " ║ "
             << setw(20) << product.getName() << " ║ "
             << right << setw(10) << product.getPrice() << " ║ "
             << setw(8) << quantity << " ║\n";
    }
};

// ==================== PAYMENT STRATEGY ====================
class PaymentStrategy {
public:
    virtual ~PaymentStrategy() {}
    virtual void pay(float amount) = 0;
    virtual const char* getMethodName() const = 0;
};

class CashPayment : public PaymentStrategy {
public:
    void pay(float amount) override {
        cout << "╔════════════════════════════════════════════╗\n";
        cout << "║          💵 PAYMENT SUCCESSFUL!           ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  Method: Cash                              ║\n";
        cout << "║  Amount: PHP " << left << setw(30) << fixed << setprecision(2) << amount << "║\n";
        cout << "╚════════════════════════════════════════════╝\n";
    }
    const char* getMethodName() const override { return "Cash"; }
};

class CardPayment : public PaymentStrategy {
public:
    void pay(float amount) override {
        cout << "╔════════════════════════════════════════════╗\n";
        cout << "║          💳 PAYMENT SUCCESSFUL!           ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  Method: Credit/Debit Card                 ║\n";
        cout << "║  Amount: PHP " << left << setw(30) << fixed << setprecision(2) << amount << "║\n";
        cout << "╚════════════════════════════════════════════╝\n";
    }
    const char* getMethodName() const override { return "Credit/Debit Card"; }
};

class GCashPayment : public PaymentStrategy {
public:
    void pay(float amount) override {
        cout << "╔════════════════════════════════════════════╗\n";
        cout << "║          📱 PAYMENT SUCCESSFUL!           ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  Method: GCash                             ║\n";
        cout << "║  Amount: PHP " << left << setw(30) << fixed << setprecision(2) << amount << "║\n";
        cout << "╚════════════════════════════════════════════╝\n";
    }
    const char* getMethodName() const override { return "GCash"; }
};

// ==================== ORDER LOGGER ====================
class OrderLogger {
private:
    static OrderLogger* instance;
    ofstream logFile;

    OrderLogger() {
        logFile.open("orders.log", ios::app);
        if (!logFile.is_open()) {
            throw runtime_error("Failed to open log file.");
        }
    }

public:
    static OrderLogger* getInstance() {
        if (!instance) {
            instance = new OrderLogger();
        }
        return instance;
    }

    void logOrder(int orderId, const char* paymentMethod) {
        if (logFile.is_open()) {
            logFile << "[ORDER #" << orderId << "] Paid with " << paymentMethod << "\n";
            logFile.flush();
        }
    }

    ~OrderLogger() {
        if (logFile.is_open()) {
            logFile.close();
        }
    }
};

OrderLogger* OrderLogger::instance = nullptr;

// ==================== SHOPPING CART ====================
class ShoppingCart {
private:
    CartItem items[MAX_CART_ITEMS];
    int itemCount;

public:
    ShoppingCart() : itemCount(0) {}

    int getItemCount() const { return itemCount; }
    const CartItem* getItems() const { return items; }

    void addProduct(const Product& product, int quantity = 1) {
        if (quantity <= 0) {
            throw invalid_argument("Quantity must be positive");
        }

        bool found = false;
        for (int i = 0; i < itemCount && !found; i++) {
            if (items[i].getProduct().getId() == product.getId()) {
                if (items[i].getQuantity() > INT_MAX - quantity) {
                    throw overflow_error("Quantity would exceed maximum value");
                }
                items[i].setQuantity(items[i].getQuantity() + quantity);
                found = true;
            }
        }

        if (!found) {
            if (itemCount >= MAX_CART_ITEMS) {
                throw runtime_error("Shopping cart is full.");
            }
            items[itemCount] = CartItem(product, quantity);
            itemCount++;
        }
    }

    float calculateTotal() const {
        float total = 0.0f;
        for (int i = 0; i < itemCount; i++) {
            total += items[i].getProduct().getPrice() * items[i].getQuantity();
        }
        return total;
    }

    void display() const {
        if (itemCount == 0) {
            throw EmptyCartException();
        }

        cout << "╔══════════╦══════════════════════╦════════════╦══════════╗\n";
        cout << "║   ID     ║        Name          ║   Price    ║  Qty     ║\n";
        cout << "╠══════════╬══════════════════════╬════════════╬══════════╣\n";
        
        for (int i = 0; i < itemCount; i++) {
            items[i].display();
        }
        
        cout << "╠════════════════════════════════╬════════════╬══════════╣\n";
        cout << "║            TOTAL               ║ "
             << right << setw(10) << fixed << setprecision(2) << calculateTotal() 
             << " ║          ║\n";
        cout << "╚════════════════════════════════╩════════════╩══════════╝\n";
    }

    void clear() {
        itemCount = 0;
    }
};

// ==================== ORDER CLASS ====================
class Order {
private:
    static int nextOrderId;
    int orderId;
    float totalAmount;
    char paymentMethod[MAX_PAYMENT_METHOD_LENGTH];
    ShoppingCart cart;

public:
    Order(const ShoppingCart& cart, PaymentStrategy* paymentStrategy) : cart(cart) {
        orderId = nextOrderId++;
        totalAmount = cart.calculateTotal();
        strncpy(paymentMethod, paymentStrategy->getMethodName(), MAX_PAYMENT_METHOD_LENGTH - 1);
        paymentMethod[MAX_PAYMENT_METHOD_LENGTH - 1] = '\0';
    }

    void display() const {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║               ORDER SUMMARY                ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  Order ID: " << left << setw(33) << orderId << "║\n";
        cout << "║  Total Amount: PHP " << setw(25) << fixed << setprecision(2) << totalAmount << "║\n";
        cout << "║  Payment Method: " << setw(27) << paymentMethod << "║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║            ORDER DETAILS                   ║\n";
        cout << "╠══════════╦══════════════════════╦════════════╦══════════╗\n";
        cout << "║   ID     ║        Name          ║   Price    ║  Qty     ║\n";
        cout << "╠══════════╬══════════════════════╬════════════╬══════════╣\n";
        
        const CartItem* items = cart.getItems();
        for (int i = 0; i < cart.getItemCount(); i++) {
            items[i].display();
        }
        
        cout << "╚══════════╩══════════════════════╩════════════╩══════════╝\n";
    }

    int getOrderId() const { return orderId; }
    const char* getPaymentMethod() const { return paymentMethod; }
};

int Order::nextOrderId = 1;

// ==================== PRODUCT CATALOG ====================
class ProductCatalog {
private:
    Product products[MAX_PRODUCTS];
    string productCodes[MAX_PRODUCTS];
    int productCount;

public:
    ProductCatalog() : productCount(0) {
        // Original products
        addProduct(Product("LAP", "Laptop", 50000));
        addProduct(Product("PHN", "Smartphone", 20000));
        addProduct(Product("HDP", "Headphones", 3000));
        addProduct(Product("KEY", "Keyboard", 1500));
        addProduct(Product("MOU", "Mouse", 800));
        
        // New additional products
        addProduct(Product("MON", "Monitor", 12000));
        addProduct(Product("TAB", "Tablet", 15000));
        addProduct(Product("SPK", "Bluetooth Speaker", 2500));
        addProduct(Product("POW", "Power Bank", 1800));
        addProduct(Product("USB", "USB Flash Drive", 500));
        addProduct(Product("HDD", "External Hard Drive", 4000));
    }

    void addProduct(const Product& product) {
        if (productCount < MAX_PRODUCTS) {
            products[productCount] = product;
            productCodes[productCount] = product.getId();
            productCount++;
        }
    }

    const Product* getProducts() const { return products; }
    int getProductCount() const { return productCount; }
    const string* getProductCodes() const { return productCodes; }

    const Product& findProductById(const string& id) const {
        for (int i = 0; i < productCount; i++) {
            if (products[i].getId() == id) {
                return products[i];
            }
        }
        throw ProductNotFoundException();
    }

    void displayProducts() const {
        cout << "╔══════════╦══════════════════════╦════════════╗\n";
        cout << "║   ID     ║        Name          ║   Price    ║\n";
        cout << "╠══════════╬══════════════════════╬════════════╣\n";
        
        for (int i = 0; i < productCount; i++) {
            products[i].display();
        }
        
        cout << "╚══════════╩══════════════════════╩════════════╝\n";
    }
};

// ==================== ECOMMERCE SYSTEM ====================
class ECommerceSystem {
private:
    ProductCatalog catalog;
    ShoppingCart cart;
    Order* orders[MAX_ORDERS];
    int orderCount;

    void displayMainMenu() {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║          🛒 E-COMMERCE SYSTEM MENU         ║\n";
        cout << "╠════════════════════════════════════════════╣\n";
        cout << "║  1. 🏷️  View Products                      ║\n";
        cout << "║  2. 🛍️  View Shopping Cart                 ║\n";
        cout << "║  3. 📋 View Orders                         ║\n";
        cout << "║  4. 🚪 Exit                                ║\n";
        cout << "╚════════════════════════════════════════════╝\n";
    }

    void handleViewProducts() {
        char choice = 'Y';
        do {
            catalog.displayProducts();
            
            try {
                string productCode = getProductCodeInput(
                    "\n╔════════════════════════════════════════════╗\n"
                    "║ Enter Product Code to add to cart (0 to back)║\n"
                    "╚════════════════════════════════════════════╝\n"
                    "➡ Product Code: ",
                    catalog.getProductCodes(),
                    catalog.getProductCount()
                );
                
                if (productCode == "0") return;

                const Product& product = catalog.findProductById(productCode);
                
                int quantity = getValidInput(
                    "╔════════════════════════════════════════════╗\n"
                    "║        Enter quantity:                     ║\n"
                    "╚════════════════════════════════════════════╝\n"
                    "➡ Quantity: ",
                    1
                );

                cart.addProduct(product, quantity);
                
                cout << "╔════════════════════════════════════════════╗\n";
                cout << "║          ✔ Product added to cart!          ║\n";
                cout << "║         Quantity: " << left << setw(22) << quantity << "║\n";
                cout << "╚════════════════════════════════════════════╝\n";
            } catch (const exception& e) {
                cout << "╔════════════════════════════════════════════╗\n";
                cout << "║               ❗ ERROR:                     ║\n";
                cout << "║ " << left << setw(40) << e.what() << " ║\n";
                cout << "╚════════════════════════════════════════════╝\n";
            }

            choice = getYesNoInput("➡ Add another product? (Y/N): ");
        } while (choice == 'Y');
    }

    void handleViewCart() {
        try {
            cart.display();
            
            char choice = getYesNoInput(
                "╔════════════════════════════════════════════╗\n"
                "║        Proceed to checkout? (Y/N)          ║\n"
                "╚════════════════════════════════════════════╝\n"
                "➡ Choice: "
            );
            
            if (choice == 'Y') {
                checkout();
            }
        } catch (const EmptyCartException& e) {
            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║              🛒 CART EMPTY                 ║\n";
            cout << "║ " << left << setw(40) << "Add products before checking out!" << " ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
        }
    }

    PaymentStrategy* getPaymentStrategy(int choice) {
        switch (choice) {
            case 1: return new CashPayment();
            case 2: return new CardPayment();
            case 3: return new GCashPayment();
            default: throw InvalidInputException();
        }
    }

    void checkout() {
        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║           🏁 CHECKOUT SUMMARY             ║\n";
        cout << "╚════════════════════════════════════════════╝\n";
        cart.display();

        int paymentChoice = getValidInput(
            "╔════════════════════════════════════════════╗\n"
            "║           💳 SELECT PAYMENT METHOD        ║\n"
            "╠════════════════════════════════════════════╣\n"
            "║  1. 💵 Cash                               ║\n"
            "║  2. 💳 Credit/Debit Card                  ║\n"
            "║  3. 📱 GCash                              ║\n"
            "╚════════════════════════════════════════════╝\n"
            "➡ Your choice: ",
            1, 3
        );

        PaymentStrategy* paymentStrategy = getPaymentStrategy(paymentChoice);
        paymentStrategy->pay(cart.calculateTotal());

        if (orderCount < MAX_ORDERS) {
            orders[orderCount] = new Order(cart, paymentStrategy);
            orderCount++;

            OrderLogger::getInstance()->logOrder(
                orders[orderCount-1]->getOrderId(),
                orders[orderCount-1]->getPaymentMethod()
            );

            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║          [LOG] -> Order ID: " << left << setw(16) << orders[orderCount-1]->getOrderId() << "║\n";
            cout << "║   has been successfully checked out        ║\n";
            cout << "║   and paid using " << left << setw(25) << orders[orderCount-1]->getPaymentMethod() << "║\n";
            cout << "╚════════════════════════════════════════════╝\n";

            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║          🎉 ORDER COMPLETED!              ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
        } else {
            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║          ❗ MAX ORDERS REACHED            ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
        }

        cart.clear();
        delete paymentStrategy;
    }

    void handleViewOrders() {
        if (orderCount == 0) {
            cout << "╔════════════════════════════════════════════╗\n";
            cout << "║           📭 NO ORDERS YET               ║\n";
            cout << "╚════════════════════════════════════════════╝\n";
            return;
        }

        cout << "\n╔════════════════════════════════════════════╗\n";
        cout << "║              ORDER HISTORY                ║\n";
        cout << "╚════════════════════════════════════════════╝\n";

        for (int i = 0; i < orderCount; i++) {
            orders[i]->display();
        }

        cout << "╔════════════════════════════════════════════╗\n";
        cout << "║           END OF ORDER HISTORY            ║\n";
        cout << "╚════════════════════════════════════════════╝\n";
    }

public:
    ECommerceSystem() : orderCount(0) {
        for (int i = 0; i < MAX_ORDERS; i++) {
            orders[i] = nullptr;
        }
    }

    ~ECommerceSystem() {
        for (int i = 0; i < orderCount; i++) {
            delete orders[i];
        }
    }

    void run() {
        int choice = 0;
        bool exitProgram = false;
        
        while (!exitProgram) {
            try {
                displayMainMenu();
                choice = getValidInput("➡ Enter your choice (1-4): ", 1, 4);

                switch (choice) {
                    case 1: handleViewProducts(); break;
                    case 2: handleViewCart(); break;
                    case 3: handleViewOrders(); break;
                    case 4: 
                        cout << "╔════════════════════════════════════════════╗\n";
                        cout << "║          🛑 THANK YOU FOR SHOPPING!       ║\n";
                        cout << "╚════════════════════════════════════════════╝\n";
                        exitProgram = true;
                        break;
                }
            } catch (const exception& e) {
                cout << "╔════════════════════════════════════════════╗\n";
                cout << "║               ❗ ERROR:                     ║\n";
                cout << "║ " << left << setw(40) << e.what() << " ║\n";
                cout << "╚════════════════════════════════════════════╝\n";
            }
        }
    }
};

// ==================== MAIN FUNCTION ====================
int main() {
    try {
        ECommerceSystem system;
        system.run();
    } catch (const exception& e) {
        cout << "╔════════════════════════════════════════════╗\n";
        cout << "║            🔴 FATAL ERROR:                ║\n";
        cout << "║ " << left << setw(40) << e.what() << " ║\n";
        cout << "╚════════════════════════════════════════════╝\n";
        return 1;
    }
    return 0;
}