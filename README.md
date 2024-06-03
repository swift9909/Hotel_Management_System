# Hotel_Management_System

This Hotel Management System is designed to streamline and automate various operations involved in running a hotel. It includes modules for managing tables, customers, orders, and billing. The system consists of the following key components:

## Table Management (table.c)

This module manages the tables in the hotel. Each table can have multiple customers associated with it, making customers the child entities of tables.

Customers are created as child processes within this module.

Functions include adding/removing tables, assigning customers to tables, and maintaining the status of each table (occupied, vacant, etc.).

## Order Management (waiter.c)

Waiters use this module to take orders from customers and send them to the kitchen or relevant departments.

Functions include taking new orders, modifying existing orders, and sending order details to the manager for billing.

## Hotel Manager (hotelmanager.c)

The manager is responsible for compiling and generating bills based on the orders received from the waiters.

Functions include calculating total costs, applying discounts or promotions, generating invoices, and managing overall financial records.

## Cleanup and Shutdown (cleanup.c)

This module is responsible for closing the hotel operations for the day. It ensures that all data is properly saved and the system is safely shut down.

Functions include finalizing pending transactions, backing up data, and releasing resources used during the day.

## Detailed Component Interactions

### table.c:

Manages table creation and deletion.

Creates customer processes as children and links them to specific tables.

Handles customer check-ins and check-outs, updating the status of tables accordingly.

### waiter.c:

Takes orders from customers seated at tables.

Sends order information to hotelmanager.c for processing and billing.

### hotelmanager.c:

Receives orders from waiter.c.

Compiles bills and processes payments.

Maintains financial records and generates daily reports.

### cleanup.c:

Finalizes the day's operations.

Ensures all data is saved.

Safely shuts down the system.

## Workflow Overview

Check-in: Customers check in and are assigned to tables via table.c. Customers are created as child processes.

Order Taking: Waiters take orders using waiter.c and forward them to hotelmanager.c.

Billing: The manager compiles bills based on orders using hotelmanager.c.

Check-out: Customers check out, and their data is updated in table.c.

Daily Cleanup: At the end of the day, cleanup.c ensures all operations are finalized and the system is shut down safely.

This modular approach ensures a well-organized and efficient management of hotel operations, enhancing both customer service and operational efficiency.
