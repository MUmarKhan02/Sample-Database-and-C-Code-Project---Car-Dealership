#include <stdio.h>
#include <sqlite3.h> 
#include <string.h>

//callback function
static int callback(void* data, int argc, char** argv, char** ColName){
   
   for(int i = 0; i<argc; i++){
      printf("%s = %s\n", ColName[i], argv[i]);
   }
   
   return 0;
}
//displaying all the options/queries the program can do
void displayListOfOptions(){
	printf("LIST OF OPTIONS TO DO:\n");
	printf("1) Display a table\n");
	printf("2) Add new Entry\n");
	printf("3) Look for a car\n");
	printf("4) Buying a car with a salesperson\n");
	printf("5) Show all car brands that have more than 1 model present in all or a specific dealership\n");
	printf("6) Expensive and Cheapest Cars and Newest and Oldest Cars\n");
	printf("7) Mileage Range of Cars that ARE recorded\n");
	printf("8) How many cars are currently in stock\n");
	printf("9) Change location where a salesperson work at\n");
	printf("10) Update a Customer's Contact info\n");
	printf("11) All cars in database by Make,Model,Trim\n");
	printf("0) EXIT\n");

	
}
//OPTION 4 BUYING A CAR ************************************
void option4BuyCar(sqlite3* db,char* errMsg){
	puts("Buying a car");
	//enter salesperson to make deal with
	char * salesName;
	printf("Enter the name of the salesperson you are making a deal with: ");
	scanf("%s",&salesName);
	//sql command to check sales person exists
	char sql1[1200];
	char *sql = sql1;
	sql+=sprintf(sql,"SELECT Name\n" \
			"FROM SalesPerson\n" \
			"WHERE EXISTS(SELECT 1 FROM Staff,CarDealership WHERE CarDealershipId = CarDealershipId AND SalesPerson.Name = '%s');\n",&salesName);
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else{
		//if exists
		printf("%s exists\n",&salesName);
	}
		char* make;
		char* model;
		//enter make and model of car buying
		printf("Enter the make of the car you are buying: ");
		scanf("%s",&make);
		printf("Enter the model of the car you are buying: ");
		scanf("%s",&model);
		char sql2[1200];
		sql = sql2;
		//sql command to check model exists within the dealership salesperson is working at
		sql+=sprintf(sql,"SELECT Car.Model\n" \
			"FROM CarDealership,Car,Staff,SalesPerson\n" \
			"WHERE SalesPerson.Name = '%s'\n" \
			"AND SalesPerson.StaffId = Staff.CarDealershipId\n" \
			"AND CarDealership.Id = Staff.CarDealershipId\n" \
			"AND Car.CarDealershipId = CarDealership.Id\n" \
			"AND Car.Make = '%s' AND Car.Model = '%s';\n",&salesName,&make,&model);
		char * sqlModel = sql2;
		
		int rr = sqlite3_exec(db, sqlModel, callback, 0, &errMsg);
		if( rr != SQLITE_OK ){
					printf("SQL error: %s\n", errMsg);
					
					sqlite3_free(errMsg);
				}
		else {
		printf("Car does exists where %s is working at\n",&salesName);
		char sql3[1200];
		char * sql = sql3;
			
			//sql command to check car in stock
			sql+=sprintf(sql,"SELECT Stock,SpecificDetails.CarId FROM SpecificDetails,Car\n" \
							"WHERE Stock IS NOT 'NO' AND Car.Make = '%s' AND Car.Model = '%s'" \
							"AND SpecificDetails.CarId = Car.Id;\n",&make,&model);
			char * sqlStock = sql3;
			
			int rrr = sqlite3_exec(db, sqlStock, callback, 0, &errMsg);
			if( rrr != SQLITE_OK ){
						printf("SQL error: %s\n", errMsg);
						
						sqlite3_free(errMsg);
					}
			else {printf("Car is in stock\n",&salesName);
				
				//in case there are more, we want some way to differentiate, so the above command will show an id for all cars relating to model and make
				//and user will select an id
				int carId;
				printf("What is the ID of the car you want?(from list above): ");
				scanf("%d",&carId);
				char sql4[1200];
				sql = sql4;
				//increment number of sales for sales person by 1, and change the stock from YES to NO for that car with ID
				sql+=sprintf(sql,"UPDATE SpecificDetails SET Stock = 'NO' WHERE Stock = 'YES' AND SpecificDetails.CarId = %d;\n",carId);
				char * sqlUpStock = sql4;
				int rrrr = sqlite3_exec(db, sqlUpStock, callback, 0, &errMsg);
			
				if( rrrr != SQLITE_OK ){
					printf("SQL error: %s\n", errMsg);
					
					sqlite3_free(errMsg);
				}
				else printf("Car Has been bought and stock set to NO\n",&salesName);
				char sql5[1200];
				sql = sql5;
				sql+=sprintf(sql,"UPDATE SalesPerson SET NumberOfSales = NumberOfSales + 1 WHERE SalesPerson.Name = '%s';\n",&salesName);
				char * sqlUpSales = sql5;
				int rrrrr = sqlite3_exec(db, sqlUpSales, callback, 0, &errMsg);
			
				if( rrrrr != SQLITE_OK ){
					printf("SQL error: %s\n", errMsg);
					
					sqlite3_free(errMsg);
				}
				else printf("%s number of sales went up by 1\n",&salesName);
			
		}
		
		}
	
	
}
//OPTION 5 MORE THAN 1 CAR OF BRAND *******************************************
void option5MoreThanOne(sqlite3* db,char* errMsg){
	puts("Car brands with more than 1 model in database");
	//command to show all cars make with more than 1 car to its make in the database and the dealership they are in 
	char* sql = "SELECT Name,Make,COUNT(Make)\n" \
				"FROM Car,CarDealership\n" \
				"WHERE Car.CarDealershipId = CarDealership.Id\n" \
				"GROUP BY Make HAVING COUNT(Make)>1;\n";
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("Car brands with more than 1 model shown\n");
}
//OPTION 6 EXPENSIVE/CHEAPEST AND NEWEST/OLDEST *******************************************
void option6Expensive(sqlite3* db,char* errMsg){
	puts("Expensive and Cheapest Car and Newest and Oldest Car");
	//sql command for expensive and cheapest car
	char* sql = "SELECT MAX(Price),MIN(Price) FROM Car;\n";
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("Expensive and Cheapest car shown\n");
	//sql command for newest and oldest car
	sql = "SELECT MAX(Year),MIN(Year) FROM Car;\n";
	r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("Newest and Oldest car Shown\n");
	
}
//OPTION 7 RANGE OF MILEAGES *******************************************
void option7Mileage(sqlite3* db,char* errMsg){
	puts("Range of recorded Mileages");
	//sql command to show all mileages recorded in database
	char* sql = "SELECT Mileage FROM SpecificDetails WHERE Mileage IS NOT NULL;\n";
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("Mileages shown\n");
}
//OPTION 8 CARS IN STOCK *******************************************
void option8Stock(sqlite3* db,char* errMsg){
	puts("Cars in Stock");
	//sql command to show number of cars that are in stock
	char* sql = "SELECT Count(Stock) FROM SpecificDetails WHERE Stock IS NOT 'NO';\n";
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("Cars in stock\n");
}
//option 9 CHANGE LOCATION OF SALES PERSON
void option9ChangeLocation(sqlite3* db,char* errMsg){
	puts("Changing the location of a sales person");
	char * name;
	
	//get name of salesperson that is looking to change work location
	printf("Enter the name of the salesperson that is changing work location: ");
	scanf("%s",&name);
	char sql1[1200];
	char *sql = sql1;
	//sql command to check salesperson exists
	sql+=sprintf(sql,"SELECT Name\n" \
			"FROM SalesPerson\n" \
			"WHERE EXISTS(SELECT 1 FROM Staff,CarDealership WHERE CarDealershipId = CarDealershipId AND SalesPerson.Name = '%s');\n",&name);
	char * sqlSalesExist = sql1;
	int r = sqlite3_exec(db, sqlSalesExist, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else{
		//if exists
		printf("%s exists\n",&name);
	}
		char dealerName[100];
		//enter dealership name where salesperson is moving to
		printf("Enter CarDealership (PRESS TAB AFTER YOU TYPE THE DEALERSHIP NAME YOU WANT PLS and then press enter): ");
		scanf("%[^\t]s",dealerName);
		char *sqlManager = "SELECT Staff.Id,Name FROM Staff,CarDealership WHERE CarDealership.Id = Staff.CarDealershipId;";
		displayTableSQL(sqlManager,db,errMsg);
		
			int idStaff;
			printf("Enter the Id corresponding to the dealership name shown above: ");
			scanf("%d",&idStaff);
			char sql3[1200];
			sql = sql3;
			//sql command to change the id of the salesperson to the id corresponding to where the dealership staff team is
			sql+=sprintf(sql,"UPDATE SalesPerson SET StaffId = %d WHERE SalesPerson.Name = '%s';\n",&idStaff,&name);
			char * sqlUpSales = sql3;
			int rr = sqlite3_exec(db, sqlUpSales, callback, 0, &errMsg);
	
			if( r != SQLITE_OK ){
				printf("SQL error: %s\n", errMsg);
				
				sqlite3_free(errMsg);
			}
			else printf("Change location done!\n");
		
	
}
//OPTION 10 CUSTOMER CONTACT UPDATE *******************************************
void option10ContactUpdate(sqlite3* db,char* errMsg){

	char* name;
	//get customer name
	printf("\nEnter customer's name: ");
	scanf("%s",&name);  
	char sql1[1200];
	char *sql = sql1;
	//sql command to check if customer exist
	sql+=sprintf(sql,"SELECT Name\n" \
			"FROM Customers\n" \
			"WHERE EXISTS(SELECT 1 FROM CarDealership WHERE CarDealershipId = CarDealershipId AND Customers.Name = '%s');\n",&name);
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else{
		//if exists
		char email[100];
		int updateOption;
		long long int phoneNumber;
		
		char oldEmail[100];
		long long int oldPhoneNumber;
		//asks the user what option would they like to update for contact information
		printf("%s exists\n",&name);
		printf("Update Number (1)\n");
		printf("Update Email Address (2)\n");
		printf("Update Both (3)\n");
		printf("What would you like to update? (Enter the number corresponding to option): ");
		scanf("%d",&updateOption);
		if(updateOption==1){//if option 1
		//enter old phone number (current one)
			printf("Enter old phone number: ");
			scanf("%ld",&oldPhoneNumber);
			char sql2[1200];
			sql = sql2;
			//check if this number exists
			sql+=sprintf(sql,"SELECT ContactPhoneNumber\n" \
			"FROM Customers\n" \
			"WHERE EXISTS(SELECT 1 FROM CarDealership WHERE CarDealershipId = CarDealershipId AND Customers.ContactPhoneNumber = %ld AND Customers.Name = '%s');\n",oldPhoneNumber,&name);
			r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
			if( r != SQLITE_OK ){
				printf("SQL error: %s\n", errMsg);
				sqlite3_free(errMsg);
				
			}
			else{//if the phone number exists
			//enter new number
				printf("%ld exists\n",oldPhoneNumber);
				printf("Enter new phone number: ");
				scanf("%ld",&phoneNumber);
				char sql3[1200];
				sql = sql3;
				//sql command to update number
				sql+=sprintf(sql,"UPDATE Customers\n" \
				"SET ContactPhoneNumber = %ld WHERE ContactPhoneNumber = %ld AND Name = '%s';\n",phoneNumber,oldPhoneNumber,&name);
				r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
				if( r != SQLITE_OK ){
					printf("SQL error: %s\n", errMsg);
					sqlite3_free(errMsg);
					
				}
				else printf("UPDATE MADE!");
				
			}
		}
		else if (updateOption==2){
			//same thing for email
			printf("Enter old email address: ");
			scanf("%s",&oldEmail);
			
			char sql4[1200];
			sql = sql4;
			sql+=sprintf(sql,"SELECT ContactEmailAddress\n" \
			"FROM Customers\n" \
			"WHERE EXISTS(SELECT 1 FROM CarDealership WHERE CarDealershipId = CarDealershipId AND Customers.ContactEmailAddress= '%s' AND Customers.Name = '%s');\n",&oldEmail,&name);
			r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
			if( r != SQLITE_OK ){
				printf("SQL error: %s\n", errMsg);
				sqlite3_free(errMsg);
				
			}
			else{
				printf("%s exists\n",oldEmail);
				printf("Enter new email address: ");
				scanf("%s",&email);
				char sql5[1200];
				sql = sql5;
				sql+=sprintf(sql,"UPDATE Customers\n" \
				"SET ContactEmailAddress = %s WHERE ContactEmailAddress = %s AND Name = '%s';\n",&email,&oldEmail,&name);
				r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
				if( r != SQLITE_OK ){
					printf("SQL error: %s\n", errMsg);
					sqlite3_free(errMsg);
					
				}
				else printf("UPDATE MADE!");
				
			}
		}
		else if(updateOption==3){
			//same thing for both number and email
			printf("Enter old phone number: ");
			scanf("%ld",&oldPhoneNumber);
			printf("Enter old email address: ");
			scanf("%s",&oldEmail);
			char sql6[1200];
			sql = sql6;
			sql+=sprintf(sql,"SELECT ContactPhoneNumber\n" \
			"FROM Customers\n" \
			"WHERE EXISTS(SELECT 1 FROM CarDealership WHERE CarDealershipId = CarDealershipId AND Customers.ContactPhoneNumber = %ld AND Customers.ContactEmailAddress = %s AND Customers.Name = '%s');\n",oldPhoneNumber,&oldEmail,&name);
			r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
			if( r != SQLITE_OK ){
				printf("SQL error: %s\n", errMsg);
				sqlite3_free(errMsg);
				
			}
			else{
				printf("%ld and %s exists\n",oldPhoneNumber,&oldEmail);
				printf("Enter new phone number: ");
				scanf("%ld",&phoneNumber);
				printf("Enter new email address: ");
				scanf("%s",&email);
				char sql7[1200];
				sql = sql7;
				sql+=sprintf(sql,"UPDATE Customers\n" \
				"SET ContactPhoneNumber = %ld WHERE ContactPhoneNumber = %ld AND Name = '%s';\n",phoneNumber,oldPhoneNumber,&name);
				sql+=sprintf(sql,"UPDATE Customers\n" \
				"SET ContactEmailAddress = %s WHERE ContactEmailAddress = %s AND Name = '%s';\n",&email,&oldEmail,&name);
				r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
				if( r != SQLITE_OK ){
					printf("SQL error: %s\n", errMsg);
					sqlite3_free(errMsg);
					
				}
				else printf("UPDATE MADE!");
				
			}
		}
		else printf("Invalid Option.......EXITING.........");
		
	}
	
}
//OPTION 11 DISPLAY CARS BY MAKE MODEL AND TRIM *******************************************
void option11MMT(sqlite3* db,char* errMsg){
	puts("Cars by Make,Model,Trim");
	//sql command to show the cars in database by Make,Model,Trim
	char* sql = "SELECT Make,Model,Trim FROM Car,SpecificDetails WHERE SpecificDetails.CarId = Car.Id;";
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("Cars by Make,Model,Trim\n");
}
void displayNewEntryOptions(sqlite3* db,char* errMsg){//for the new entries to be made with option 2
	printf("What table would you like to add a new entry to?\n");
	printf("1) Add new Car\n");
	printf("2) Add new SalesPerson\n");
	printf("3) Add new Car Dealership\n");
	printf("4) Add new Customer\n");
	printf("5) Add new Staff Team\n");
	printf("6) Add new list of Specific Details\n");
	int optionNum;
	printf("Select option: ");
	scanf("%d",&optionNum);
	char* op;
	//depending on option chosen, it will redirect to the function to serve that purpose
	switch(optionNum){
		case 1:
			carEntry(db,errMsg);
			break;
		case 2:
			salesPersonEntry(db,errMsg);
			break;
		case 3:
			dealershipEntry(db,errMsg);
			break;
		case 4:
			customerEntry(db,errMsg);
			break;
		case 5:
			staffEntry(db,errMsg);
			break;
		case 6:
			detailsEntry(db,errMsg);
			break;
		default:
			printf("Invalid Option\n");
			break;
	}
}
//inserting car
void carEntry(sqlite3* db,char* errMsg){
	
	
	char* sql = "SELECT Id,Name FROM CarDealership;" ;
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("CarDealership ID have been displayed successfully\n");
	printf("For new Car Entry, Insert data for: Model,Price,Year,Make,CarDealershipId. (For ID, refer to the number displayed)\n");
	char* model;
	char *make;
	int year;
	int price;
	int iD;
	printf("Model: ");
	scanf("%s",&model);
	printf("Price: ");
	scanf("%d",&price);
	printf("Year: ");
	scanf("%d",&year);
	printf("Make: ");
	scanf("%s",&make);
	printf("ID: ");
	scanf("%d",&iD);
	
	char sql1[1200];
	sql = sql1;
	sql+=sprintf(sql,"INSERT INTO Car (Model,Price,Year,Make,CarDealershipId) VALUES ('%s',%d,%d,'%s',%d);\n",&model,price,year,&make,iD);
	r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("INSERT SUCCESS\n");
}
//SalesPerson Entry
void salesPersonEntry(sqlite3* db,char* errMsg){

	
	char* sql = "SELECT Id,Manager FROM Staff;" ;
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("Staff ID Shown have been displayed successfully\n");
	printf("For new SalesPerson Entry, Insert data for: Name,BirthPlace,Gender,Age(opt),NumberOfSales,BirthMonth,BirthDay,BirthYear,ID (For ID, refer to the number displayed)\n");
	printf("FOR ANY DATA THAT STATES (Opt), THAT IS OPTIONAL, SO IF YOU DO NOT WANT TO ENTER DATA FOR THAT, JUST TYPE NULL\n");
	char* name;
	char* birthPlace;
	char* gender;
	int age;
	int numOfSales;
	char* birthMonth;
	int birthDay;
	int birthYear;
	int iD;
	
	printf("Name (First Name): ");
	scanf("%s",&name);
	printf("BirthPlace: ");
	scanf("%s",&birthPlace);
	printf("Gender: ");
	scanf("%s",&gender);
	printf("Age(Opt): ");
	scanf("%d",&age);
	printf("NumOfSales: ");
	scanf("%d",&numOfSales);
	printf("BirthMonth: ");
	scanf("%s",&birthMonth);
	printf("BirthDay: ");
	scanf("%d",&birthDay);
	printf("BirthYear: ");
	scanf("%d",&birthYear);
	printf("ID: ");
	scanf("%d",&iD);
	
	char sql1[1200];
	sql = sql1;
	
	sql+=sprintf(sql,"INSERT INTO SalesPerson(Name,BirthPlace,Gender,Age,NumberOfSales,BirthMonth,BirthDay,BirthYear,StaffId) VALUES('%s','%s','%s',%d,%d,'%s',%d,%d,%d);\n",&name,&birthPlace,&gender,age,numOfSales,&birthMonth,birthDay,birthYear,iD);
	r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("INSERT SUCCESS\n");
}
//inserting new dealership`
void dealershipEntry(sqlite3* db,char* errMsg){
	
	
	printf("For new CarDealership Entry, Insert data for: Location (opt),Name\n");
	printf("FOR ANY DATA THAT STATES (Opt), THAT IS OPTIONAL, SO IF YOU DO NOT WANT TO ENTER DATA FOR THAT, JUST TYPE NULL\n");
	char name[100];
	char location[100];
	
	printf("Location(Opt): ");
	scanf("%s",location);
	printf("Enter CarDealership (PRESS TAB AFTER YOU TYPE THE DEALERSHIP NAME YOU WANT PLS and then press enter): ");
	scanf("%[^\t]s",name);
	
	
	char sql1[1200];
	char* sql = sql1;
	
	char * sqlCDEntry;
	//checks if the optional column is NULL, if it is, put NULL in respective Column
	if(strcmp(location,"NULL")==0){
		sql+=sprintf(sql,"INSERT INTO CarDealership(Location,Name) VALUES(NULL,'%s');\n",&name);
		sqlCDEntry = sql1;
		int r = sqlite3_exec(db, sqlCDEntry, callback, 0, &errMsg);
	
		if( r != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//otherwise, normal sql command
	else{
		sql+=sprintf(sql,"INSERT INTO CarDealership(Location,Name) VALUES('%s','%s');\n",&location,&name);
		sqlCDEntry = sql1;
		int rr = sqlite3_exec(db, sqlCDEntry, callback, 0, &errMsg);
		
		if( rr != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	
}
//inserting new customer
void customerEntry(sqlite3* db,char* errMsg){
	
	
	char* sql = "SELECT Id,Name FROM CarDealership;" ;
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("CarDealership ID have been displayed successfully\n");
	printf("For new Customers Entry, Insert data for: Age(Opt),Gender,ContactEmail,ContactPhoneNumber,BirthMonth,BirthDay,BirthYear,BirthPlace,Name,ID (For ID, refer to the number displayed)\n");
	printf("FOR ANY DATA THAT STATES (Opt), THAT IS OPTIONAL, SO IF YOU DO NOT WANT TO ENTER DATA FOR THAT, JUST TYPE NULL\n");
	char* name;
	char* birthPlace;
	char* birthMonth;
	char* birthDay;
	int birthYear;
	char* gender;
	int age;
	long long int phoneNum;
	char* emailAddr;
	
	int iD;
	//since age is int, no need to check for null since an int is not surrounded by ' '
	printf("Name (First Name): ");
	scanf("%s",&name);
	printf("BirthPlace: ");
	scanf("%s",&birthPlace);
	printf("Gender: ");
	scanf("%s",&gender);
	printf("Age(Opt): ");
	scanf("%d",&age);
	printf("BirthMonth: ");
	scanf("%s",&birthMonth);
	printf("BirthDay: ");
	scanf("%s",&birthDay);
	printf("BirthYear: ");
	scanf("%d",&birthYear);
	printf("ContactEmail: ");
	scanf("%s",&emailAddr);
	printf("PhoneNumber: ");
	scanf("%ld",&phoneNum);
	printf("ID: ");
	scanf("%d",&iD);
	
	char sql1[1200];
	sql = sql1;
	sql+=sprintf(sql,"INSERT INTO Customers(Age,Gender,ContactEmail,ContactPhoneNumber,BirthMonth,BirthDay,BirthYear,Name,CarDealershipId) VALUES(%d,'%s','%s',%ld,'%s','%s',%d,'%s','%s',%d);\n",age,&gender,&emailAddr,phoneNum,&birthMonth,&birthDay,birthYear,&birthPlace,&name,iD);
	r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("INSERT SUCCESS\n");
}
//Inserting a New Staff entry
void staffEntry(sqlite3* db,char* errMsg){
	
	
	char* sql = "SELECT Id,Name FROM CarDealership;" ;
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("CarDealership ID have been displayed successfully\n");
	printf("For new Staff Entry, Insert data for: CarDetailer(Opt),CustomerService(Opt),ID,Manager(For ID, refer to the number displayed)\n");
	printf("FOR ANY DATA THAT STATES (Opt), THAT IS OPTIONAL, SO IF YOU DO NOT WANT TO ENTER DATA FOR THAT, JUST TYPE NULL\n");

	char* manager;
	char carDetailer[50];
	char customerService[50];
	int iD;
	
	printf("CarDetailer(Opt)(First Name): ");
	scanf("%s",carDetailer);
	printf("CustomerService(Opt)(First Name): ");
	scanf("%s",customerService);
	printf("Manager (First Name): ");
	scanf("%s",&manager);
	printf("ID: ");
	scanf("%d",&iD);
	
	char sql1[1200];
	sql = sql1;
	char * sqlSEntry;
	//checks if carDetailer NULL but other isnt
	if(strcmp(carDetailer,"NULL")==0 && strcmp(customerService,"NULL")!=0){
		sql+=sprintf(sql,"INSERT INTO Staff(CarDetailer,CustomerService,CarDealershipId,Manager) VALUES(NULL,'%s',%d,'%s');\n",&customerService,iD,&manager);
		sqlSEntry = sql1;
		int r = sqlite3_exec(db, sqlSEntry, callback, 0, &errMsg);
	
		if( r != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//checks if carDetailer not NULL but Customer Service is
	else if(strcmp(carDetailer,"NULL")!=0 && strcmp(customerService,"NULL")==0){
		sql+=sprintf(sql,"INSERT INTO Staff(CarDetailer,CustomerService,CarDealershipId,Manager) VALUES('%s',NULL,%d,'%s');\n",&carDetailer,iD,&manager);
		sqlSEntry = sql1;
		int rr = sqlite3_exec(db, sqlSEntry, callback, 0, &errMsg);
	
		if( rr != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//check if both null
	else if(strcmp(carDetailer,"NULL")==0 && strcmp(customerService,"NULL")==0){
		sql+=sprintf(sql,"INSERT INTO Staff(CarDetailer,CustomerService,CarDealershipId,Manager) VALUES(NULL,NULL,%d,'%s');\n",iD,&manager);
		sqlSEntry = sql1;
		int rrr = sqlite3_exec(db, sqlSEntry, callback, 0, &errMsg);
	
		if( rrr != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//if none are null
	else{
		sql+=sprintf(sql,"INSERT INTO Staff(CarDetailer,CustomerService,CarDealershipId,Manager) VALUES('%s','%s',%d,'%s');\n",&carDetailer,&customerService,iD,&manager);
		sqlSEntry = sql1;
		int rrrr = sqlite3_exec(db, sqlSEntry, callback, 0, &errMsg);
		if( rrrr!= SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}

}
void detailsEntry(sqlite3* db,char* errMsg){
	
	
	char* sql = "SELECT Id,Model,Make FROM Car;" ;
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("CarID have been displayed successfully\n");
	printf("For new SpecificDetails Entry, Insert data for: Transmission,Trim,Stock,Engine (Opt),Colour,BodyType,FuelType(Opt),Mileage(Opt),ID (For ID, refer to the number displayed)\n");
	printf("FOR ANY DATA THAT STATES (Opt), THAT IS OPTIONAL, SO IF YOU DO NOT WANT TO ENTER DATA FOR THAT, JUST TYPE NULL\n");
	char* transmission;
	char* trim;
	char* stock;
	char engine[50];
	char* colour;
	char* bodyType;
	char fuelType[50];
	char mileage[50];
	
	int iD;
	
	printf("Transmission (Automatic or Manual): ");
	scanf("%s",&transmission);
	printf("Trim: ");
	scanf("%s",&trim);
	printf("Stock (Either YES or NO): ");
	scanf("%s",&stock);
	printf("Engine(Opt): ");
	scanf("%s",engine);
	printf("Colour: ");
	scanf("%s",&colour);
	printf("BodyType: ");
	scanf("%s",&bodyType);
	printf("FuelType(Opt): ");
	scanf("%s",fuelType);
	printf("Mileage(Opt): ");
	scanf("%s",mileage);
	printf("ID: ");
	scanf("%d",&iD);
	
	char sql1[1200];
	sql = sql1;
	char * sqlSDEntry;
	//checks if engine is null but others arent
	if(strcmp(engine,"NULL")==0 && strcmp(fuelType,"NULL")!=0 && strcmp(mileage,"NULL")!=0 ){
		sql+=sprintf(sql,"INSERT INTO SpecificDetails(Transmission,Trim,Stock,Engine,Colour,BodyType,FuelType,Mileage,CarId) VALUES('%s','%s','%s',NULL,'%s','%s','%s','%s',%d);\n",&transmission,&trim,&stock,&colour,&bodyType,&fuelType,&mileage,iD);
		sqlSDEntry = sql1;
		int r = sqlite3_exec(db, sqlSDEntry, callback, 0, &errMsg);
	
		if( r != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//check if fueltype is null but others arent
	else if(strcmp(engine,"NULL")!=0 && strcmp(fuelType,"NULL")==0 && strcmp(mileage,"NULL")!=0 ){
		sql+=sprintf(sql,"INSERT INTO SpecificDetails(Transmission,Trim,Stock,Engine,Colour,BodyType,FuelType,Mileage,CarId) VALUES('%s','%s','%s','%s','%s','%s',NULL,'%s',%d);\n",&transmission,&trim,&stock,&engine,&colour,&bodyType,&mileage,iD);
		sqlSDEntry = sql1;
		int r2 = sqlite3_exec(db, sqlSDEntry, callback, 0, &errMsg);
	
		if( r2 != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//check if mileage is null but others arent
	else if(strcmp(engine,"NULL")!=0 && strcmp(fuelType,"NULL")!=0 && strcmp(mileage,"NULL")==0 ){
		sql+=sprintf(sql,"INSERT INTO SpecificDetails(Transmission,Trim,Stock,Engine,Colour,BodyType,FuelType,Mileage,CarId) VALUES('%s','%s','%s','%s','%s','%s','%s',NULL,%d);\n",&transmission,&trim,&stock,&engine,&colour,&bodyType,iD);
		sqlSDEntry = sql1;
		int r3 = sqlite3_exec(db, sqlSDEntry, callback, 0, &errMsg);
	
		if( r3 != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//check if engine and fueltype is null and mileage isnt
	else if(strcmp(engine,"NULL")==0 && strcmp(fuelType,"NULL")==0 && strcmp(mileage,"NULL")!=0 ){
		sql+=sprintf(sql,"INSERT INTO SpecificDetails(Transmission,Trim,Stock,Engine,Colour,BodyType,FuelType,Mileage,CarId) VALUES('%s','%s','%s',NULL,'%s','%s',NULL,'%s',%d);\n",&transmission,&trim,&stock,&colour,&bodyType,&mileage,iD);
		sqlSDEntry = sql1;
		int r4 = sqlite3_exec(db, sqlSDEntry, callback, 0, &errMsg);
	
		if( r4 != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//check if engine and mileage is null but fueltype isnt
	else if(strcmp(engine,"NULL")==0 && strcmp(fuelType,"NULL")!=0 && strcmp(mileage,"NULL")==0 ){
		sql+=sprintf(sql,"INSERT INTO SpecificDetails(Transmission,Trim,Stock,Engine,Colour,BodyType,FuelType,Mileage,CarId) VALUES('%s','%s','%s',NULL,'%s','%s','%s',NULL,%d);\n",&transmission,&trim,&stock,&colour,&bodyType,&fuelType,&mileage,iD);
		sqlSDEntry = sql1;
		int r5 = sqlite3_exec(db, sqlSDEntry, callback, 0, &errMsg);
	
		if( r5 != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//check if fuelType and mileage is null but engine isnt
	else if(strcmp(engine,"NULL")!=0 && strcmp(fuelType,"NULL")==0 && strcmp(mileage,"NULL")==0 ){
		sql+=sprintf(sql,"INSERT INTO SpecificDetails(Transmission,Trim,Stock,Engine,Colour,BodyType,FuelType,Mileage,CarId) VALUES('%s','%s','%s','%s','%s','%s',NULL,NULL,%d);\n",&transmission,&trim,&stock,&engine,&colour,&bodyType,iD);
		sqlSDEntry = sql1;
		int r6 = sqlite3_exec(db, sqlSDEntry, callback, 0, &errMsg);
	
		if( r6 != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//check if all 3; engine, fueltype,mileage is null
	else if(strcmp(engine,"NULL")==0 && strcmp(fuelType,"NULL")==0 && strcmp(mileage,"NULL")==0 ){
		sql+=sprintf(sql,"INSERT INTO SpecificDetails(Transmission,Trim,Stock,Engine,Colour,BodyType,FuelType,Mileage,CarId) VALUES('%s','%s','%s',NULL,'%s','%s',NULL,NULL,%d);\n",&transmission,&trim,&stock,&colour,&bodyType,iD);
		sqlSDEntry = sql1;
		int r7 = sqlite3_exec(db, sqlSDEntry, callback, 0, &errMsg);
	
		if( r7 != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
	//check if none are null
	else{
		sql+=sprintf(sql,"INSERT INTO SpecificDetails VALUES('%s','%s','%s','%s','%s','%s','%s','%s',%d);\n",&transmission,&trim,&stock,&engine,&colour,&bodyType,&fuelType,&mileage,iD);
		sqlSDEntry = sql1;
		int r8 = sqlite3_exec(db, sqlSDEntry, callback, 0, &errMsg);
		
		if( r8 != SQLITE_OK ){
			printf("SQL error: %s\n", errMsg);
			
			sqlite3_free(errMsg);
		}
		else printf("INSERT SUCCESS\n");
	}
}
//for OPTION 1, DISPLAY the TABLE
void displayTableOptions(sqlite3* db,char* errMsg){
	//shows the options
	printf("What table would you like to display\n");
	printf("1) All Car Dealerships\n");
	printf("2) All Cars in database\n");
	printf("3) All Cars' SpecificDetails\n");
	printf("4) All Customers\n");
	printf("5) All Staff Teams\n");
	printf("6) All Car Dealership's SalesPersons\n");
	int optionNum;
	printf("Select option: ");
	scanf("%d",&optionNum);
	char* op;
	switch(optionNum){
		//depending on option calls the displayTableSQL function with proper SQL statement to show data of specified table
		case 1:
			op = "SELECT * FROM CarDealership";
			displayTableSQL(op,db,errMsg);
			break;
		case 2:
			op = "SELECT * FROM Car";
			displayTableSQL(op,db,errMsg);
			break;
		case 3:
			op = "SELECT * FROM SpecificDetails";
			displayTableSQL(op,db,errMsg);
			break;
		case 4:
			op = "SELECT * FROM Customers";
			displayTableSQL(op,db,errMsg);
			break;
		case 5:
			op = "SELECT * FROM Staff";
			displayTableSQL(op,db,errMsg);
			break;
		case 6:
			op = "SELECT * FROM SalesPerson";
			displayTableSQL(op,db,errMsg);
			break;
		default:
			printf("Invalid Option\n");
			break;
			
	}
	
}
//This process the option entered by user from the start menu, redirecting whatever option they entered to the function that will serve out that option
void processOption(int num,sqlite3* db,char* errMsg){
	switch(num){
		case 1:
			displayTableOptions(db,errMsg);
			break;
		case 2:
			displayNewEntryOptions(db,errMsg);
			break;
		case 3:
			option3Look(db,errMsg);
			break;
		case 4:
			option4BuyCar(db,errMsg);
			break;
		case 5:
			option5MoreThanOne(db,errMsg);
			break;
		case 6:
			option6Expensive(db,errMsg);
			break;
		case 7:
			option7Mileage(db,errMsg);
			break;
		case 8:
			option8Stock(db,errMsg);
			break;
		case 9:
			option9ChangeLocation(db,errMsg);
			break;
		case 10:
			option10ContactUpdate(db,errMsg);
			break;
		case 11:
			option11MMT(db,errMsg);
			break;
		default:
			printf("Invalid Option\n");
			break;
		
	}
	
}
//to DISPLAY THE TABLE BASED ON INPUT
void displayTableSQL(char* word,sqlite3* db,char* errMsg){
	char* sql=word;
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);

	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	else printf("Table have been displayed successfully\n");
	
}
//OPTION 3 LOOK UP CAR *******************************************
void option3Look(sqlite3* db,char* errMsg){
	
	puts("Looking up a car by Make and Model");
	//calls the displayTableSQL function to display name
	char* sql = "SELECT Name FROM CarDealership;" ;
	int r = sqlite3_exec(db, sql, callback, 0, &errMsg);

	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		sqlite3_free(errMsg);
	}
	else {printf("Table have been displayed successfully\n");}
	
	//from list, show car dealership want to look for
	
	//enter car dealership name and make and model of car
	char carDealer[100];
	char make[50];
	char model[50];
	printf("Enter CarDealership (PRESS TAB AFTER YOU TYPE THE DEALERSHIP NAME YOU WANT PLS and then press enter): ");
	scanf("%[^\t]s",carDealer);
	
	printf("Enter make of car: ");
	scanf("%s",make);
	printf("Enter the model of car: ");
	scanf("%s",model);
	
	char sql1[1200];
	sql = sql1;
	
	//slq command to look up make and model in car dealership
	sql+=sprintf(sql1,"SELECT Make,Model FROM Car, CarDealership WHERE Car.CarDealershipId = CarDealership.Id AND CarDealership.Name = '%s' AND Car.Make = '%s' AND Car.Model = '%s';",&carDealer,&make,&model);

	r = sqlite3_exec(db, sql, callback, 0, &errMsg);
	
	if( r != SQLITE_OK ){
		printf("SQL error: %s\n", errMsg);
		
		sqlite3_free(errMsg);
	}
	else printf("Lookup complete, %s %s exists in the database\n",make,model);
	
}

int main(int argc, char* argv[]) {

	sqlite3* db;
	int c;
	//connects to database
	c = sqlite3_open_v2("CarDealer.db", &db, SQLITE_OPEN_READWRITE, NULL);
	if(c != SQLITE_OK) {
		printf("Can’t open database! %s\n", sqlite3_errmsg(db));
		return(0);
	} 
	else {
		printf("Connect to database successfully.\n");
	}
	//code to open database
	char* errMsg = 0;
	
	int optionNum;
	printf("Welcome to the CarDealership Database Application!\n");
	puts("**************************************************");
	puts("**************************************************");
	while(1){//forever loop
		displayListOfOptions();//displays list of options
		printf("Enter an option from the list: ");
		scanf("%d",&optionNum);//select number option
		printf("Selected option: %d\n",optionNum);
		if(optionNum == 0){//if its 0, break out of loop
			printf("See ya Later - From CarDealership Database\n");
			break;
		}
		processOption(optionNum,db,errMsg);//process that option for command
		
		
	}
	//close database
	sqlite3_close(db);
}
