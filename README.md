# Surveillance System Backend

**Surveillance System Backend** is a high-performance server application developed in **C++20** using the **Drogon** framework. It serves as the central core of a video surveillance system, managing cameras, processing security events, storing AI metadata, and handling access control.

---

## Overview

This project provides a robust REST API for:
- Authorization and secure user session management.# Surveillance System Backend

**Surveillance System Backend** is a high-performance server application developed in **C++20** using the **Drogon** framework. It serves as the central core of a video surveillance system, managing cameras, processing security events, storing AI metadata, and handling access control.

---

## Overview

This project provides a robust REST API for:
- Authorization and secure user session management.
- CRUD operations on system entities (Cameras, Persons, Events).
- Collecting and retrieving analytical data (Alerts, System Logs).
- Handling vector data (Embeddings) for face recognition systems.
- High-speed asynchronous interaction with a **PostgreSQL** database.

---

## Core Features

## Security & Authentication Module

Security is a fundamental part of the system. The module ensures data protection and strict access control to the API.

### Main Capabilities
- **JWT-like Authentication:** Uses Bearer tokens to protect private endpoints.
- **Secure Password Storage:** Custom integration of the **bcrypt** (Blowfish) algorithm for reliable password hashing.
- **Token Management:**
  - Generates unique tokens (UUID).
  - Configurable session lifetime via the `TOKEN_TTL_SECONDS` environment variable.
  - Automatic validity and expiration checks via the `AuthFilter` middleware.
- **Automatic Hash Upgrade:** The system automatically upgrades outdated password hash algorithms upon successful user login.

---

## Entity Management System

The system provides a flexible and unified interface for managing surveillance objects.

### Managed Entities
- **Persons:** A database with photos, roles, and access status.
- **Cameras:** A registry of surveillance cameras with settings for IP addresses, locations, and stream URLs.
- **Events:** A chronology of recognition events (person ID, camera, timestamp, AI confidence).
- **Alerts:** Security notifications with severity levels.
- **Embeddings:** Vector representations of faces for further analysis and search.

### Data Handling
- **Unified DTOs:** Uses Data Transfer Objects (DTO) to standardize JSON responses.
- **Field Filtering:** Intelligent filtering automatically hides sensitive fields (passwords, tokens, deleted records) in API responses.
- **Aggregated Data:** A special endpoint `/api/all` to retrieve a full snapshot of the system state (persons, cameras, events, logs) in a single request.

---

## Database Integration

The backend is built on **PostgreSQL** using the powerful ORM from Drogon.

### Features
- **Asynchronous I/O:** All DB requests are non-blocking, allowing the handling of a large number of concurrent connections.
- **Connection Pooling:** Efficient connection pool management to optimize resources.
- **Generic Repository Pattern:** Uses templates (`TableControllerBase`, `TableRepository`) to standardize data access and reduce code duplication.
- **Dynamic Configuration:** Connections are configured via environment variables, ensuring compatibility with cloud databases (e.g., Supabase).

---

## Deployment & Configuration

The project is designed with modern deployment and containerization standards in mind.

- **Docker Support:** Includes a `Dockerfile` with multi-stage builds to create lightweight images.
- **CMake Presets:** Pre-configured build profiles for Windows (x64/x86) and Linux.
- **Cross-Platform:** Supports building on both Linux (Ubuntu) and Windows.

### Environment Variables
| Variable | Description |
| :--- | :--- |
| `SUPABASE_DB_URL` | PostgreSQL connection string (Mandatory) |
| `PORT` | Server port (default is 8080) |
| `TOKEN_TTL_SECONDS` | Authorization token lifetime (default is 24 hours) |

---

## Project Structure
- CRUD operations on system entities (Cameras, Persons, Events).
- Collecting and retrieving analytical data (Alerts, System Logs).
- Handling vector data (Embeddings) for face recognition systems.
- High-speed asynchronous interaction with a **PostgreSQL** database.

---

## Core Features

## Security & Authentication Module

Security is a fundamental part of the system. The module ensures data protection and strict access control to the API.

### Main Capabilities
- **JWT-like Authentication:** Uses Bearer tokens to protect private endpoints.
- **Secure Password Storage:** Custom integration of the **bcrypt** (Blowfish) algorithm for reliable password hashing.
- **Token Management:**
  - Generates unique tokens (UUID).
  - Configurable session lifetime via the `TOKEN_TTL_SECONDS` environment variable.
  - Automatic validity and expiration checks via the `AuthFilter` middleware.
- **Automatic Hash Upgrade:** The system automatically upgrades outdated password hash algorithms upon successful user login.

---

## Entity Management System

The system provides a flexible and unified interface for managing surveillance objects.

### Managed Entities
- **Persons:** A database with photos, roles, and access status.
- **Cameras:** A registry of surveillance cameras with settings for IP addresses, locations, and stream URLs.
- **Events:** A chronology of recognition events (person ID, camera, timestamp, AI confidence).
- **Alerts:** Security notifications with severity levels.
- **Embeddings:** Vector representations of faces for further analysis and search.

### Data Handling
- **Unified DTOs:** Uses Data Transfer Objects (DTO) to standardize JSON responses.
- **Field Filtering:** Intelligent filtering automatically hides sensitive fields (passwords, tokens, deleted records) in API responses.
- **Aggregated Data:** A special endpoint `/api/all` to retrieve a full snapshot of the system state (persons, cameras, events, logs) in a single request.

---

## Database Integration

The backend is built on **PostgreSQL** using the powerful ORM from Drogon.

### Features
- **Asynchronous I/O:** All DB requests are non-blocking, allowing the handling of a large number of concurrent connections.
- **Connection Pooling:** Efficient connection pool management to optimize resources.
- **Generic Repository Pattern:** Uses templates (`TableControllerBase`, `TableRepository`) to standardize data access and reduce code duplication.
- **Dynamic Configuration:** Connections are configured via environment variables, ensuring compatibility with cloud databases (e.g., Supabase).

---

## Deployment & Configuration

The project is designed with modern deployment and containerization standards in mind.

- **Docker Support:** Includes a `Dockerfile` with multi-stage builds to create lightweight images.
- **CMake Presets:** Pre-configured build profiles for Windows (x64/x86) and Linux.
- **Cross-Platform:** Supports building on both Linux (Ubuntu) and Windows.

### Environment Variables
| Variable | Description |
| :--- | :--- |
| `SUPABASE_DB_URL` | PostgreSQL connection string (Mandatory) |
| `PORT` | Server port (default is 8080) |
| `TOKEN_TTL_SECONDS` | Authorization token lifetime (default is 24 hours) |

---

## Project Structure
```
├── CMakeLists.txt              # Main build configuration
├── Dockerfile                  # Docker image configuration
├── vcpkg.json                  # Dependencies (Drogon, libpq, jsoncpp)
└── myServer/
    ├── myServer.cpp            # Entry point (Main)
    └── src/
        ├── controllers/        # REST API Controllers
        │   ├── AuthController.h / .cpp
        │   ├── PersonController.h / .cpp
        │   ├── CameraController.h / .cpp
        │   ├── EventController.h / .cpp
        │   ├── AlertController.h / .cpp
        │   ├── SystemLogController.h / .cpp
        │   ├── EmbeddingController.h / .cpp
        │   ├── RootController.h / .cpp
        │   └── AllController.h / .cpp
        ├── dto/                # Data Transfer Objects
        │   └── EntityDtos.h / .cpp
        ├── filters/            # Middleware
        │   └── AuthFilter.h / .cpp
        ├── repositories/       # Data Access Layer
        │   ├── UserRepository.h / .cpp
        │   └── TableRepository.h / .cpp
        └── security/           # Cryptography
            ├── PasswordHasher.h / .cpp
            └── bcrypt/         # Blowfish Algorithm Implementation
```
---

## API Endpoints

The system provides clearly structured routes. Most of them are protected by the `AuthFilter` and require the `Authorization: Bearer <token>` header.

### Public Access
- **`GET /`** — Check server status and list available endpoints.
- **`POST /auth/login`** — User login and token retrieval.

### Protected Resources
- **Persons:** `GET`, `POST`, `PUT`, `DELETE` `/api/persons`.
- **Cameras:** `GET`, `POST`, `PUT`, `DELETE` `/api/cameras`.
- **Events:** `GET` `/api/events`.
- **Alerts:** `GET` `/api/alerts`.
- **System Logs:** `GET` `/api/system_logs`.
- **Embeddings:** `GET` `/api/embeddings`.
- **All Data:** `GET` `/api/all` — Aggregated response.

---

## Tech Stack

| Component | Technology |
|------------|-------------|
| Language | C++20 |
| Web Framework | Drogon |
| Database | PostgreSQL (via libpq) |
| Build System | CMake |
| Package Manager | vcpkg |
| Serialization | JsonCpp |
| Security | Bcrypt (Blowfish) |
| Containerization | Docker |

---

## Documentation

The documentation is generated using **Doxygen** and contains a complete description of all classes, methods, parameters, and usage examples.

### Documentation Format

Code comments use the tags:
`@brief` `@param` `@return` `@throws` `@example`

## Automatic Generation via GitHub Actions

After every push to the `feature/docs-ci` branch:

- a GitHub Action is triggered,

- new HTML documentation is generated,

- it is automatically published to GitHub Pages.

Workflow file:

`.github/workflows/docs.yml`

## Online Documentation

The official documentation page is available at:
https://fuw4kr.github.io/myServer/
