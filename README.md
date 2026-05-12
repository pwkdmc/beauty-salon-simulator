# Beauty Salon Simulator 💇‍♀️💅
A full-featured C++ business simulation of a beauty salon with PostgreSQL persistence, polymorphic employee hierarchy, appointment scheduling, financial tracking, and data analytics. Features a layered architecture with separate UI, business logic, data access, and model layers.
## 🌟 Overview
This project simulates the daily operations of a beauty salon with staff management, client relationships, service catalog, appointment booking, financial accounting, and business analytics. It uses a PostgreSQL database for persistent storage with a custom save/load mechanism, implements a factory pattern for employee creation, and provides comprehensive exception handling across all layers.
## 🧩 Features
### 👥 Employee Management
- **Four specialization types**: Hairdresser, Manicurist, Cosmetologist, Makeup Artist
- **Polymorphic hierarchy** with virtual methods for workload limits and service times
- **Salary calculation**: Base salary + commission percentage with pro-rata adjustment for partial months
- **Status tracking**: Working, on vacation, fired (soft delete)
- **Experience and rating** attributes affecting service quality
### 👤 Client Management
- Contact information with phone number tracking
- **Loyalty program**: Three-tier discount system (5%/10%/15%) based on total spending
- **Bonus points**: Earn 5% of service cost, redeemable up to 50% of service price
- Visit history tracking with cumulative spending
- Soft delete support with database-level filtering
### 💇 Service Catalog
- Services mapped to employee specializations
- Duration and cost configuration
- Materials cost tracking for expense calculation
- Specialization-based filtering for appointment booking
### 📅 Appointment Scheduling
- **Conflict detection**: Prevents double-booking employees or clients at same time
- **Time validation**: Respects working hours (10:00–20:00 weekdays, 10:00–18:00 Saturdays)
- **Workload limits**: Per-employee daily maximum based on specialization
- Status lifecycle: Scheduled → Completed / Cancelled
- Sunday closure enforcement
### 💰 Financial System
- **Cash balance** tracking (starts at 50,000 RUB)
- **Revenue** from completed appointments with loyalty discounts applied
- **Expenses**: Materials cost, employee salaries, monthly rent (5,000 RUB)
- **Automatic end-of-month processing**: Salary payout and rent deduction
- Period-based revenue/expense reporting
### 🕹️ Simulation Modes
- **Step-by-step**: Advance one day or one month at a time
- **Auto mode**: Continuous daily progression with 1-second interval, stoppable with 'q' key
- **Save/Load**: Full state persistence to PostgreSQL with save table mirroring
### 📊 Analytics & Reporting
- **Master workload**: Current bookings vs. capacity with percentage
- **Top services**: Most popular services by completion count
- **Client retention**: Returning vs. one-time client percentage
- **Client bonuses**: Points balance for all active clients
- **Schedule views**: Today, tomorrow, weekly
### Layer Responsibilities
| Layer | Files | Purpose |
|-------|-------|---------|
| **UI** | `Menu.cpp/h` | Console menus, input validation, user interaction |
| **Business Logic** | `BeautySalonSimulator.cpp/h` | State machine, day processing, financial operations |
| **Models** | `Employee.cpp/h`, `Client.cpp/h`, `Service.cpp/h`, `Appointment.cpp/h` | Domain entities with CRUD operations |
| **Factory** | `EmployeeFactory.cpp/h` | Polymorphic employee creation from type string |
| **Database** | `DatabaseManager.cpp/h` | Singleton PostgreSQL connection, query execution |
| **Utils** | `DateUtils.cpp/h` | Date arithmetic, validation, formatting |
| **Exceptions** | `*.h` files in `exceptions/` | Typed exception hierarchy |
## 🗄️ Database Schema
### Main Tables
- **salon_state** — Current date, cash balance, revenue/expenses totals
- **employees** — Staff records with salary configuration
- **employee_types** — Specialization-specific parameters (max clients, service time)
- **clients** — Customer profiles with loyalty data
- **services** — Service catalog with pricing
- **appointments** — Booking records with status tracking
- **salary_log** — Monthly salary payment history
### Save Tables
Mirror tables prefixed with `save_` for full state preservation across sessions.
## 🎯 Key Design Patterns
| Pattern | Implementation |
|---------|---------------|
| **Singleton** | `DatabaseManager` — single database connection instance |
| **Factory Method** | `EmployeeFactory` — creates correct subclass from type string |
| **Polymorphism** | `Employee` base with virtual `getMaxClientsPerDay()`, `getAvgServiceTime()`, `getType()` |
| **RAII** | `unique_ptr` in `BeautySalonSimulator` for model collections |
| **Transaction Script** | `processDayEnd()` processes all daily appointments in a DB transaction |
| **Observer-like** | `executeWithCallback()` pattern for database query result processing |
## 📄 License
The project is intended for educational purposes. It can be used and modified freely.