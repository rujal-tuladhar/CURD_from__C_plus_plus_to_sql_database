//Group 6
//Name: Rujal Tuladhar
//seneca id: 154594188
//email : rtuladhar@myseneca.ca

//Name: Long Nguyen
//seneca id: 155176183
//email : lnguyen97@myseneca.ca




#include <iostream>
#include <occi.h>


using oracle::occi::Environment;
using oracle::occi::Connection;
using namespace oracle::occi;
using namespace std;

struct ShoppingCart {
	int product_id;
	double price;
	int quantity;
};

bool yn();
int mainMenu();
int customerLogin(Connection* conn, int customerId);
int addToCart(Connection* conn, struct ShoppingCart cart[]);
double findProduct(Connection* conn, int product_id);
void displayProducts(struct ShoppingCart cart[], int productCount);
int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount);

int mainMenu(void) {
	int option = 0;

	cout << "******************** Main Menu ********************\n"
		<< "1)\tLogin\n"
		<< "0)\tExit\n";
	cout << "Enter an option (0-1): ";
	cin >> option;

	while (option < 0 || option > 1)
	{
		cout << "******************** Main Menu ********************\n"
			<< "1)\tLogin\n"
			<< "0)\tExit\n";
		cout << "You entered a wrong value. Enter an option (0-1): ";
		cin.clear();
		cin.ignore(200, '\n');
		cin >> option;
	}


	return option;
}

int customerLogin(Connection* conn, int customerId) {
	int result = 0;
	Statement* stmt = conn->createStatement();
	stmt->setSQL("BEGIN"" find_customer(:1,:2);""END;");
	stmt->setNumber(1, customerId);
	stmt->registerOutParam(2, Type::OCCIINT, sizeof(result));
	stmt->executeUpdate();
	result = stmt->getInt(2);
	conn->terminateStatement(stmt);
	return result;
}

int addToCart(Connection* conn, struct ShoppingCart cart[]) {
	int productId = 0;
	bool check = 1;
	int num = 0;
	vector<ShoppingCart> vCart;
	cout << "-------------- Add Products to Cart --------------" << endl;
	while (vCart.size() <= 5 && check != 0) {
		cout << "Enter the product ID: ";
		cin >> productId;
		if (productId != 0) {
			if (findProduct(conn, productId) == 0) {
				cout << "The product does not exists. Try again..." << endl;
			}
			else {
				cout << "Product Price: " << findProduct(conn, productId) << endl;
				cout << "Enter the product Quantity: ";
				cin >> num;
				ShoppingCart item;
				item.product_id = productId;
				item.price = findProduct(conn, productId);
				item.quantity = num;
				vCart.push_back(item);
				cout << "Enter 1 to add more products or 0 to checkout: ";
				cin >> check;
			}
		}
		else {
			check = 0;
		}
	}
	for (size_t i = 0; i < vCart.size(); i++) {
		cart[i] = vCart[i];
	}
	return vCart.size();
}

double findProduct(Connection* conn, int product_id) {
	double price = 0;
	Statement* stmt = conn->createStatement();
	stmt->setSQL("BEGIN"
		" find_product(:1, :2);"
		"  END;");
	stmt->setNumber(1, product_id);
	stmt->registerOutParam(2, Type::OCCIDOUBLE, sizeof(price));
	stmt->executeUpdate();

	price = stmt->getDouble(2);
	conn->terminateStatement(stmt);
	return price;
}

void displayProducts(struct ShoppingCart cart[], int productCount) {
	double total = 0;
	cout << "------- Ordered Products ---------" << endl;
	for (size_t i = 0; i < productCount; i++) {
		cout << "---Item " << i + 1 << endl;
		cout << "Product ID: " << cart[i].product_id << endl;
		cout << "Price: " << cart[i].price << endl;
		cout << "Quantity: " << cart[i].quantity << endl;
		total = cart[i].price * cart[i].quantity;
	}
	cout << "----------------------------------" << endl;
	cout << "Total: " << total << endl;
}

int checkout(Connection* conn, struct ShoppingCart cart[], int customerId, int productCount) {
	if (yn()) {
		int newOrderId = 0;
		Statement* stmt = conn->createStatement();
		stmt->setSQL("BEGIN"
			" add_order(:1, :2);"
			" END;");
		stmt->setNumber(1, customerId);
		stmt->registerOutParam(2, Type::OCCIINT, sizeof(newOrderId));
		stmt->executeUpdate();
		newOrderId = stmt->getInt(2);
		conn->terminateStatement(stmt);

		stmt = conn->createStatement();
		for (int i = 0; i < productCount; i++) {
			stmt->setSQL("BEGIN"
				" add_order_item(:1,:2,:3,:4,:5);"
				" END;");
			stmt->setNumber(1, newOrderId);
			stmt->setNumber(2, (i + 1));
			stmt->setNumber(3, cart[i].product_id);
			stmt->setNumber(4, cart[i].quantity);
			stmt->setNumber(5, cart[i].price);
		}
		conn->terminateStatement(stmt);
		return 1;
	}
	else {
		return 0;
	}
}

bool yn() {
	bool result = false;
	string option;
	int check = 0;
	do {
		cout << "Would you like to checkout? (Y/y or N/n) ";
		cin >> option;
		cin.ignore(1000, '\n');
		if (option == "Y" || option == "y") {
			check = 1;
			result = true;
		}
		else {
			if (option == "N" || option == "n") {
				check = 0;
				result = false;
			}
			else {
				check = 2;
				cout << "Wrong input. Try again..." << endl;;
			}
		}
	} while (check == 2);
	return result;
}


int main(void) {
	//OCCI variables
	Environment* env = nullptr;
	Connection* conn = nullptr;
	//used variables
	string str;
	string user = "dbs311_211a32";
	string pass = "13627192";
	string constr = "myoracle12c.senecacollege.ca:1521/oracle12c";
	
	try {
		env = Environment::createEnvironment(Environment::DEFAULT);
		conn = env->createConnection(user, pass, constr);
		int check;
		int customerId;
		do {
		
			check = mainMenu();
			if (check == 1) {
				cout << "Enter the customer ID: ";
				cin >> customerId;
				
				if (customerLogin(conn, customerId) == 0) {
					cout << "The customer does not exist." << endl;
				}
				else {
					//if exist create the cart 
					ShoppingCart cart[5];
					int productCnt = addToCart(conn, cart);
					displayProducts(cart, productCnt);
					checkout(conn, cart, customerId, productCnt);
				}
			}
		} while (check != 0);

		cout << "Good bye!..." << endl;

		env->terminateConnection(conn);
		Environment::terminateEnvironment(env);
	}
	catch (SQLException& sqlExcp) {
		cout << sqlExcp.getErrorCode() << ": " << sqlExcp.getMessage();
	}

	return 0;
}