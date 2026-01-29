# Car Dealership Management System (C + SQLite3)

This repository contains an academic project developed during my undergraduate studies using **C** and **SQLite3**  
The project implements a simple **car dealership management system** backed by a relational database and accessed through a C program.

The goal of this project was to practice:
- Relational database design
- SQL queries
- Integrating SQLite3 with C
- Basic CRUD-style operations in a systems programming context

---

## Database Design

The system uses an **SQLite3** database with the following main tables:

### Tables Overview

- **Car**
  - Stores basic vehicle inventory information
  - Fields include make, model, year, and price
  - Linked to a dealership and extended by vehicle-specific details

- **CarDealership**
  - Represents dealership locations
  - Stores dealership name and location
  - Acts as a parent entity for cars, customers, and staff

 - **SpecificDetails**
  - Stores detailed vehicle attributes:
    - Transmission
    - Trim
    - Engine
    - Fuel type
    - Body type
    - Mileage
    - Colour
    - Stock status
  - Linked to the `Car` table via a foreign key

---

### People & Operations

- **Customers**
  - Stores customer demographic and contact information
  - Includes birth details, contact email, and phone number
  - Linked to a specific dealership

- **SalesPerson**
  - Stores salesperson information
  - Tracks number of sales and basic personal details
  - Represents sales activity within a dealership

- **Staff**
  - Represents non-sales dealership staff roles
  - Includes:
    - Car detailers
    - Customer service staff
    - Managers
  - Linked to a dealership


### Design Notes

- Auto-incrementing primary keys are used throughout
- Foreign keys model real-world relationships between:
  - Dealerships and inventory
  - Vehicles and detailed specifications
  - Customers, salespeople, and staff
- The schema reflects standard relational database normalization principles

---

## Technologies Used

- **C (ANSI C)**
- **SQLite3**
- **GCC** (or any standard C compiler)

---

## How to Build and Run

### Requirements
- SQLite3 development library installed  
- GCC or another C compiler

## If on Windows, download WSL with ```wsl --install```and setup on local machine and run the following lines to 
```bash
sudo apt update
sudo apt install sqlite3 libsqlite3-dev
sudo apt install gcc
```

### Compile
```bash
gcc main.c -o dealership -lsqlite3
```
